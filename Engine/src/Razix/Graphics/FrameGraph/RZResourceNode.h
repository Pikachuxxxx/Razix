#pragma once

#include "Razix/Graphics/FrameGraph/RZPassNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            constexpr uint32_t kResourceInitialVersion{1u};

            class RZResourceNode final : public RZGraphNode
            {
                friend class RZFrameGraph;

            public:
                RZResourceNode(const std::string_view name, uint32_t id, uint32_t resourceID, uint32_t version);

            private:
                const uint32_t m_ResourceID;
                const uint32_t m_Version;
                RZPassNode*    m_Producer = nullptr;
                RZPassNode*    m_Last     = nullptr;
            };
        }    // namespace FrameGraph
    }    // namespace Graphics
}    // namespace Razix