// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGraphNode.h"

namespace Razix {
    namespace Gfx {

        RZGraphNode::RZGraphNode(const RZString& name, u32 id)
            : m_Name(name), m_ID(id)
        {
        }
    }    // namespace Gfx
}    // namespace Razix