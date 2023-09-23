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

                // Refcount management so we do this
                RAZIX_NONCOPYABLE_CLASS(RZGraphNode)

                // https://stackoverflow.com/questions/40457302/c-vector-emplace-back-calls-copy-constructor
                // According to https://en.cppreference.com/w/cpp/container/vector/emplace_back, the value_type of a std::vector<T>
                // needs to be MoveInsertable and EmplaceConstructible. MoveInsertable in particular requires a move constructor or a copy constructor
                // So, if you don't want your class to be copied, you should add an explicit move constructor to work
                // You can use = default to use the compiler-provided default implementation that just moves all fields
                // Since a Framegraph stores a vector of nodes we need to at least enable this
                RAZIX_DEFAULT_MOVABLE_CLASS(RZGraphNode)

                const std::string& getName() { return m_Name; }

            protected:
                /**
                 * Creates a node of type (Pass or Resource) using a name and unique ID 
                 */
                RZGraphNode(const std::string_view name, u32 id);

            protected:
                const std::string m_Name;          /* Name of the Node                                                                                                    */
                const u32         m_ID       = -1; /* Unique ID (should be name it RZFrameGraphResource?), matches an vector index in FG (m_PassNodes or m_ResourceNodes) */
                i32               m_RefCount = 0;  /* References count to this node in the graph                                                                          */
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
