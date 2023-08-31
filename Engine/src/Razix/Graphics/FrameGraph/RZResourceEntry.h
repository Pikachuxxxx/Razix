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

            // [Type Erasure] Source 1 : https://madptr.com/posts/2023-06-24-typeerasureintro/
            // [Type Erasure] Source 2 : https://davekilian.com/cpp-type-erasure.html

            /**
             * Type Erasure class to hold different types of Graphics API resources using a wrapped class under on umbrella
             * 
             * Of course we can have this common IRZResource<T> and not have an entry point, but we don't have a common handle for all resources, 
             * this is not possible and we don't encourage storing IRZResources* anywhere except in a Pool (Transient in this case) and also 
             * anything other than the Graphics API can only interact via Handles! and it will get messy even if we do so!
             * Graphics API is always DECOPLED from anything else, it's fixed!!!
             * 
             * Think like this, the Graphics API classes are itself polymorphic, now again enforcing rules on those interfaces restricts how 
             * they are created, this way we have minimal rules from the TypeErasure concept so decoupling is necessary and also we have reached
             * the upper limit of combining under a single type for Graphics API
             * 
             * This is why the T passes as resource is neither directly the Graphics API nor is just the Handle<U> because by itself it's of no use
             * Hence we have wrappers around Handle<U> and pass that wrapper FrameGraphTResource as T to the EntryPoint class
             */
            class RZResourceEntry final
            {
                // Only Frame Graph can create these and store
                friend class RZFrameGraph;

                // Define custom types here

                /**
                 * Concept is used to enforce certain rules in Type Erasure, like a fake interface for all types without making anyone derive from them
                 */
                struct Concept
                {
                    virtual void create(void *)  = 0;
                    virtual void destroy(void *) = 0;

                    virtual std::string toString() const = 0;
                };

                /**
                 * Implementation for concept
                 */
                template<typename T>
                struct Model : Concept
                {
                    /**
                     * constructor for type, usually a pointer is stored but here we are taking a universal reference + a data member which we enforce rules on it to have
                     * 
                     * Since this is designed specifically for the Graphics API they need a Desc to create them, now hiding the Handle as a pointer and using this entry to 
                     * create them is not possible so we store the Handle as a universal reference because it's not a opaque data type and enforce a T::Desc in resources creation.
                     * A little overhead for storage but it is what it is, instead of giving this struct in stack while creating we store it as data member
                     */
                    Model(typename T::Desc &&, T &&)
                    {
                    }

                    void create(void *allocator) final
                    {
                    }
                    void destroy(void *allocator) final
                    {
                    }

                    std::string toString() const final
                    {
                    }

                    // Resource creation descriptor
                    typename const T::Desc descriptor;

                    // Resource handle
                    T resource;
                };

            public:
                RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZResourceEntry)

                RAZIX_NONCOPYABLE_CLASS(RZResourceEntry)

                //------------------------------------------------------------
                // Must be same as concept to be used by the external world because this is the real world public face for concept and model
                // TODO: Derive this from Concept?
                void create(void *allocator);
                void destroy(void *allocator);

                std::string toString() const;
                //------------------------------------------------------------

                // methods to access the T
                template<typename T>
                T &get();

                template<typename T>
                typename const T::Desc &getDescriptor() const;

                u32  getVersion() const { return m_Version; }
                bool isImported() const { return m_Imported; }
                bool isTransient() const { return !m_Imported; }

            private:
                //---------------------------------
                rzstl::UniqueRef<Concept> m_Concept;    // Type erased implementation class
                //---------------------------------
                const u32   m_ID;
                const bool  m_Imported = false;
                u32         m_Version;
                RZPassNode *m_Producer = nullptr;
                RZPassNode *m_Last     = nullptr;

            private:
                template<typename T>
                RZResourceEntry(u32 id, typename T::Desc &&, T &&, u32 version, bool imported = false);

                template<typename T>
                auto *getModel() const;
            };

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix