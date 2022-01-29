#include "rzxpch.h"
#include "RZModel.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    namespace Graphics {

        RZModel::RZModel(const std::string& filePath)
            : m_FilePath(filePath)
        {
             loadModel(m_FilePath);
        }

        void RZModel::loadModel(const std::string& path)
        {
            std::string physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(path, physicalPath)) {
                RAZIX_CORE_ERROR("Failed to load Model - {0}", path);
                return;
            }
            std::string resolvedPath = physicalPath;

            const std::string fileExtension = Utilities::GetFilePathExtension(path);

            if (fileExtension == "obj")
                loadOBJ(resolvedPath);
            else if (fileExtension == "gltf" || fileExtension == "glb")
                loadGLTF(resolvedPath);
            else if (fileExtension == "fbx" || fileExtension == "FBX")
                loadFBX(resolvedPath);
            else
                RAZIX_CORE_ERROR("Unsupported Model Format : {0}", fileExtension);

            RAZIX_CORE_INFO("Loaded Model - {0}", path);
        }
    }
}