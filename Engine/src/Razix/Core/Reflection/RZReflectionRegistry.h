#pragma once

#include <type_traits>
#include <typeindex>
#include <unordered_map>

#include "Razix/Core/RZCore.h"

#include "Razix/Core/Reflection/RZReflectionMetaData.h"

namespace Razix {

    /**
	 * By default they are viewed as default type, 
	 * these enums can be used to customize how reflection systems views them
	 * for ex. a string can be just read-only or read-write or loaded by a file browser by UI
	 * This helps with generating additional metadata into .DDL files and helps with UI generation
	 */
    enum class MetaDataViewHint
    {
        kDefault,
        kReadOnly,
        kReadWrite,
        kRange,    // basically a slider
        kFile,
        kVector2d,
        kVector3d,
        kVector4d,
        kToggle,
        kIntPrecision,
        kFloatPrecision,
        // ADD MORE HERE
        COUNT
    };
    static_assert((int)MetaDataViewHint::COUNT == 11);

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
            return nullptr;
        }

    private:
        static std::unordered_map<std::type_index, TypeMetaData>& getRegistry()
        {
            static std::unordered_map<std::type_index, TypeMetaData> m_Registry;
            return m_Registry;
        }
    };
}    // namespace Razix