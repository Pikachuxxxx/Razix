#pragma once

#include "Razix/AssetSystem/RZAssetFileSpec.h"

namespace Razix {
    namespace Gfx {

        class RZMesh;
        class RZMaterial;

        RZMesh*     loadMesh(const std::string& filePath);
        RZMaterial* loadMaterial(const std::string& materialName, const std::string& folderNam);
    }    // namespace Graphics
}    // namespace Razix
