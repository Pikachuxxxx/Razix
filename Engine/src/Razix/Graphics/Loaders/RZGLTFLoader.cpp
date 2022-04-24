#include "rzxpch.h"

#include "Razix/Core/OS/RZFileSystem.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZTexture.h"

#include "Razix/Utilities/RZStringUtilities.h"

#define NOMINMAX

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef RAZIX_DISTRIBUTION
#define TINYGLTF_NOEXCEPTION
#endif

#include <tinygltf/tiny_gltf.h>

namespace Razix {
    namespace Graphics {

        static std::map<int32_t, size_t> ComponentSize{
    { TINYGLTF_COMPONENT_TYPE_BYTE, sizeof(int8_t) },
    { TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, sizeof(uint8_t) },
    { TINYGLTF_COMPONENT_TYPE_SHORT, sizeof(int16_t) },
    { TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, sizeof(uint16_t) },
    { TINYGLTF_COMPONENT_TYPE_INT, sizeof(int32_t) },
    { TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, sizeof(uint32_t) },
    { TINYGLTF_COMPONENT_TYPE_FLOAT, sizeof(float) },
    { TINYGLTF_COMPONENT_TYPE_DOUBLE, sizeof(double) }
        };

        static std::map<int, int> GLTF_COMPONENT_LENGTH_LOOKUP = {
            { TINYGLTF_TYPE_SCALAR, 1 },
            { TINYGLTF_TYPE_VEC2, 2 },
            { TINYGLTF_TYPE_VEC3, 3 },
            { TINYGLTF_TYPE_VEC4, 4 },
            { TINYGLTF_TYPE_MAT2, 4 },
            { TINYGLTF_TYPE_MAT3, 9 },
            { TINYGLTF_TYPE_MAT4, 16 }
        };

        static std::map<int, int> GLTF_COMPONENT_BYTE_SIZE_LOOKUP = {
            { TINYGLTF_COMPONENT_TYPE_BYTE, 1 },
            { TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, 1 },
            { TINYGLTF_COMPONENT_TYPE_SHORT, 2 },
            { TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, 2 },
            { TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, 4 },
            { TINYGLTF_COMPONENT_TYPE_FLOAT, 4 }
        };

        struct Vector4Simple
        {
            float x, y, z, w;
        };

        inline glm::vec4 ToVector(const Vector4Simple& vec)
        {
            return glm::vec4(vec.x, vec.y, vec.z, vec.w);
        }

        std::vector<Graphics::RZMesh*> LoadMesh(tinygltf::Model& model, tinygltf::Mesh& mesh)
        {
            std::vector<Graphics::RZMesh*> meshes;

            for (auto& primitive : mesh.primitives) {
                const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices];

                std::vector<uint16_t> indices;
                std::vector<Graphics::RZVertex> vertices;

                indices.resize(indicesAccessor.count);
                vertices.resize(indicesAccessor.count);

                for (auto& attribute : primitive.attributes) {
                    // Get accessor info
                    auto& accessor = model.accessors.at(attribute.second);
                    auto& bufferView = model.bufferViews.at(accessor.bufferView);
                    auto& buffer = model.buffers.at(bufferView.buffer);
                    int componentLength = GLTF_COMPONENT_LENGTH_LOOKUP.at(accessor.type);
                    int componentTypeByteSize = GLTF_COMPONENT_BYTE_SIZE_LOOKUP.at(accessor.componentType);

                    // Extra vertex data from buffer
                    size_t bufferOffset = bufferView.byteOffset + accessor.byteOffset;
                    int bufferLength = static_cast<int>(accessor.count) * componentLength * componentTypeByteSize;
                    auto first = buffer.data.begin() + bufferOffset;
                    auto last = buffer.data.begin() + bufferOffset + bufferLength;
                    std::vector<uint8_t> data = std::vector<uint8_t>(first, last);

                    // -------- Position attribute -----------

                    if (attribute.first == "POSITION") {
                        size_t positionCount = accessor.count;
                        glm::vec3* positions = reinterpret_cast<glm::vec3*>(data.data());
                        for (auto p = 0; p < positionCount; ++p) {
                            vertices[p].Position = positions[p];
                        }
                    }

                    // -------- Normal attribute -----------

                    else if (attribute.first == "NORMAL") {
                        size_t normalCount = accessor.count;
                        glm::vec3* normals = reinterpret_cast<glm::vec3*>(data.data());
                        for (auto p = 0; p < normalCount; ++p) {
                            vertices[p].Normal = normals[p];
                        }
                    }

                    // -------- Texcoord attribute -----------

                    else if (attribute.first == "TEXCOORD_0") {
                        size_t uvCount = accessor.count;
                        glm::vec2* uvs = reinterpret_cast<glm::vec2*>(data.data());
                        for (auto p = 0; p < uvCount; ++p) {
                            vertices[p].TexCoords = uvs[p];
                        }
                    }

                    // -------- Colour attribute -----------
                
                    else if (attribute.first == "COLOR_0") {
                        size_t uvCount = accessor.count;
                        Vector4Simple* colours = reinterpret_cast<Vector4Simple*>(data.data());
                        for (auto p = 0; p < uvCount; ++p) {
                            vertices[p].Color = ToVector(colours[p]);
                        }
                    }

                    // -------- Tangent attribute -----------

                    //else if (attribute.first == "TANGENT") {
                    //    size_t uvCount = accessor.count;
                    //    Maths::Vector3Simple* uvs = reinterpret_cast<Maths::Vector3Simple*>(data.data());
                    //    for (auto p = 0; p < uvCount; ++p) {
                    //        vertices[p].Tangent = parentTransform.GetWorldMatrix() * ToVector(uvs[p]);
                    //    }
                    //}
                }

                // -------- Indices ----------
                {
                    // Get accessor info
                    auto indexAccessor = model.accessors.at(primitive.indices);
                    auto indexBufferView = model.bufferViews.at(indexAccessor.bufferView);
                    auto indexBuffer = model.buffers.at(indexBufferView.buffer);

                    int componentLength = GLTF_COMPONENT_LENGTH_LOOKUP.at(indexAccessor.type);
                    int componentTypeByteSize = GLTF_COMPONENT_BYTE_SIZE_LOOKUP.at(indexAccessor.componentType);

                    // Extra index data
                    size_t bufferOffset = indexBufferView.byteOffset + indexAccessor.byteOffset;
                    int bufferLength = static_cast<int>(indexAccessor.count) * componentLength * componentTypeByteSize;
                    auto first = indexBuffer.data.begin() + bufferOffset;
                    auto last = indexBuffer.data.begin() + bufferOffset + bufferLength;
                    std::vector<uint8_t> data = std::vector<uint8_t>(first, last);

                    size_t indicesCount = indexAccessor.count;
                    if (componentTypeByteSize == 2) {
                        uint16_t* in = reinterpret_cast<uint16_t*>(data.data());
                        for (auto iCount = 0; iCount < indicesCount; iCount++) {
                            indices[iCount] = (uint32_t) in[iCount];
                        }
                    }
                    else if (componentTypeByteSize == 4) {
                        auto in = reinterpret_cast<uint32_t*>(data.data());
                        for (auto iCount = 0; iCount < indicesCount; iCount++) {
                            indices[iCount] = in[iCount];
                        }
                    }
                }

                auto lMesh = new Graphics::RZMesh(indices, vertices);
                meshes.emplace_back(lMesh);
            }

            return meshes;
        }

        void LoadNode(RZModel* mainModel, int nodeIndex, tinygltf::Model& model, std::vector<std::vector<Graphics::RZMesh*>>& meshes)
        {
            if (nodeIndex < 0)
                return;

            auto& node = model.nodes[nodeIndex];
            auto name = node.name;

            // Print the info about the model accrediting the author of the model (Make this persist in dist builds`)
            RAZIX_CORE_TRACE("asset.copyright   : {0}", model.asset.copyright);
            RAZIX_CORE_TRACE("asset.generator   : {0}", model.asset.generator);
            RAZIX_CORE_TRACE("asset.version     : {0}", model.asset.version);
            RAZIX_CORE_TRACE("asset.minVersion  : {0}", model.asset.minVersion);

            if (node.mesh >= 0) {
                int subIndex = 0;

                auto meshes = LoadMesh(model, model.meshes[node.mesh]);

                for (auto& mesh : meshes) {
                    auto subname = node.name;
                    mesh->setName(subname);
                    mainModel->addMesh(mesh);
                    subIndex++;
                }
            }

        }

        void RZModel::loadGLTF(const std::string& path)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            tinygltf::Model model;
            tinygltf::TinyGLTF loader;
            std::string err;
            std::string warn;

            std::string ext = Utilities::GetFilePathExtension(path);

            // Not needed just yet
            loader.SetImageLoader(tinygltf::LoadImageData, nullptr);
            loader.SetImageWriter(tinygltf::WriteImageData, nullptr);

            bool ret;

            if (ext == "glb") // assume binary glTF.
            {
                ret = tinygltf::TinyGLTF().LoadBinaryFromFile(&model, &err, &warn, path);
            }
            else // assume ascii glTF2.0 embedded
            {
                ret = tinygltf::TinyGLTF().LoadASCIIFromFile(&model, &err, &warn, path);
            }

            if (!err.empty()) {
                RAZIX_CORE_ERROR(err);
            }

            if (!warn.empty()) {
                RAZIX_CORE_ERROR(warn);
            }

            if (!ret) {
                RAZIX_CORE_ERROR("Failed to parse glTF");
            }

            std::string name = path.substr(path.find_last_of('/') + 1);
            auto meshes = std::vector<std::vector<Graphics::RZMesh*>>();
            const tinygltf::Scene& gltfScene = model.scenes[std::max(0, model.defaultScene)];
            for (size_t i = 0; i < gltfScene.nodes.size(); i++)
                LoadNode(this, gltfScene.nodes[i], model, meshes);
        }
    }
}