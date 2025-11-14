#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "Razix/Core/std/type_traits.h"

namespace Razix {

    template<typename Derived>
    class RZSerializable
    {
    public:
        virtual ~RZSerializable() = default;

        const TypeMetaData* getTypeMetaData() const
        {
            return Razix::RZTypeRegistry::getTypeMetaData<rz_remove_cv_t<rz_remove_pointer_t<Derived>>>();
        }

        RZDynamicArray<u8> serializeToBinary() const
        {
            RZDynamicArray<u8>  buffer;
            const TypeMetaData* metaData = getTypeMetaData();
            if (!metaData) return buffer;

            for (const auto& member: metaData->members) {
                const u8* dataPtr = reinterpret_cast<const u8*>(this) + member.offset;
                buffer.insert(buffer.end(), dataPtr, dataPtr + member.size);
            }
            return buffer;
        }

        void deserializeFromBinary(const RZDynamicArray<u8>& binaryData)
        {
            const TypeMetaData* metaData = getTypeMetaData();
            if (!metaData) return;

            size_t offset = 0;
            for (const auto& member: metaData->members) {
                if (offset + member.size > binaryData.size()) break;
                u8* dataPtr = reinterpret_cast<u8*>(this) + member.offset;
                memcpy(dataPtr, binaryData.data() + offset, member.size);
                offset += member.size;
            }
        }
    };
}    // namespace Razix
#endif    // SERIALZABLE_H