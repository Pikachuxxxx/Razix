// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraph.h"

#include <stack>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            //-----------------------------------------------------------------------------------
            // Builder Class
            //-----------------------------------------------------------------------------------

            ENFORCE_CONCEPT_IMPL inline RZFrameGraphResource RZFrameGraph::RZBuilder::create(const std::string_view name, typename T::CreateDesc &&desc)
            {
                const auto id = m_FrameGraph.create<T>(name, std::move(desc));
                return m_PassNode.m_Creates.emplace_back(id);
            }

            RZFrameGraphResource RZFrameGraph::RZBuilder::read(RZFrameGraphResource id)
            {
                return m_PassNode.read(id);
            }

            RZFrameGraphResource RZFrameGraph::RZBuilder::write(RZFrameGraphResource id)
            {
                return m_PassNode.write(id);
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

            template<typename PassData, typename SetupFunc, typename ExecuteFunc>
            const PassData &Razix::Graphics::FrameGraph::RZFrameGraph::addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
            {
            }

            template<typename SetupFunc, typename ExecuteFunc>
            void Razix::Graphics::FrameGraph::RZFrameGraph::addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
            {
            }

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
                        getResourceEntry(id).create(allocator);

                    RZFrameGraphPassResources resources{*this, pass};
                    std::invoke(*pass.m_Exec, resources, renderContext);

                    for (auto &entry: m_ResourceRegistry)
                        if (entry.m_Last == &pass && entry.isTransient())
                            entry.destroy(allocator);
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

            ENFORCE_CONCEPT_IMPL RZFrameGraphResource RZFrameGraph::import(const std::string_view name, typename T::CreateDesc &&, T &&)
            {
                const auto resourceId = static_cast<uint32_t>(m_ResourceRegistry.size());
                m_ResourceRegistry.emplace_back(
                    RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), std::forward<T>(resource), kResourceInitialVersion, true});
                return createResourceNode(name, resourceId).m_id;
            }

            bool RZFrameGraph::isValid(RZFrameGraphResource id)
            {
                const auto &node     = getResourceNode(id);
                auto       &resource = m_ResourceRegistry[node.m_ResourceID];
                return node.m_Version == resource.m_Version;
            }

            ENFORCE_CONCEPT_IMPL typename const T::CreateDesc &RZFrameGraph::getDescriptor(RZFrameGraphResource id)
            {
                return getResourceEntry(id).getTypeResource()<T>()->descriptor;
            }

            const Razix::Graphics::FrameGraph::RZResourceNode &RZFrameGraph::getResourceNode(RZFrameGraphResource id) const
            {
                assert(id < m_ResourceNodes.size());
                return m_ResourceNodes[id];
            }

            Razix::Graphics::FrameGraph::RZResourceEntry &RZFrameGraph::getResourceEntry(RZFrameGraphResource id)
            {
                const auto &node = getResourceNode(id);
                assert(node.m_ResourceID < m_ResourceRegistry.size());
                return m_ResourceRegistry[node.m_ResourceID];
            }

            Razix::Graphics::FrameGraph::RZPassNode &RZFrameGraph::createPassNode(const std::string_view name, std::unique_ptr<RZFrameGraphPassConcept> &&base)
            {
                const auto id = static_cast<uint32_t>(m_PassNodes.size());
                return m_PassNodes.emplace_back(RZPassNode(name, id, std::move(base)));
            }

            ENFORCE_CONCEPT_IMPL RZFrameGraphResource RZFrameGraph::createResource(const std::string_view name, typename T::CreateDesc &&desc)
            {
                const auto resourceId = static_cast<uint32_t>(m_ResourceRegistry.size());
                m_ResourceRegistry.emplace_back(RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), T{}, kResourceInitialVersion});
                return createResourceNode(name, resourceId).m_ID;
            }

            RZResourceNode &RZFrameGraph::createResourceNode(const std::string_view name, uint32_t resourceID)
            {
                const auto id = static_cast<uint32_t>(m_ResourceNodes.size());
                return m_ResourceNodes.emplace_back(RZResourceNode{name, id, resourceID, kResourceInitialVersion});
            }

            Razix::Graphics::FrameGraph::RZFrameGraphResource RZFrameGraph::cloneResource(RZFrameGraphResource id)
            {
                const auto &node = getResourceNode(id);
                assert(node.m_ResourceID < m_ResourceRegistry.size());
                auto &entry = m_ResourceRegistry[node.m_ResourceID];
                entry.m_Version++;

                const auto cloneId = static_cast<uint32_t>(m_ResourceNodes.size());
                m_ResourceNodes.emplace_back(RZResourceNode{node.m_Name, cloneId, node.m_ResourceID, entry.getVersion()});
                return cloneId;
            }

            //-----------------------------------------------------------------------------------
            // RZFrameGraphPassResources Class
            //-----------------------------------------------------------------------------------

            RZFrameGraphPassResources::RZFrameGraphPassResources(RZFrameGraph &fg, RZPassNode &passNode)
                : m_FrameGraph(fg), m_PassNode(passNode)
            {
            }

            ENFORCE_CONCEPT_IMPL T &RZFrameGraphPassResources::get(RZFrameGraphResource id)
            {
                assert(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) ||
                       m_PassNode.canWriteResouce(id));
                return m_FrameGraph.getResourceEntry(id).get<T>();
            }

            ENFORCE_CONCEPT_IMPL typename const T::CreateDesc &RZFrameGraphPassResources::getDescriptor(RZFrameGraphResource id) const
            {
                assert(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) ||
                       m_PassNode.canWriteResouce(id));
                return m_FrameGraph.getResourceEntry(id).getDescriptor<T>();
            }

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix