#pragma once

#include <any>
#include <typeindex>

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

/**
    * FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
    * Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
    * With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
    * Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
    */

namespace Razix {
    namespace Gfx {

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
                return m_Storage[typeid(T)].emplace<T>(T{std::forward<Args>(args)...});
            }

            void add(RZString resStrID, RZFrameGraphResource ID)
            {
                // FIXME: Support this assert(!has<T>()); here
                //m_DataDrivenStorage[passStrID].push_back({resStrID, ID});
                m_DataDrivenStorage[resStrID] = ID;
            }

            template<typename T>
            RAZIX_NO_DISCARD const T& get() const
            {
                assert(has<T>());
                return std::any_cast<const T&>(m_Storage.at(typeid(T)));
            }

            RZFrameGraphResource getID(RZString resStrID)
            {
                //auto& passResources = m_DataDrivenStorage[passStrID];
                //for (auto& res: passResources) {
                //    if (res.first == resStrID)
                //        return res.second;
                //}
                //return RZFrameGraphResource{-1};

                RAZIX_ASSERT((m_DataDrivenStorage.find(resStrID) != m_DataDrivenStorage.end()), "[Frame Graph Blackboard] Cannot find resource! please check and the resource previously exists!");
                return m_DataDrivenStorage[resStrID];
            }

            template<typename T>
            RAZIX_NO_DISCARD const T* try_get() const
            {
                const auto it = m_Storage.cfind(typeid(T));
                return it != m_Storage.end() ? std::any_cast<const T>(&it->second) : NULL;
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
                return m_Storage.find(typeid(T)) != m_Storage.end();
            }

            RAZIX_INLINE const RZString& getFinalOutputName() const { return m_FinalOutputName; }
            RAZIX_INLINE void            setFinalOutputName(const RZString& val) { m_FinalOutputName = val; }
            RZFrameGraphResource         getFinalOutputID() { return getID(m_FinalOutputName); }

            void destroy()
            {
                m_Storage.clear();
                m_DataDrivenStorage.clear();
            }

        private:
            RZHashMap<std::type_index, std::any> m_Storage;
            //RZHashMap<RZString, RZDynamicArray<std::pair<RZString, RZFrameGraphResource>>> m_DataDrivenStorage;
            RZHashMap<RZString, RZFrameGraphResource> m_DataDrivenStorage;
            RZString                                  m_FinalOutputName = "SceneHDR";
        };
    }    // namespace Gfx
}    // namespace Razix
