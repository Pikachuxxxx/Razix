// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceNode.h"

namespace Razix {
    namespace Gfx {

        RZResourceNode::RZResourceNode(const std::string_view name, u32 id, u32 resourceID, u32 version)
            : RZGraphNode(name, id), m_ResourceEntryID(resourceID), m_Version(version)
        {
        }
    }    // namespace Gfx
}    // namespace Razix