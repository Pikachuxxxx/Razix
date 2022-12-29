// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            RZResourceNode::RZResourceNode(const std::string_view name, uint32_t id, uint32_t resourceID, uint32_t version)
                : RZGraphNode{name, id}, m_ResourceID(resourceID), m_Version(version)
            {
            }
        }    // namespace FrameGraph

    }    // namespace Graphics
}    // namespace Razix