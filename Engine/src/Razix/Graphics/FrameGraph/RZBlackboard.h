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

#include "RZBlackboard.inl"

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
