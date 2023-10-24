\// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPassNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            static RAZIX_NO_DISCARD bool hasId(const std::vector<RZFrameGraphResource> &v, RZFrameGraphResource id)
            {
#if __cpp_lib_ranges
                return std::ranges::find(v, id) != v.cend();
#else
                return std::find(v.cbegin(), v.cend(), id) != v.cend();
#endif
            }

            static RAZIX_NO_DISCARD bool hasId(const std::vector<RZFrameGraphResourceAcessView> &v, RZFrameGraphResource id)
            {
                const auto match = [id](const auto &e) { return e.id == id; };

#if __cpp_lib_ranges
                return std::ranges::find_if(v, match) != v.cend();
#else
                return std::find_if(v.cbegin(), v.cend(), match) != v.cend();
#endif
            }

            // TODO: Add a function check to check not just the ID but also the complete pair ID and flags
            // because when we want to mark a resource as read/write onto a pass it can have multiple access
            // views with different read/write properties, so check for the complete pair just in case
            // hasId is not as safe as contains but it's a lil faster so we only use it for existing resources

            //---------------------------------------------------------------------------

            RZPassNode::RZPassNode(const std::string_view name, u32 id, std::unique_ptr<IRZFrameGraphPass> &&exec)
                : RZGraphNode{name, id}, m_Exec{std::move(exec)}
            {
                m_Creates.reserve(10);
                m_Reads.reserve(10);
                m_Writes.reserve(10);
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

            bool RZPassNode::canExecute() const
            {
                return m_RefCount > 0 || isStandAlone();
            }

            RZFrameGraphResource RZPassNode::registerResourceForRead(RZFrameGraphResource id, u32 flags)
            {
                RAZIX_CORE_ASSERT((!canCreateResouce(id) && !canWriteResouce(id)), "Cannot read a resource that this node creates or write to!");
                return canReadResouce(id) ? id : m_Reads.emplace_back(id, flags).id;
            }

            RZFrameGraphResource RZPassNode::registerResourceForWrite(RZFrameGraphResource id, u32 flags)
            {
                return canWriteResouce(id) ? id : m_Writes.emplace_back(id, flags).id;
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix