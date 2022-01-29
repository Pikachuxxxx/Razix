#include "rzxpch.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZTexture.h"

#include <OpenFBX/ofbx.h>

namespace Razix {
    namespace Graphics {

        const uint32_t MAX_PATH_LENGTH = 260;

        std::string m_FBXModelDirectory;

        enum class Orientation
        {
            Y_UP,
            Z_UP,
            Z_MINUS_UP,
            X_MINUS_UP,
            X_UP
        };

        Orientation orientation = Orientation::Y_UP;
        float fbx_scale = 1.f;

        static ofbx::Vec3 operator-(const ofbx::Vec3& a, const ofbx::Vec3& b)
        {
            return { a.x - b.x, a.y - b.y, a.z - b.z };
        }

        void RZModel::loadFBX(const std::string& path)
        {

        }
    }
}