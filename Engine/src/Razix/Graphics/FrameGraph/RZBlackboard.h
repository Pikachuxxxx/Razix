#pragma once

#include <any>
#include <typeindex>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            /**
             * Black Board is a global locker kinda thing to hold the resources of a particular pass
             */
            class RAZIX_API RZBlackboard
            {
            public:
                RZBlackboard()  = default;
                ~RZBlackboard() = default;

                template<typename T, typename... Args>
                T& add(Args&&... args);

                template<typename T>
                const T& get() const;
                template<typename T>
                const T* try_get() const;

                template<typename T>
                const T& get();
                template<typename T>
                const T* try_get();

                template<typename T>
                bool has() const;

            private:
                std::unordered_map<std::type_index, std::any> m_Storage;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
