#ifndef _RZ_COMPRESSED_SERIALIZER_H_
#define _RZ_COMPRESSED_SERIALIZER_H_

#include "Razix/Core/Compression/RZCompression.h"

#include "Razix/Core/Serialization/RZSerializable.h"

namespace Razix {

    template<typename T>
    class RZCompressedSerializer
    {
    public:
        static void Serialize(typename RZSerializable<T>::RZBinaryArchive& ar, const T& data)
        {
            Razix::RZDynamicArray<u8> uncompressedData = RZSerializable<T>::serializeToBinary(data);

            // walk through the type medata and compress blob members
            const TypeMetaData* meta = RZSerializable<T>::getTypeMetaData();
            if (!meta) {
                RAZIX_CORE_ERROR("[RZCompressedSerializer] Type metadata for type '{}' not found.",
                    typeid(T).name());
                return;
            }
        }
    }
}    // namespace Razix
#endif    // _RZ_COMPRESSED_SERIALIZER_H_
