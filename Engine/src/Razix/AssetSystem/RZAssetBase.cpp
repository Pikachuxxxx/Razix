// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZAssetBase.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include <Core/RZCore.h>

namespace Razix {

    RZString AssetTypeToVFSFilePath(RZAssetType type, const RZString& name)
    {
        // Map asset type to subdirectory
        const char* subdir = "";
        switch (type) {
            case RZAssetType::kAnimation: subdir = "Animations/"; break;
            case RZAssetType::kAudio: subdir = "Audio/"; break;
            case RZAssetType::kCamera: subdir = "Cameras/"; break;
            case RZAssetType::kCloth: subdir = "Clothes/"; break;
            case RZAssetType::kGameData: subdir = "GameData/"; break;
            case RZAssetType::kLight: subdir = "Lights/"; break;
            case RZAssetType::kLuaScript: subdir = "LuaScripts/"; break;
            case RZAssetType::kMaterial: subdir = "Materials/"; break;
            case RZAssetType::kMesh: subdir = "Meshes/"; break;
            case RZAssetType::kPhysicsMaterial: subdir = "PhysicsMaterials/"; break;
            case RZAssetType::kTexture: subdir = "Textures/"; break;
            case RZAssetType::kTransform: subdir = "Transforms/"; break;
            case RZAssetType::kVignerePuzzle: subdir = "Puzzles/"; break;
            default:
                RAZIX_CORE_ERROR("[AssetSystem] Unknown asset type: {}", static_cast<u32>(type));
                break;
        }

        return "//Assets/" + RZString(subdir) + name + ".rzasset";
    }

    RZAsset::RZAsset(RZAssetType type, void* pColdDataMemory)
    {
        RAZIX_CORE_ASSERT(pColdDataMemory != NULL, "[Asset] pColdDataMemory is NULL, creating asset with provided memory.");

        memset(&m_Hot, 0, sizeof(m_Hot));
        m_Hot.UUID = rz_uuid_generate();
        m_Hot.type = type;

        m_pCold = (RZAssetColdData*) pColdDataMemory;

        memset(m_pCold, 0, sizeof(RZAssetColdData));
        m_pCold->CS = rz_critical_section_create();
    }

    void RZAsset::destroy()
    {
        if (m_pCold) {
            rz_critical_section_destroy(&m_pCold->CS);
            // Don't free cold data here as its managed by the asset pool
            // You do not own the memory! and we don't want double frees.
            // The freelist in the asset pool will take care of freeing the cold data memory
            //rz_free(m_pCold);
            //m_pCold = NULL;
        }
    }
}    // namespace Razix
