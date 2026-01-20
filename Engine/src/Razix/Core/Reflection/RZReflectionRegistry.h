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
            RAZIX_CORE_ERROR("[RZTypeRegistry] Type metadata for type '{}' not found in registry.", typeid(T).name());
            return NULL;
        }

    private:
        static RZHashMap<std::type_index, TypeMetaData>& getRegistry()
        {
            static RZHashMap<std::type_index, TypeMetaData> m_Registry;
            return m_Registry;
        }
    };
}    // namespace Razix
