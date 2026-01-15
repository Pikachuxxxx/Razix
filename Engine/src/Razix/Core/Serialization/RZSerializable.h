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
            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) return buffer;

            const u8* base = reinterpret_cast<const u8*>(&m_Data);

            for (const auto& member: meta->members) {
                size_t oldSize = buffer.size();
                buffer.resize(oldSize + member.size);

                memcpy(buffer.data() + oldSize,
                    base + member.offset,
                    member.size);
            }

            return buffer;
        }

        void deserializeFromBinary(const RZDynamicArray<u8>& binary)
        {
            const TypeMetaData* meta = getTypeMetaData();
            if (!meta) return;

            u8*    base   = reinterpret_cast<u8*>(&m_Data);
            size_t offset = 0;

            for (const auto& member: meta->members) {
                if (offset + member.size > binary.size()) break;
                memcpy(base + member.offset, binary.data() + offset, member.size);
                offset += member.size;
            }
        }

        const Derived& getData() const
        {
            return m_Data;
        }

    private:
        Derived m_Data;
    };
}    // namespace Razix
#endif    // SERIALZABLE_H