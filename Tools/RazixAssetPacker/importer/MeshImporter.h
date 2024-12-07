#pragma once

// Based on https://github.com/diharaw/asset-core

#include "common/intermediate_types.h"

struct aiMaterial;
struct aiScene;
struct aiNode;

namespace Razix {
    namespace Tool {
        namespace AssetPacker {

            struct MeshImportOptions
            {
                bool  flipUVs        = false;
                bool  encodeVertices = false;
                bool  encodeIndices  = false;
                float mergeDistance  = 0.05f;
            };

            class MeshImporter
            {
            public:
                MeshImporter()  = default;
                ~MeshImporter() = default;

                bool importMesh(const std::string& meshFilePath, MeshImportResult& result, MeshImportOptions options = MeshImportOptions());

                const Node* getRootNode() const { return rootNode; }

            private:
                void readMaterial(const std::string& materialsDirectory, aiMaterial* aiMat, Gfx::MaterialData& material);
                bool findTexurePath(const std::string& materialsDirectory, aiMaterial* aiMat, uint32_t textureType, uint32_t index, char* material);
                void printHierarchy(const aiNode* node, const aiScene* scene, uint32_t depthIndex);
                void extractHierarchy(Node* hierarchyNode, const aiNode* node, const aiScene* scene, uint32_t depthIndex);

            private:
                bool  m_IsGlTF = false;
                Node* rootNode = nullptr;
            };
        }    // namespace AssetPacker
    }        // namespace Tool
}    // namespace Razix