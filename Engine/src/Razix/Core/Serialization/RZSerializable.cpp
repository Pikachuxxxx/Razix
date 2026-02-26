// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZSerializable.h"

#include "Razix/AssetSystem/RZAssetBase.h"

namespace Razix {

    RZDiskTypeTag SerializableTypeToDiskTag(SerializeableDataType t)
    {
        switch (t) {
            default:
            case SerializeableDataType::kPrimitive: return RZDiskTypeTag::kPrimitive;
            case SerializeableDataType::kBlob: return RZDiskTypeTag::kBlob;
            case SerializeableDataType::kArray: return RZDiskTypeTag::kArray;
            case SerializeableDataType::kHashMap: return RZDiskTypeTag::kHashMap;
            case SerializeableDataType::kString: return RZDiskTypeTag::kString;
            case SerializeableDataType::kObject: return RZDiskTypeTag::kObject;
            case SerializeableDataType::kObjectArray: return RZDiskTypeTag::kObjectArray;
            case SerializeableDataType::kEnum: return RZDiskTypeTag::kEnum;
            case SerializeableDataType::kBitField: return RZDiskTypeTag::kBitField;
            case SerializeableDataType::kUUID: return RZDiskTypeTag::kUUID;
        }
        return RZDiskTypeTag::kPrimitive;
    }

    void* RZAssetUtilCreateAssetInstanceInPlace(void* memoryBacking, void* coldDataBacking)
    {
        return reinterpret_cast<void*>(new (memoryBacking) Razix::RZAsset(Razix::RZAssetType::kTransform, coldDataBacking));
    }

}    // namespace Razix
