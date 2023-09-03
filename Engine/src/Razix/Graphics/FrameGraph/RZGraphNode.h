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
             * A node in the graph(frame graph) is nothing but a vertex in the graph DS
             * A node can be 2 types either a pass that executed some code by binding resources or the resource itself
             * Input/output resources are represented in the graph as nodes in addition to the pass nodes
             */
            class RZGraphNode
            {
            public:
                // We don't want anyone except the frame graph to create graph nodes
                RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZGraphNode)

                RAZIX_VIRTUAL_DESCTURCTOR(RZGraphNode)

                // Refcount management is only done by FG so we do this
                RAZIX_NONCOPYABLE_NONMOVABLE_CLASS(RZGraphNode)

            protected:
                /**
                 * Creates a node of type (Pass or Resource) using a name and unique ID 
                 */
                RZGraphNode(const std::string_view name, u32 id);

            protected:
                const std::string m_Name;        /* Name of the Node                                                                                                    */
                const u32         m_ID;          /* Unique ID (should be name it RZFrameGraphResource?), matches an vector index in FG (m_PassNodes or m_ResourceNodes) */
                i32               m_RefCount{0}; /* References count to this node in the graph                                                                          */
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
