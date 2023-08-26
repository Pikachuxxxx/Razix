#include "fg/FrameGraph.hpp"
#include <stack>
#include <sstream>

namespace {

struct StyleSheet {
  bool useClusters{true};
  const char *rankDir{"TB"}; // TB, LR, BT, RL

  struct {
    const char *name{"helvetica"};
    int32_t size{10};
  } font;
  struct {
    // https://graphviz.org/doc/info/colors.html
    struct {
      const char *executed{"orange"};
      const char *culled{"lightgray"};
    } pass;
    struct {
      const char *imported{"lightsteelblue"};
      const char *transient{"skyblue"};
    } resource;
    struct {
      const char *read{"olivedrab3"};
      const char *write{"orangered"};
    } edge;
  } color;
};

} // namespace

//
// FrameGraph class:
//

void FrameGraph::reserve(uint32_t numPasses, uint32_t numResources) {
  m_passNodes.reserve(numPasses);
  m_resourceNodes.reserve(numResources);
  m_resourceRegistry.reserve(numResources);
}

bool FrameGraph::isValid(FrameGraphResource id) const {
  const auto &node = _getResourceNode(id);
  auto &resource = m_resourceRegistry[node.m_resourceId];
  return node.m_version == resource.m_version;
}

void FrameGraph::compile() {
  for (auto &pass : m_passNodes) {
    pass.m_refCount = pass.m_writes.size();
    for (auto &[id, _] : pass.m_reads) {
      auto &consumed = m_resourceNodes[id];
      consumed.m_refCount++;
    }
    for (auto &[id, _] : pass.m_writes) {
      auto &written = m_resourceNodes[id];
      written.m_producer = &pass;
    }
  }

  // -- Culling:

  std::stack<ResourceNode *> unreferencedResources;
  for (auto &node : m_resourceNodes)
    if (node.m_refCount == 0) unreferencedResources.push(&node);

  while (!unreferencedResources.empty()) {
    auto *unreferencedResource = unreferencedResources.top();
    unreferencedResources.pop();
    PassNode *producer{unreferencedResource->m_producer};
    if (producer == nullptr || producer->hasSideEffect()) continue;

    assert(producer->m_refCount >= 1);
    if (--producer->m_refCount == 0) {
      for (auto &[id, _] : producer->m_reads) {
        auto &node = m_resourceNodes[id];
        if (--node.m_refCount == 0) unreferencedResources.push(&node);
      }
    }
  }

  // -- Calculate resources lifetime:

  for (auto &pass : m_passNodes) {
    if (pass.m_refCount == 0) continue;

    for (auto id : pass.m_creates)
      _getResourceEntry(id).m_producer = &pass;
    for (auto &[id, _] : pass.m_writes)
      _getResourceEntry(id).m_last = &pass;
    for (auto &[id, _] : pass.m_reads)
      _getResourceEntry(id).m_last = &pass;
  }
}
void FrameGraph::execute(void *context, void *allocator) {
  for (auto &pass : m_passNodes) {
    if (!pass.canExecute()) continue;

    for (auto id : pass.m_creates)
      _getResourceEntry(id).create(allocator);

    for (auto &&[id, flags] : pass.m_reads) {
      if (flags != kFlagsIgnored) _getResourceEntry(id).preRead(flags, context);
    }
    for (auto &&[id, flags] : pass.m_writes) {
      if (flags != kFlagsIgnored)
        _getResourceEntry(id).preWrite(flags, context);
    }
    FrameGraphPassResources resources{*this, pass};
    std::invoke(*pass.m_exec, resources, context);

    for (auto &entry : m_resourceRegistry)
      if (entry.m_last == &pass && entry.isTransient())
        entry.destroy(allocator);
  }
}

