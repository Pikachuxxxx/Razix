#pragma once

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
                const u32    m_ID;          /* Unique id, matches an array index in FrameGraph */
                int32_t           m_RefCount{0}; /* References to this node in the graph            */
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
