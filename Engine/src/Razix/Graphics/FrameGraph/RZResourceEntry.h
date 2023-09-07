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

            RAZIX_CHECK_TYPE_HAS_FUNCTION(T, preRead)
            RAZIX_CHECK_TYPE_HAS_FUNCTION(T, preWrite)

            // [Type Erasure] Source 1 : https://madptr.com/posts/2023-06-24-typeerasureintro/
            // [SFINAE] Source 1 : https://en.cppreference.com/w/cpp/language/sfinae
            // [SFINAE] Source 2 :  https://www.cppstories.com/2016/02/notes-on-c-sfinae/
            // [SFINAE] Source 3 :  https://jguegant.github.io/blogs/tech/sfinae-introduction.html

            /**
             * Type Erasure class to hold different types of Graphics API resources using a wrapped class under one umbrella
             * 
             * So instead of calling a interface we use types to resolve the implementation and call stuff
             * 
             * Of course we can have this common IRZResource<T> and not have an entry point, but we don't have a common handle for all resources, 
             * this is not possible and we don't encourage storing IRZResources* anywhere except in a Pool (Transient in this case) and also 
             * anything other than the Graphics API can only interact via Handles! and it will get messy even if we do so!
             * Graphics API is always DECOUPLED from anything else, it's fixed!!!
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
                    virtual void create()  = 0;
                    virtual void destroy() = 0;

                    // Optional functions so we don't check for existence of these functions on the type rather on model before calling them
                    virtual void preRead(uint32_t flags)  = 0;
                    virtual void preWrite(uint32_t flags) = 0;

                    virtual std::string toString() const = 0;
                };

                /**
                 * Implementation for concept
                 */
                template<typename T>
                struct Model final : Concept
                {
                    /**
                     * constructor for type, usually a pointer is stored but here we are taking a universal reference + a data member which we enforce rules on it to have
                     * 
                     * Since this is designed specifically for the Graphics API they need a Desc to create them, now hiding the Handle as a pointer and using this entry to 
                     * create them is not possible so we store the Handle as a universal reference because it's not a opaque data type and enforce a T::Desc in resources creation.
                     * A little overhead for storage but it is what it is, instead of giving this struct in stack while creating we store it as data member
                     * 
                     * PROBLEM: How do we know the resource has the method that we are calling from here? Pure virtual functions check is gone
                     * To solve this we use STL magic to verify they have some methods 
                     * SFINAE - "Substitution Failure Is Not An Error"
                     * SFINAE uses to create catered template functions that only accepts types that we enforce the concept rules on
                     * and will reject non-matching overloads for types without any error, SFINAE's safe failure can choose the different
                     * paths during compile time to tell whether a type has a method/sub type or not and these compile time expression can be used for final evaluation
                     */
                    Model(typename T::Desc &&desc, T &&obj)
                        : descriptor(std::move(desc)), resource(std::move(obj))
                    {
                    }

                    /**
                     * We pass an extra argument only possible through model,this extra arg comes from the frame graph that is needed by the FrameGraphTResource
                     * This is a little deviation from the Concept signature, since actual FG resource creation happens at frame graph and these methods are called later
                     * it is allowed to be different and this descriptor (T::Desc) is actually need to create the Graphics API and this is the only way to pass to it from FG
                     * 
                     * FrameGraph common create calls this create and some args from the first method are passed here later
                     */

                    void create() final
                    {
                        resource.create(descriptor);
                    }

                    void destroy() final
                    {
                        resource.destroy(descriptor);
                    }

                    /**
                     * The flags are given to concept from frame graph when which as passes as args to the read/write methods of the FrameGraph class
                     */

                    void preRead(uint32_t flags)
                    {
                        // Since these functions are optional for a resource to have and not enforce we check here before calling them
                        if constexpr (RAZIX_TYPE_HAS_FUNCTION_V(T, preRead))
                            resource.preRead(descriptor, flags);
                    }

                    void preWrite(uint32_t flags)
                    {
                        // Since these functions are optional for a resource to have and not enforce we check here before calling them
                        if constexpr (RAZIX_TYPE_HAS_FUNCTION_V(T, preWrite))
                            resource.preWrite(descriptor, flags);
                    }

                    std::string toString() const final
                    {
                        resource.to_string(descriptor);
                    }

                    T                      resource;   /* Resource handle              */
                    typename const T::Desc descriptor; /* Resource creation descriptor */
                };

            public:
                /* Only Frame Graph can create the resources */
                RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZResourceEntry)

                RAZIX_NONCOPYABLE_CLASS(RZResourceEntry)

                template<typename T>
                RAZIX_NO_DISCARD T &get()
                {
                    getModel<T>()->resource;
                }

                template<typename T>
                typename const T::Desc &getDescriptor() const
                {
                    getModel<T>()->descriptor;
                }

                RAZIX_NO_DISCARD u32  getVersion() const { return m_Version; }
                RAZIX_NO_DISCARD bool isImported() const { return m_Imported; }
                RAZIX_NO_DISCARD bool isTransient() const { return !m_Imported; }

            private:
                //---------------------------------
                std::unique_ptr<Concept> m_Concept; /* Type Erased implementation class */
                //---------------------------------
                const u32   m_ID;                 /* Index of the resource, usually same as FreamGraphResource to identify it   */
                const bool  m_Imported = false;   /* Whether or not the resource has been imported                              */
                u32         m_Version;            /* Version of the cloned resource                                             */
                RZPassNode *m_Producer = nullptr; /* Pass Node who writes to this resources */
                RZPassNode *m_Last     = nullptr; /* Next Pass Node that will read this resource  */

            private:
                template<typename T>
                RZResourceEntry(u32 id, typename T::Desc &&desc, T &&obj, u32 version, bool imported = false)
                    : m_ID(id), m_Concept{std::make_unique<Model<T>>(std::forward<typename T::Desc>(desc), std::forward<T>(obj))}, m_Version(version), m_Imported(imported)
                {
                }

                template<typename T>
                RAZIX_NO_DISCARD Model<T> *getModel() const
                {
                    auto *model = dynamic_cast<Model<T> *>(m_Concept.get());
                    return model;
                }

                RAZIX_NO_DISCARD Concept *getConcept() const
                {
                    return m_Concept.get();
                }
            };
            //-----------------------------------------------------------------------------------
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix