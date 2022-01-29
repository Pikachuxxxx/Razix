#include "rzxpch.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZTexture.h"


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef LUMOS_DIST
#define TINYGLTF_NOEXCEPTION
#endif
#include <tinygltf/tiny_gltf.h>

namespace Razix {
    namespace Graphics {

        void RZModel::loadGLTF(const std::string& path)
        {

        }
    }
}