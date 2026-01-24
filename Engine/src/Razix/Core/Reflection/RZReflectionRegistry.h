#pragma once

#include <type_traits>
#include <typeindex>
#include <unordered_map>

#include "Razix/Core/RZCore.h"

#include "Razix/Core/Reflection/RZReflectionMetaData.h"

namespace Razix {

    class RZTypeRegistry
    {
    public:
        template<typename T>
        static void registerType(const TypeMetaData& typeData)
        {
            getRegistry()[typeid(T)] = typeData;
        }

        template<typename T>
        static const TypeMetaData* getTypeMetaData()
        {
            auto it = getRegistry().find(typeid(T));
            if (it != getRegistry().end()) {
                return &it->second;
            }
            return NULL;
        }

        static bool isTypeTriviallySerializable(std::type_index typeIdx)
        {
            auto it = getRegistry().find(typeIdx);
            if (it != getRegistry().end()) {
                return it->second.bIsTriviallySerializable;
            }
            return false;
        }

    private:
        static RZHashMap<std::type_index, TypeMetaData>& getRegistry()
        {
            static RZHashMap<std::type_index, TypeMetaData> m_Registry;
            return m_Registry;
        }
    };
}    // namespace Razix
