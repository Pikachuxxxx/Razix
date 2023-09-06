#pragma once

#include <any>
#include <typeindex>

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
             * Black Board is a global locker kinda thing to hold the resources of a particular pass
             */
            class RAZIX_API RZBlackboard
            {
            public:
                RZBlackboard()  = default;
                ~RZBlackboard() = default;

                template<typename T, typename... Args>
                T& add(Args&&... args)
                {
                    assert(!has<T>());
                    return m_storage[typeid(T)].emplace<T>(T{std::forward<Args>(args)...});
                }

                template<typename T>
                RAZIX_NO_DISCARD const T& get() const
                {
                    assert(has<T>());
                    return std::any_cast<const T&>(m_storage.at(typeid(T)));
                }
                template<typename T>
                RAZIX_NO_DISCARD const T* try_get() const
                {
                    auto it = m_storage.find(typeid(T));
                    return it != m_storage.cend() ? std::any_cast<const T>(&it->second) : nullptr;
                }

                template<typename T>
                T& get()
                {
                    return const_cast<T&>(const_cast<const RZBlackboard*>(this)->get<T>());
                }
                template<typename T>
                T* try_get()
                {
                    return const_cast<T*>(const_cast<const RZBlackboard*>(this)->try_get<T>());
                }

                template<typename T>
                bool has() const
                {
                    return m_storage.find(typeid(T)) != m_storage.cend();
                }

            private:
                std::unordered_map<std::type_index, std::any> m_Storage;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
