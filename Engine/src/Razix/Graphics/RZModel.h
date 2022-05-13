#pragma once

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

namespace Razix {
    namespace Graphics {

        /**
         * Model is a 3D asset loaded externally that is rendered in the game world
         * 
         * Note: Only loads OBJ, GLTF and FBX files
         * Collada and other format support will be added later
         */
        class RAZIX_API RZModel
        {
        public:
            RZModel() = default;
            RZModel(const std::string& filePath);

            ~RZModel() = default;

            void Destroy();

            template<typename Archive>
            void load(Archive& archive)
            {
                archive(cereal::make_nvp("FilePath", m_FilePath));
                m_Meshes.clear();
                loadModel(m_FilePath);
            }

            template<typename Archive>
            void save(Archive& archive) const
            {
                if (m_Meshes.size() > 0) {
                    std::string filePath;
                    RZVirtualFileSystem::Get().resolvePhysicalPath(m_FilePath, filePath);
                    archive(cereal::make_nvp("FilePath", filePath));
                }
            }

            RAZIX_INLINE void addMesh(RZMesh* mesh) { m_Meshes.push_back(mesh); }

            RAZIX_INLINE const std::vector<RZMesh*>& getMeshes() const { return m_Meshes; }
            RAZIX_INLINE const std::string& getFilePath() const { return m_FilePath; }

        private:
            std::string          m_FilePath;
            std::vector<RZMesh*> m_Meshes;

        private:
            void loadModel(const std::string& path);
            void loadOBJ(const std::string& path);
            void loadGLTF(const std::string& path);
            void loadFBX(const std::string& path);
        };

    }    // namespace Graphics
}    // namespace Razix