void FrameGraph::exportGraphviz(std::ostream &os) const {
  // https://www.graphviz.org/pdf/dotguide.pdf

  static StyleSheet style;

  os << "digraph FrameGraph {" << std::endl;
  os << "graph [style=invis, rankdir=\"" << style.rankDir
     << "\" ordering=out, splines=spline]" << std::endl;
  os << "node [shape=record, fontname=\"" << style.font.name
     << "\", fontsize=" << style.font.size << ", margin=\"0.2,0.03\"]"
     << std::endl
     << std::endl;

  // -- Define pass nodes

  for (const PassNode &node : m_passNodes) {
    os << "P" << node.m_id << " [label=<{ {<B>" << node.m_name << "</B>} | {"
       << (node.hasSideEffect() ? "&#x2605; " : "")
       << "Refs: " << node.m_refCount << "<BR/> Index: " << node.m_id
       << "} }> style=\"rounded,filled\", fillcolor="
       << ((node.m_refCount > 0 || node.hasSideEffect())
             ? style.color.pass.executed
             : style.color.pass.culled);

    os << "]" << std::endl;
  }
  os << std::endl;

  // -- Define resource nodes

  for (const ResourceNode &node : m_resourceNodes) {
    const auto &entry = m_resourceRegistry[node.m_resourceId];
    os << "R" << entry.m_id << "_" << node.m_version << " [label=<{ {<B>"
       << node.m_name << "</B>";
    if (node.m_version > kResourceInitialVersion) {
      // FIXME: Bold text overlaps regular text
      os << "   <FONT>v" + std::to_string(node.m_version) + "</FONT>";
    }
    os << "<BR/>" << entry.toString() << "} | {Index: " << entry.m_id << "<BR/>"
       << "Refs : " << node.m_refCount << "} }> style=filled, fillcolor="
       << (entry.isImported() ? style.color.resource.imported
                              : style.color.resource.transient);

    os << "]" << std::endl;
  }
  os << std::endl;

  // -- Each pass node points to resource that it writes

  for (const PassNode &node : m_passNodes) {
    os << "P" << node.m_id << " -> { ";
    for (auto &[id, _] : node.m_writes) {
      const auto &written = m_resourceNodes[id];
      os << "R" << written.m_resourceId << "_" << written.m_version << " ";
    }
    os << "} [color=" << style.color.edge.write << "]" << std::endl;
  }

  // -- Each resource node points to pass where it's consumed

  os << std::endl;
  for (const ResourceNode &node : m_resourceNodes) {
    os << "R" << node.m_resourceId << "_" << node.m_version << " -> { ";
    // find all readers of this resource node
    for (const PassNode &pass : m_passNodes) {
      for (const auto &[id, _] : pass.m_reads)
        if (id == node.m_id) os << "P" << pass.m_id << " ";
    }
    os << "} [color=" << style.color.edge.read << "]" << std::endl;
  }
  os << std::endl;

  // -- Clusters:

  if (style.useClusters) {
    for (const PassNode &node : m_passNodes) {
      os << "subgraph cluster_" << node.m_id << " {" << std::endl;

      os << "P" << node.m_id << " ";
      for (auto id : node.m_creates) {
        const auto &r = m_resourceNodes[id];
        os << "R" << r.m_resourceId << "_" << r.m_version << " ";
      }
      os << std::endl << "}" << std::endl;
    }
    os << std::endl;

    os << "subgraph cluster_imported_resources {" << std::endl;
    os << "graph [style=dotted, fontname=\"helvetica\", label=< "
          "<B>Imported</B> >]"
       << std::endl;

    for (const ResourceEntry &entry : m_resourceRegistry) {
      if (entry.isImported()) os << "R" << entry.m_id << "_1 ";
    }
    os << std::endl << "}" << std::endl << std::endl;
  }

  os << "}";
}

// ---

PassNode &
FrameGraph::_createPassNode(const std::string_view name,
                            std::unique_ptr<FrameGraphPassConcept> &&base) {
  const auto id = static_cast<uint32_t>(m_passNodes.size());
  return m_passNodes.emplace_back(PassNode{name, id, std::move(base)});
}

ResourceNode &FrameGraph::_createResourceNode(const std::string_view name,
                                              uint32_t resourceId) {
  const auto id = static_cast<uint32_t>(m_resourceNodes.size());
  return m_resourceNodes.emplace_back(
    ResourceNode{name, id, resourceId, kResourceInitialVersion});
}
FrameGraphResource FrameGraph::_clone(FrameGraphResource id) {
  const auto &node = _getResourceNode(id);
  assert(node.m_resourceId < m_resourceRegistry.size());
  auto &entry = m_resourceRegistry[node.m_resourceId];
  entry.m_version++;

  const auto cloneId = static_cast<uint32_t>(m_resourceNodes.size());
  m_resourceNodes.emplace_back(
    ResourceNode{node.m_name, cloneId, node.m_resourceId, entry.getVersion()});
  return cloneId;
}

const ResourceNode &FrameGraph::_getResourceNode(FrameGraphResource id) const {
  assert(id < m_resourceNodes.size());
  return m_resourceNodes[id];
}
ResourceEntry &FrameGraph::_getResourceEntry(FrameGraphResource id) {
  const auto &node = _getResourceNode(id);
  assert(node.m_resourceId < m_resourceRegistry.size());
  return m_resourceRegistry[node.m_resourceId];
}

// ---

std::ostream &operator<<(std::ostream &os, const FrameGraph &fg) {
  fg.exportGraphviz(os);
  return os;
}

//
// FrameGraph::Builder class:
//

FrameGraphResource FrameGraph::Builder::read(FrameGraphResource id,
                                             uint32_t flags) {
  assert(m_frameGraph.isValid(id));
  return m_passNode._read(id, flags);
}
FrameGraphResource FrameGraph::Builder::write(FrameGraphResource id,
                                              uint32_t flags) {
  assert(m_frameGraph.isValid(id));
  if (m_frameGraph._getResourceEntry(id).isImported()) setSideEffect();

  if (m_passNode.creates(id)) {
    return m_passNode._write(id, flags);
  } else {
    // Writing to a texture produces a renamed handle.
    // This allows us to catch errors when resources are modified in
    // undefined order (when same resource is written by different passes).
    // Renaming resources enforces a specific execution order of the render
    // passes.
    m_passNode._read(id, kFlagsIgnored);
    return m_passNode._write(m_frameGraph._clone(id), flags);
  }
}

FrameGraph::Builder &FrameGraph::Builder::setSideEffect() {
  m_passNode.m_hasSideEffect = true;
  return *this;
}

FrameGraph::Builder::Builder(FrameGraph &fg, PassNode &node)
    : m_frameGraph{fg}, m_passNode{node} {}

//
// FrameGraphPassResources class:
//

FrameGraphPassResources::FrameGraphPassResources(FrameGraph &fg, PassNode &node)
    : m_frameGraph{fg}, m_passNode{node} {}
