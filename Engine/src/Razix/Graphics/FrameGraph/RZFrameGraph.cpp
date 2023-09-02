// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraph.h"

// [Source] : https://github.com/skaarj1989/FrameGraph

#include <stack>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            //-----------------------------------------------------------------------------------
            // Builder Class
            //-----------------------------------------------------------------------------------

            RZFrameGraphResource RZFrameGraph::RZBuilder::read(RZFrameGraphResource id, u32 flags)
            {
                return m_PassNode.registerResourceForRead(id, flags);
            }

            RZFrameGraphResource RZFrameGraph::RZBuilder::write(RZFrameGraphResource id, u32 flags)
            {
                if (m_FrameGraph.getResourceEntry(id).isImported())
                    setAsStandAlonePass();

                if (m_PassNode.canCreateResouce(id)) {
                    return m_PassNode.registerResourceForWrite(id, flags);
                } else {
                    // Writing to a texture produces a renamed handle.
                    // This allows us to catch errors when resources are modified in
                    // undefined order (when same resource is written by different passes).
                    // Renaming resources enforces a specific execution order of the render
                    // passes.
                    m_PassNode.registerResourceForRead(id);
                    return m_PassNode.registerResourceForWrite(m_FrameGraph.cloneResource(id));
                }
            }

            RZFrameGraph::RZBuilder &RZFrameGraph::RZBuilder::setAsStandAlonePass()
            {
                m_PassNode.m_IsStandAlone = true;
                return *this;
            }

            RZFrameGraph::RZBuilder::RZBuilder(RZFrameGraph &fg, RZPassNode &pass)
                : m_FrameGraph(fg), m_PassNode(pass)
            {
            }

            //-----------------------------------------------------------------------------------
            // Frame Graph Class
            //-----------------------------------------------------------------------------------

            void RZFrameGraph::compile()
            {
                // Set the read and write passes
                for (auto &pass: m_PassNodes) {
                    pass.m_RefCount = static_cast<int32_t>(pass.m_Writes.size());
                    for (auto &id: pass.m_Reads) {
                        auto &consumed = m_ResourceNodes[id];
                        consumed.m_RefCount++;
                    }
                    for (auto id: pass.m_Writes) {
                        auto &written      = m_ResourceNodes[id];
                        written.m_Producer = &pass;
                    }
                }

                // Culling
                std::stack<RZResourceNode *> unreferencedResources;
                for (auto &node: m_ResourceNodes)
                    if (node.m_RefCount == 0) unreferencedResources.push(&node);

                while (!unreferencedResources.empty()) {
                    auto *unreferencedResource = unreferencedResources.top();
                    unreferencedResources.pop();
                    RZPassNode *producer{unreferencedResource->m_Producer};
                    if (producer == nullptr || producer->isStandAlone()) continue;

                    assert(producer->m_RefCount >= 1);
                    if (--producer->m_RefCount == 0) {
                        for (auto id: producer->m_Reads) {
                            auto &node = m_ResourceNodes[id];
                            if (--node.m_RefCount == 0) unreferencedResources.push(&node);
                        }
                    }
                }

                // Calculate resources lifetime:

                for (auto &pass: m_PassNodes) {
                    if (pass.m_RefCount == 0) continue;

                    for (auto id: pass.m_Creates)
                        getResourceEntry(id).m_Producer = &pass;
                    for (auto id: pass.m_Writes)
                        getResourceEntry(id).m_Last = &pass;
                    for (auto id: pass.m_Reads)
                        getResourceEntry(id).m_Last = &pass;
                }
            }

            void RZFrameGraph::execute(void *renderContext /*= nullptr*/, void *allocator /*= nullptr*/)
            {
                for (auto &pass: m_PassNodes) {
                    if (!pass.canExecute()) continue;

                    for (auto id: pass.m_Creates)
                        getResourceEntry(id).getConcept()->create();

                    RZFrameGraphPassResourcesDirectory resources{*this, pass};
                    std::invoke(*pass.m_Exec, resources, renderContext);

                    for (auto &entry: m_ResourceRegistry)
                        if (entry.m_Last == &pass && entry.isTransient())
                            entry.getConcept()->destroy();
                }
            }

            struct StyleSheet
            {
                bool        useClusters{true};
                const char *rankDir{"TB"};    // TB, LR, BT, RL

                struct
                {
                    const char *name{"helvetica"};
                    int32_t     size{10};
                } font;
                struct
                {
                    // https://graphviz.org/doc/info/colors.html
                    struct
                    {
                        const char *executed{"orange"};
                        const char *culled{"lightgray"};
                    } pass;
                    struct
                    {
                        const char *imported{"lightsteelblue"};
                        const char *transient{"skyblue"};
                    } resource;
                    struct
                    {
                        const char *read{"olivedrab3"};
                        const char *write{"orangered"};
                    } edge;
                } color;
            };

            void RZFrameGraph::exportToGraphViz(std::ostream &os) const
            {
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

                for (const RZPassNode &node: m_PassNodes) {
                    os << "P" << node.m_ID << " [label=<{ {<B>" << node.m_Name << "</B>} | {"
                       << (node.isStandAlone() ? "&#x2605; " : "")
                       << "Refs: " << node.m_RefCount << "<BR/> Index: " << node.m_ID
                       << "} }> style=\"rounded,filled\", fillcolor="
                       << ((node.m_RefCount > 0 || node.isStandAlone())
                                  ? style.color.pass.executed
                                  : style.color.pass.culled);

                    os << "]" << std::endl;
                }
                os << std::endl;

                // -- Define resource nodes

                for (const RZResourceNode &node: m_ResourceNodes) {
                    const auto &entry = m_ResourceRegistry[node.m_ResourceID];
                    os << "R" << entry.m_ID << "_" << node.m_Version << " [label=<{ {<B>"
                       << node.m_Name << "</B>";
                    if (node.m_Version > kResourceInitialVersion) {
                        // FIXME: Bold text overlaps regular text
                        os << "   <FONT>v" + std::to_string(node.m_Version) + "</FONT>";
                    }
                    os << "<BR/>" << entry.toString() << "} | {Index: " << entry.m_ID << "<BR/>"
                       << "Refs : " << node.m_RefCount << "} }> style=filled, fillcolor="
                       << (entry.isImported() ? style.color.resource.imported
                                              : style.color.resource.transient);

                    os << "]" << std::endl;
                }
                os << std::endl;

                // -- Each pass node points to resource that it writes

                for (const RZPassNode &node: m_PassNodes) {
                    os << "P" << node.m_ID << " -> { ";
                    for (auto id: node.m_Writes) {
                        const auto &written = m_ResourceNodes[id];
                        os << "R" << written.m_ResourceID << "_" << written.m_Version << " ";
                    }
                    os << "} [color=" << style.color.edge.write << "]" << std::endl;
                }

                // -- Each resource node points to pass where it's consumed

                os << std::endl;
                for (const RZResourceNode &node: m_ResourceNodes) {
                    os << "R" << node.m_ResourceID << "_" << node.m_Version << " -> { ";
                    // find all readers of this resource node
                    for (const RZPassNode &pass: m_PassNodes) {
                        for (const auto id: pass.m_Reads)
                            if (id == node.m_ID) os << "P" << pass.m_ID << " ";
                    }
                    os << "} [color=" << style.color.edge.read << "]" << std::endl;
                }
                os << std::endl;

                // -- Clusters:

                if (style.useClusters) {
                    for (const RZPassNode &node: m_PassNodes) {
                        os << "subgraph cluster_" << node.m_ID << " {" << std::endl;

                        os << "P" << node.m_ID << " ";
                        for (auto id: node.m_Creates) {
                            const auto &r = m_ResourceNodes[id];
                            os << "R" << r.m_ResourceID << "_" << r.m_Version << " ";
                        }
                        os << std::endl
                           << "}" << std::endl;
                    }
                    os << std::endl;

                    os << "subgraph cluster_imported_resources {" << std::endl;
                    os << "graph [style=dotted, fontname=\"helvetica\", label=< "
                          "<B>Imported</B> >]"
                       << std::endl;

                    for (const RZResourceEntry &entry: m_ResourceRegistry) {
                        if (entry.isImported()) os << "R" << entry.m_ID << "_1 ";
                    }
                    os << std::endl
                       << "}" << std::endl
                       << std::endl;
                }

                os << "}";
            }

            bool RZFrameGraph::isValid(RZFrameGraphResource id)
            {
                const auto &node     = getResourceNode(id);
                auto       &resource = m_ResourceRegistry[node.m_ResourceID];
                return node.m_Version == resource.m_Version;
            }

            const RZResourceNode &RZFrameGraph::getResourceNode(RZFrameGraphResource id) const
            {
                assert(id < m_ResourceNodes.size());
                return m_ResourceNodes[id];
            }

            RZResourceEntry &RZFrameGraph::getResourceEntry(RZFrameGraphResource id)
            {
                const auto &node = getResourceNode(id);
                assert(node.m_ResourceID < m_ResourceRegistry.size());
                return m_ResourceRegistry[node.m_ResourceID];
            }

            std::ostream &operator<<(std::ostream &os, const RZFrameGraph &fg)
            {
                fg.exportToGraphViz(os);
                return os;
            }

            RZPassNode &RZFrameGraph::createPassNode(const std::string_view name, std::unique_ptr<RZFrameGraphPassConcept> &&base)
            {
                const auto id = static_cast<u32>(m_PassNodes.size());
                return m_PassNodes.emplace_back(RZPassNode(name, id, std::move(base)));
            }

            RZResourceNode &RZFrameGraph::createResourceNode(const std::string_view name, u32 resourceID)
            {
                const auto id = static_cast<u32>(m_ResourceNodes.size());
                return m_ResourceNodes.emplace_back(RZResourceNode{name, id, resourceID, kResourceInitialVersion});
            }

            Razix::Graphics::FrameGraph::RZFrameGraphResource RZFrameGraph::cloneResource(RZFrameGraphResource id)
            {
                const auto &node = getResourceNode(id);
                assert(node.m_ResourceID < m_ResourceRegistry.size());
                auto &entry = m_ResourceRegistry[node.m_ResourceID];
                entry.m_Version++;

                const auto cloneId = static_cast<u32>(m_ResourceNodes.size());
                m_ResourceNodes.emplace_back(RZResourceNode{node.m_Name, cloneId, node.m_ResourceID, entry.getVersion()});
                return cloneId;
            }

            //-----------------------------------------------------------------------------------
            // RZFrameGraphPassResources Class
            //-----------------------------------------------------------------------------------

            RZFrameGraphPassResourcesDirectory::RZFrameGraphPassResourcesDirectory(RZFrameGraph &fg, RZPassNode &passNode)
                : m_FrameGraph(fg), m_PassNode(passNode)
            {
            }

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix