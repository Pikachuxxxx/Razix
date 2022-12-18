// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPassNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            [[nodiscard]] bool hasId(const std::vector<RZFrameGraphResource> &v, RZFrameGraphResource id)
            {
#if __cpp_lib_ranges
                return std::ranges::find(v, id) != v.cend();
#else
                return std::find(v.cbegin(), v.cend(), id) != v.cend();
#endif
            }

            bool RZPassNode::canCreateResouce(RZFrameGraphResource resourceID) const
            {
                return hasId(m_Creates, resourceID);
            }

            bool RZPassNode::canReadResouce(RZFrameGraphResource resourceID) const
            {
                return hasId(m_Reads, resourceID);
            }

            bool RZPassNode::canWriteResouce(RZFrameGraphResource resourceID) const
            {
                return hasId(m_Writes, resourceID);
            }

            bool RZPassNode::isStandAlone() const
            {
                return m_IsStandAlone;
            }

            bool RZPassNode::canExecute() const
            {
                return m_RefCount > 0 || isStandAlone();
            }

            RZPassNode::RZPassNode(const std::string_view name, uint32_t id, std::unique_ptr<RZFrameGraphPassConcept> &&exec)
                : RZGraphNode{name, id}, m_Exec{std::move(exec)}
            {
            }

            Razix::Graphics::FrameGraph::RZFrameGraphResource RZPassNode::read(RZFrameGraphResource id)
            {
                RAZIX_CORE_ASSERT((!canCreateResouce(id) && !canWriteResouce(id)), "No Create and Write resources for the pass");
                return canReadResouce(id) ? id : m_Reads.emplace_back(id);
            }

            Razix::Graphics::FrameGraph::RZFrameGraphResource RZPassNode::write(RZFrameGraphResource id)
            {
                return canReadResouce(id) ? id : m_Writes.emplace_back(id);
            }
        }    // namespace FrameGraph
    }    // namespace Graphics
}    // namespace Razix