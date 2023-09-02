#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

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
                const u32   m_ResourceID = 0;       /* Index to virtual resource (m_resourceRegistry in FrameGraph) // Can we call it RZFrameGraphResource instead?   */
                const u32   m_Version    = 0;       /* Same resource can be read/written multiple time, in that case we maintain version no for each clone            */
                RZPassNode* m_Producer   = nullptr; /*  */
                RZPassNode* m_Last       = nullptr; /*  */
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix