#pragma once

#include "Razix/Graphics/FrameGraph/RZPassNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            // http://www.cplusplus.com/articles/oz18T05o/
            // https://www.modernescpp.com/index.php/c-core-guidelines-type-erasure-with-templates

            /**
         * Wrapper around a virtual resource and it's entry point for creation and execution
         */
            class RZResourceEntry final
            {
                friend class RZFrameGraph;

                // Define custom types here

                /**
             * Concept is used to enforce certain rules on the Resources 
             */
                struct RZConcept
                {
                    virtual ~RZConcept() = default;

                    virtual void create(void *)  = 0;
                    virtual void destroy(void *) = 0;

                    virtual std::string toString() const = 0;
                };

                template<typename T>
                struct RZTypeResource : RZConcept
                {
                    RZTypeResource(typename T::Desc &&, T &&);

                    void create(void *allocator) final;
                    void destroy(void *allocator) final;

                    std::string toString() const final;

                    // Resource creation descriptor
                    typename const T::Desc descriptor;

                    // Resource handle
                    T resource;
                };

            public:
                RZResourceEntry(RZResourceEntry &&) noexcept            = default;
                RZResourceEntry &operator=(RZResourceEntry &&) noexcept = default;

                RZResourceEntry()                                   = delete;
                RZResourceEntry(const RZResourceEntry &)            = delete;
                RZResourceEntry &operator=(const RZResourceEntry &) = delete;

                void create(void *allocator);
                void destroy(void *allocator);

                std::string toString() const;

                u32 getVersion() const { return m_Version; }
                bool     isImported() const { return m_Imported; }
                bool     isTransient() const { return !m_Imported; }

                template<typename T>
                T &get();

                template<typename T>
                typename const T::Desc &getDescriptor() const;

            private:
                const u32             m_ID;
                std::unique_ptr<RZConcept> m_Concept;
                const bool                 m_Imported = false;
                u32                   m_Version;
                RZPassNode                *m_Producer = nullptr;
                RZPassNode                *m_Last     = nullptr;

            private:
                template<typename T>
                RZResourceEntry(u32 id, typename T::Desc &&, T &&, u32 version, bool imported = false);

                template<typename T>
                auto *getTypeResource() const;
            };

#include "RZResourceEntry.inl"

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix