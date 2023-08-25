#pragma once

// [Source] : https://github.com/skaarj1989/FrameGraph

#include "Razix/Graphics/FrameGraph/RZPassNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            constexpr u32 kResourceInitialVersion{1u};

            class RZResourceNode final : public RZGraphNode
            {
                friend class RZFrameGraph;

            public:
                RZResourceNode(const std::string_view name, u32 id, u32 resourceID, u32 version);

            private:
                const u32   m_ResourceID;
                const u32   m_Version;
                RZPassNode* m_Producer = nullptr;
                RZPassNode* m_Last     = nullptr;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix