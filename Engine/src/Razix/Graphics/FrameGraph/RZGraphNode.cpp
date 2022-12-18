// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGraphNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            RZGraphNode::RZGraphNode(const std::string_view name, uint32_t id)
                : m_Name(name), m_ID(id)
            {
            }
        }    // namespace FrameGraph
    }    // namespace Graphics
}    // namespace Razix