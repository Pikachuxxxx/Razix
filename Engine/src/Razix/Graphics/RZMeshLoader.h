#pragma once

#include "Razix/AssetSystem/RZAssetFileSpec.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;
        class RZMaterial;

        RZMesh*     loadMesh(const std::string& filePath);
        RZMaterial* loadMaterial(const std::string& materialName);
    }    // namespace Graphics
}    // namespace Razix
