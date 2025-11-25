
#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include "Razix/Gfx/FrameGraph/RZPassNode.h"

namespace Razix {
    namespace Gfx {

        constexpr u32 kRESOURCE_INITIAL_VERSION = 1;

        class RAZIX_API RZResourceNode final : public RZGraphNode
        {
        private:
            friend class RZFrameGraph;

            /**
              * Create a resource node with a name, it's unique ID in the graph and ID to resource entry for the recourse it refers to and it's version
              * 
              * Because the resources can be cloned and have multiple instance in the graph they need another ID to point to it's entry point in FG
              */
            RZResourceNode(const RZString& name, u32 id, u32 resourceID, u32 version);

        public:
            inline u32 getResourceEntryId() const { return m_ResourceEntryID; }
            inline u32 getVersion() const { return m_Version; }

        private:
            const u32   m_ResourceEntryID = 0;                         /* Index to resource entry point (m_resourceRegistry in FrameGraph)                                    */
            const u32   m_Version         = kRESOURCE_INITIAL_VERSION; /* Same resource can be read/written multiple time, in that case we maintain version no for each clone */
            RZPassNode* m_Producer        = nullptr;                   /* Pass Node who writes to this resources, used to create edges in graph                               */
            RZPassNode* m_Last            = nullptr;                   /* Next Pass Node that will read this resource, used to create edges in graph                          */
        };
    }    // namespace Gfx
}    // namespace Razix