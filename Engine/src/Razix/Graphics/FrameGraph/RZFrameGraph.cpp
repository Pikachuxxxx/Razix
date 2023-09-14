// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraph.h"

#include <stack>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            //-----------------------------------------------------------------------------------
            // Frame Graph Class
            //-----------------------------------------------------------------------------------

            void RZFrameGraph::compile()
            {
                // TODO: Build the descriptors set tables using the flags during compilation phase

                // Set the read and write passes
                for (auto &pass: m_PassNodes) {
                    pass.m_RefCount = static_cast<int32_t>(pass.m_Writes.size());
                    for (auto &[id, flags]: pass.m_Reads) {
                        auto &consumed = m_ResourceNodes[id];
                        consumed.m_RefCount++;
                    }
                    for (auto &[id, flags]: pass.m_Writes) {
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
                        for (auto &[id, flags]: producer->m_Reads) {
                            auto &node = m_ResourceNodes[id];
                            if (--node.m_RefCount == 0) unreferencedResources.push(&node);
                        }
                    }
                }

                // Calculate resources lifetime:
                // TODO: Improve this!

                for (auto &pass: m_PassNodes) {
                    if (pass.m_RefCount == 0) continue;

                    for (auto id: pass.m_Creates)
                        getResourceEntry(id).m_Producer = &pass;
                    for (auto &[id, flags]: pass.m_Writes)
                        getResourceEntry(id).m_Last = &pass;
                    for (auto &[id, flags]: pass.m_Reads)
                        getResourceEntry(id).m_Last = &pass;
                }
            }

            void RZFrameGraph::execute(void *transientAllocator)
            {
                // Iterate though all passes and call their ExecuteFunc
                for (auto &pass: m_PassNodes) {
                    // Only it it's executable and not culled
                    if (!pass.canExecute()) continue;

                    // Call create for all the resources created by this node : Lazy Allocation --> helps with memory aliasing (pass transient resources)
                    for (const auto &id: pass.m_Creates)
                        getResourceEntry(id).getConcept()->create(transientAllocator);

                    // Call pre read and pre write functions on the resource before the execute function
                    // Safety of existence is taken care in the ResourceEntry class
                    for (auto &&[id, flags]: pass.m_Reads) {
                        if (flags != kFlagsNone)
                            getResourceEntry(id).getConcept()->preRead(flags);
                    }
                    for (auto &&[id, flags]: pass.m_Writes) {
                        if (flags != kFlagsNone)
                            getResourceEntry(id).getConcept()->preWrite(flags);
                    }

                    // call the ExecuteFunc
                    RZPassResourceDirectory resources{*this, pass};
                    std::invoke(*pass.m_Exec, resources);

                    /**
                     * Current nodes resources can still be used by other nodes so we check
                     * for the EntryPoints and see at all the resources to check which are done with this
                     * the m_Last will keep track of which node will require this resource, if this pass is done
                     * then all the resources in the framegraph that depend on this can be deleted safely
                     */
                    for (auto &entry: m_ResourceRegistry)
                        if (entry.m_Last == &pass && entry.isTransient())
                            entry.getConcept()->destroy(transientAllocator);
                }
            }

            void RZFrameGraph::resize(u32 width, u32 height)
            {
                // Iterate though all passes and call their ExecuteFunc
                for (auto &pass: m_PassNodes) {
                    // Only it it's executable and not culled
                    if (!pass.canExecute()) continue;

                    // call the ResizeFunc
                    RZPassResourceDirectory resources{*this, pass};
                    pass.m_Exec->resize(resources, width, height);
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
                    const auto &entry = m_ResourceRegistry[node.m_ResourceEntryID];
                    os << "R" << entry.m_ID << "_" << node.m_Version << " [label=<{ {<B>"
                       << node.m_Name << "</B>";
                    if (node.m_Version > kResourceInitialVersion) {
                        // FIXME: Bold text overlaps regular text
                        os << "   <FONT>v" + std::to_string(node.m_Version) + "</FONT>";
                    }
                    os << "<BR/>" << entry.getConcept()->toString() << "} | {Index: " << entry.m_ID << "<BR/>"
                       << "Refs : " << node.m_RefCount << "} }> style=filled, fillcolor="
                       << (entry.isImported() ? style.color.resource.imported
                                              : style.color.resource.transient);

                    os << "]" << std::endl;
                }
                os << std::endl;

                // -- Each pass node points to resource that it writes

                for (const RZPassNode &node: m_PassNodes) {
                    os << "P" << node.m_ID << " -> { ";
                    for (auto [id, flags]: node.m_Writes) {
                        const auto &written = m_ResourceNodes[id];
                        os << "R" << written.m_ResourceEntryID << "_" << written.m_Version << " ";
                    }
                    os << "} [color=" << style.color.edge.write << "]" << std::endl;
                }

                // -- Each resource node points to pass where it's consumed

                os << std::endl;
                for (const RZResourceNode &node: m_ResourceNodes) {
                    os << "R" << node.m_ResourceEntryID << "_" << node.m_Version << " -> { ";
                    // find all readers of this resource node
                    for (const RZPassNode &pass: m_PassNodes) {
                        for (const auto [id, flags]: pass.m_Reads)
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
                            os << "R" << r.m_ResourceEntryID << "_" << r.m_Version << " ";
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
                auto       &resource = m_ResourceRegistry[node.m_ResourceEntryID];
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
                assert(node.m_ResourceEntryID < m_ResourceRegistry.size());
                return m_ResourceRegistry[node.m_ResourceEntryID];
            }

            std::ostream &operator<<(std::ostream &os, const RZFrameGraph &fg)
            {
                fg.exportToGraphViz(os);
                return os;
            }

            RZPassNode &RZFrameGraph::createPassNode(const std::string_view name, std::unique_ptr<IRZFrameGraphPass> &&base)
            {
                const auto id = static_cast<u32>(m_PassNodes.size());
                return m_PassNodes.emplace_back(RZPassNode(name, id, std::move(base)));
            }

            RZResourceNode &RZFrameGraph::createResourceNode(const std::string_view name, u32 resourceID)
            {
                const auto id = static_cast<u32>(m_ResourceNodes.size());
                return m_ResourceNodes.emplace_back(RZResourceNode(name, id, resourceID, kResourceInitialVersion));
            }

            RZFrameGraphResource RZFrameGraph::cloneResource(RZFrameGraphResource id)
            {
                // Get the OG resource and increase it's version
                const auto &node = getResourceNode(id);
                assert(node.m_ResourceEntryID < m_ResourceRegistry.size());
                auto &entry = m_ResourceRegistry[node.m_ResourceEntryID];
                entry.m_Version++;

                // Now add it to the m_ResourceNodes array, see here we have more resources than the entry point
                // Despite being uses differently in the graph these read/write/clones have same entry point so we have a separate array for them
                const auto cloneId = static_cast<u32>(m_ResourceNodes.size());
                m_ResourceNodes.emplace_back(RZResourceNode(node.m_Name, cloneId, node.m_ResourceEntryID, entry.getVersion()));
                return cloneId;
            }

            //-----------------------------------------------------------------------------------
            // RZPassResourceBuilder Class
            //-----------------------------------------------------------------------------------

            RZFrameGraphResource RZPassResourceBuilder::read(RZFrameGraphResource id, u32 flags /*= kFlagsNone*/)
            {
                RAZIX_ASSERT(m_FrameGraph.isValid(id), "Invalid resource");
                return m_PassNode.registerResourceForRead(id, flags);
            }

            RZFrameGraphResource RZPassResourceBuilder::write(RZFrameGraphResource id, u32 flags /*= kFlagsNone*/)
            {
                RAZIX_ASSERT(m_FrameGraph.isValid(id), "Invalid resource");

                // If it writes to an imported resource mark this pass as stand alone pass
                // WHY?
                if (m_FrameGraph.getResourceEntry(id).isImported())
                    setAsStandAlonePass();

                // If the pass creates a resources it means its writing to it
                if (m_PassNode.canCreateResouce(id))
                    return m_PassNode.registerResourceForWrite(id, flags);
                else {
                    /**
                     * If it's writing to a resource not created by this PassNode
                     * then, it must first be able to read from it and then write to 
                     * a clone of the same resource 
                     */
                    m_PassNode.registerResourceForRead(id, flags);
                    // we're writing to the same external resource so clone it before writing to it
                    return m_PassNode.registerResourceForWrite(m_FrameGraph.cloneResource(id), flags);
                }
            }

            RZPassResourceBuilder &RZPassResourceBuilder::setAsStandAlonePass()
            {
                m_PassNode.m_IsStandAlone = true;
                return *this;
            }

            RZPassResourceBuilder::RZPassResourceBuilder(RZFrameGraph &frameGraph, RZPassNode &passNode)
                : m_FrameGraph(frameGraph), m_PassNode{passNode}
            {
            }

            //-----------------------------------------------------------------------------------
            // RZPassResourcesDirectory Class
            //-----------------------------------------------------------------------------------

            RZPassResourceDirectory::RZPassResourceDirectory(RZFrameGraph &frameGraph, RZPassNode &passNode)
                : m_FrameGraph(frameGraph), m_PassNode{passNode}
            {
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix