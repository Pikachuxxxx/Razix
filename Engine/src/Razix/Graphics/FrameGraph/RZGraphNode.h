#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {
            /**
             * A node in the frame graph
             */
            class RZGraphNode
            {
            public:
                RZGraphNode(RZGraphNode &&) noexcept = default;
                virtual ~RZGraphNode()               = default;

                RZGraphNode()                                   = delete;
                RZGraphNode(const RZGraphNode &)                = delete;
                RZGraphNode &operator=(const RZGraphNode &)     = delete;
                RZGraphNode &operator=(RZGraphNode &&) noexcept = delete;

            protected:
                RZGraphNode(const std::string_view name, u32 id);

            protected:
                const std::string m_Name;        /* Name of the Node                                */
                const u32         m_ID;          /* Unique id, matches an array index in FrameGraph */
                int32_t           m_RefCount{0}; /* References to this node in the graph            */
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
