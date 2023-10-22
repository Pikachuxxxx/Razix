#pragma once

#include <any>
#include <typeindex>

#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"

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
                    return m_Storage[typeid(T)].emplace<T>(T{std::forward<Args>(args)...});
                }

                void add(std::string resStrID, RZFrameGraphResource ID)
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

                RZFrameGraphResource getID(std::string resStrID)
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
                    auto it = m_Storage.find(typeid(T));
                    return it != m_Storage.cend() ? std::any_cast<const T>(&it->second) : nullptr;
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
                    return m_Storage.find(typeid(T)) != m_Storage.cend();
                }

                RAZIX_INLINE const std::string& getFinalOutputName() const { return m_FinalOutputName; }
                RAZIX_INLINE void               setFinalOutputName(const std::string& val) { m_FinalOutputName = val; }
                RZFrameGraphResource            getFinalOutputID() { return getID(m_FinalOutputName); }

                void destroy()
                {
                    m_Storage.clear();
                    m_DataDrivenStorage.clear();
                }

            private:
                std::unordered_map<std::type_index, std::any> m_Storage;
                //std::unordered_map<std::string, std::vector<std::pair<std::string, RZFrameGraphResource>>> m_DataDrivenStorage;
                std::unordered_map<std::string, RZFrameGraphResource> m_DataDrivenStorage;
                std::string                                           m_FinalOutputName = "SceneHDR";
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
