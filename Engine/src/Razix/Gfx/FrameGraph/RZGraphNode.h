#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

namespace Razix {
    namespace Gfx {
        class RAZIX_API RZGraphNode
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

            inline const RZString& getName() const { return m_Name; }
            inline u32             getID() const { return m_ID; }
            inline u32             getRefCount() const { return m_RefCount; }

        protected:
            RZGraphNode(const RZString& name, u32 id);

        protected:
            RZString m_Name;
            u32      m_ID       = -1;
            i32      m_RefCount = 0;
        };
    }    // namespace Gfx
}    // namespace Razix
