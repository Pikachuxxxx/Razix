// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Core/OS/RZFileSystem.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

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

        std::string AlbedoTexName   = "baseColorTexture";
        std::string NormalTexName   = "normalTexture";
        std::string MetallicTexName = "metallicRoughnessTexture";
        std::string GlossTexName    = "metallicRoughnessTexture";
        std::string AOTexName       = "occlusionTexture";
        std::string EmissiveTexName = "emissiveTexture";

        struct GLTFTexture
        {
            tinygltf::Image*   Image;
            tinygltf::Sampler* Sampler;
        };

        static std::map<int32_t, sz> ComponentSize{
            {TINYGLTF_COMPONENT_TYPE_BYTE, sizeof(int8_t)},
            {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, sizeof(u8)},
            {TINYGLTF_COMPONENT_TYPE_SHORT, sizeof(int16_t)},
            {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, sizeof(u16)},
            {TINYGLTF_COMPONENT_TYPE_INT, sizeof(int32_t)},
            {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, sizeof(u32)},
            {TINYGLTF_COMPONENT_TYPE_FLOAT, sizeof(f32)},
            {TINYGLTF_COMPONENT_TYPE_DOUBLE, sizeof(d32)}};

        static std::map<int, int> GLTF_COMPONENT_LENGTH_LOOKUP = {
            {TINYGLTF_TYPE_SCALAR, 1},
            {TINYGLTF_TYPE_VEC2, 2},
            {TINYGLTF_TYPE_VEC3, 3},
            {TINYGLTF_TYPE_VEC4, 4},
            {TINYGLTF_TYPE_MAT2, 4},
            {TINYGLTF_TYPE_MAT3, 9},
            {TINYGLTF_TYPE_MAT4, 16}};

        static std::map<int, int> GLTF_COMPONENT_BYTE_SIZE_LOOKUP = {
            {TINYGLTF_COMPONENT_TYPE_BYTE, 1},
            {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, 1},
            {TINYGLTF_COMPONENT_TYPE_SHORT, 2},
            {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, 2},
            {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, 4},
            {TINYGLTF_COMPONENT_TYPE_FLOAT, 4}};

        struct Vector4Simple
        {
            f32 x, y, z, w;
        };

        inline glm::vec4 ToVector(const Vector4Simple& vec)
        {
            return glm::vec4(vec.x, vec.y, vec.z, vec.w);
        }

        std::vector<RZMaterial*> LoadMaterials(tinygltf::Model& gltfModel)
        {
            std::vector<Graphics::RZTexture2D*> loadedTextures;
            std::vector<RZMaterial*>            loadedMaterials;
            loadedTextures.reserve(gltfModel.textures.size());
            loadedMaterials.reserve(gltfModel.materials.size());

            for (tinygltf::Texture& gltfTexture: gltfModel.textures) {
                GLTFTexture imageAndSampler{};

                if (gltfTexture.source != -1) {
                    imageAndSampler.Image = &gltfModel.images.at(gltfTexture.source);
                }

                if (gltfTexture.sampler != -1) {
                    imageAndSampler.Sampler = &gltfModel.samplers.at(gltfTexture.sampler);
                }

                if (imageAndSampler.Image) {
                    Graphics::RZTexture2D* texture2D = Graphics::RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG(gltfTexture.name) imageAndSampler.Image->name, imageAndSampler.Image->width, imageAndSampler.Image->height, imageAndSampler.Image->image.data(), Graphics::RZTexture::Format::RGBA8);
                    loadedTextures.push_back(texture2D ? texture2D : nullptr);
                }
            }

            auto TextureName = [&](int index) {
                if (index >= 0) {
                    const tinygltf::Texture& tex = gltfModel.textures[index];
                    if (tex.source >= 0 && tex.source < loadedTextures.size()) {
                        return loadedTextures[tex.source];
                    }
                } else
                    return RZMaterial::GetDefaultTexture();
            };

            for (tinygltf::Material& mat: gltfModel.materials) {

                // TODO: Fix this use a more robust default shader
                auto shader = Graphics::RZShaderLibrary::Get().getShader("forward_renderer.rzsf");

                RZMaterial*                  forwardRendererMaterial = new RZMaterial(shader);
                MaterialTextures             textures;
                Graphics::MaterialProperties properties;

                const tinygltf::PbrMetallicRoughness& pbr = mat.pbrMetallicRoughness;
                textures.albedo                           = TextureName(pbr.baseColorTexture.index);
                textures.normal                           = TextureName(mat.normalTexture.index);
                textures.metallic                         = TextureName(pbr.metallicRoughnessTexture.index);
                textures.roughness                        = TextureName(pbr.metallicRoughnessTexture.index);
                textures.emissive                         = TextureName(mat.emissiveTexture.index);
                textures.ao                               = TextureName(mat.occlusionTexture.index);
                //properties.workflow                       = WorkFlow::PBR_WORKFLOW_METALLIC_ROUGHTNESS;

                // metallic-roughness workflow:
                auto baseColorFactor = mat.values.find("baseColorFactor");
                auto roughnessFactor = mat.values.find("roughnessFactor");
                auto metallicFactor  = mat.values.find("metallicFactor");
                auto emissiveFactor  = mat.values.find("emissiveFactor");

                if (roughnessFactor != mat.values.end()) {
                    properties.roughnessColor = glm::vec4(static_cast<f32>(roughnessFactor->second.Factor())).a;
                }

                if (metallicFactor != mat.values.end()) {
                    properties.metallicColor = glm::vec4(static_cast<f32>(metallicFactor->second.Factor())).r;
                }

                if (baseColorFactor != mat.values.end()) {
                    properties.albedoColor = glm::vec3((f32) baseColorFactor->second.ColorFactor()[0], (f32) baseColorFactor->second.ColorFactor()[1], (f32) baseColorFactor->second.ColorFactor()[2]);

                    properties.opacity = (f32) baseColorFactor->second.ColorFactor()[3];
                }

                // Extensions
                auto metallicGlossinessWorkflow = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
                if (metallicGlossinessWorkflow != mat.extensions.end()) {
                    if (metallicGlossinessWorkflow->second.Has("diffuseTexture")) {
                        int index       = metallicGlossinessWorkflow->second.Get("diffuseTexture").Get("index").Get<int>();
                        textures.albedo = loadedTextures[gltfModel.textures[index].source];
                        //properties.isUsingAlbedoMap = true;
                    }

                    if (metallicGlossinessWorkflow->second.Has("metallicGlossinessTexture")) {
                        int index          = metallicGlossinessWorkflow->second.Get("metallicGlossinessTexture").Get("index").Get<int>();
                        textures.roughness = loadedTextures[gltfModel.textures[index].source];
                        //properties.isUsingMetallicMap = true;
                    }

                    if (metallicGlossinessWorkflow->second.Has("diffuseFactor")) {
                        auto& factor             = metallicGlossinessWorkflow->second.Get("diffuseFactor");
                        properties.albedoColor.x = factor.ArrayLen() > 0 ? f32(factor.Get(0).IsNumber() ? factor.Get(0).Get<d32>() : factor.Get(0).Get<int>()) : 1.0f;
                        properties.albedoColor.y = factor.ArrayLen() > 1 ? f32(factor.Get(1).IsNumber() ? factor.Get(1).Get<d32>() : factor.Get(1).Get<int>()) : 1.0f;
                        properties.albedoColor.z = factor.ArrayLen() > 2 ? f32(factor.Get(2).IsNumber() ? factor.Get(2).Get<d32>() : factor.Get(2).Get<int>()) : 1.0f;
                        properties.opacity       = factor.ArrayLen() > 3 ? f32(factor.Get(3).IsNumber() ? factor.Get(3).Get<d32>() : factor.Get(3).Get<int>()) : 1.0f;
                    }
                    if (metallicGlossinessWorkflow->second.Has("metallicFactor")) {
                        auto& factor               = metallicGlossinessWorkflow->second.Get("metallicFactor");
                        properties.metallicColor = factor.ArrayLen() > 0 ? f32(factor.Get(0).IsNumber() ? factor.Get(0).Get<d32>() : factor.Get(0).Get<int>()) : 1.0f;
                        //properties.metallicColor.y = factor.ArrayLen() > 0 ? f32(factor.Get(1).IsNumber() ? factor.Get(1).Get<d32>() : factor.Get(1).Get<int>()) : 1.0f;
                        //properties.metallicColor.z = factor.ArrayLen() > 0 ? f32(factor.Get(2).IsNumber() ? factor.Get(2).Get<d32>() : factor.Get(2).Get<int>()) : 1.0f;
                        //properties.metallicColor.w = factor.ArrayLen() > 0 ? f32(factor.Get(3).IsNumber() ? factor.Get(3).Get<d32>() : factor.Get(3).Get<int>()) : 1.0f;
                    }
                    if (metallicGlossinessWorkflow->second.Has("glossinessFactor")) {
                        auto& factor              = metallicGlossinessWorkflow->second.Get("glossinessFactor");
                        properties.roughnessColor = 1.0f - f32(factor.IsNumber() ? factor.Get<d32>() : factor.Get<int>());
                    }
                }

                forwardRendererMaterial->setTextures(textures);
                forwardRendererMaterial->setProperties(properties);
                forwardRendererMaterial->createDescriptorSet();

                loadedMaterials.push_back(forwardRendererMaterial);
            }

            return loadedMaterials;
        }

        std::vector<Graphics::RZMesh*> LoadMesh(tinygltf::Model& model, tinygltf::Mesh& mesh)
        {
            std::vector<Graphics::RZMesh*> meshes;

            for (auto& primitive: mesh.primitives) {
                const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices];

                std::vector<u32>           indices;
                std::vector<Graphics::RZVertex> vertices;

                indices.resize(indicesAccessor.count);
                vertices.resize(indicesAccessor.count);

                for (auto& attribute: primitive.attributes) {
                    // Get accessor info
                    auto& accessor              = model.accessors.at(attribute.second);
                    auto& bufferView            = model.bufferViews.at(accessor.bufferView);
                    auto& buffer                = model.buffers.at(bufferView.buffer);
                    int   componentLength       = GLTF_COMPONENT_LENGTH_LOOKUP.at(accessor.type);
                    int   componentTypeByteSize = GLTF_COMPONENT_BYTE_SIZE_LOOKUP.at(accessor.componentType);

                    // Extra vertex data from buffer
                    sz               bufferOffset = bufferView.byteOffset + accessor.byteOffset;
                    int                  bufferLength = static_cast<int>(accessor.count) * componentLength * componentTypeByteSize;
                    auto                 first        = buffer.data.begin() + bufferOffset;
                    auto                 last         = buffer.data.begin() + bufferOffset + bufferLength;
                    std::vector<u8> data         = std::vector<u8>(first, last);

                    // -------- Position attribute -----------

                    if (attribute.first == "POSITION") {
                        sz     positionCount = accessor.count;
                        glm::vec3* positions     = reinterpret_cast<glm::vec3*>(data.data());
                        for (auto p = 0; p < positionCount; ++p) {
                            vertices[p].Position = positions[p];
                        }
                    }

                    // -------- Normal attribute -----------

                    else if (attribute.first == "NORMAL") {
                        sz     normalCount = accessor.count;
                        glm::vec3* normals     = reinterpret_cast<glm::vec3*>(data.data());
                        for (auto p = 0; p < normalCount; ++p) {
                            vertices[p].Normal = normals[p];
                        }
                    }

                    // -------- Texcoord attribute -----------

                    else if (attribute.first == "TEXCOORD_0") {
                        sz     uvCount = accessor.count;
                        glm::vec2* uvs     = reinterpret_cast<glm::vec2*>(data.data());
                        for (auto p = 0; p < uvCount; ++p) {
                            vertices[p].TexCoords = uvs[p];
                        }
                    }

                    // -------- Color attribute -----------

                    else if (attribute.first == "COLOR_0") {
                        sz         uvCount = accessor.count;
                        Vector4Simple* Colors  = reinterpret_cast<Vector4Simple*>(data.data());
                        for (auto p = 0; p < uvCount; ++p) {
                            vertices[p].Color = ToVector(Colors[p]);
                        }
                    }

                    // -------- Tangent attribute -----------

                    //else if (attribute.first == "TANGENT") {
                    //    sz uvCount = accessor.count;
                    //    Maths::Vector3Simple* uvs = reinterpret_cast<Maths::Vector3Simple*>(data.data());
                    //    for (auto p = 0; p < uvCount; ++p) {
                    //        vertices[p].Tangent = parentTransform.GetWorldMatrix() * ToVector(uvs[p]);
                    //    }
                    //}
                }

                // -------- Indices ----------
                {
                    // Get accessor info
                    auto indexAccessor   = model.accessors.at(primitive.indices);
                    auto indexBufferView = model.bufferViews.at(indexAccessor.bufferView);
                    auto indexBuffer     = model.buffers.at(indexBufferView.buffer);

                    int componentLength       = GLTF_COMPONENT_LENGTH_LOOKUP.at(indexAccessor.type);
                    int componentTypeByteSize = GLTF_COMPONENT_BYTE_SIZE_LOOKUP.at(indexAccessor.componentType);

                    // Extra index data
                    sz               bufferOffset = indexBufferView.byteOffset + indexAccessor.byteOffset;
                    int                  bufferLength = static_cast<int>(indexAccessor.count) * componentLength * componentTypeByteSize;
                    auto                 first        = indexBuffer.data.begin() + bufferOffset;
                    auto                 last         = indexBuffer.data.begin() + bufferOffset + bufferLength;
                    std::vector<u32> data         = std::vector<u32>(first, last);

                    sz indicesCount = indexAccessor.count;
                    if (componentTypeByteSize == 2) {
                        u32* in = reinterpret_cast<u32*>(data.data());
                        for (auto iCount = 0; iCount < indicesCount; iCount++) {
                            indices[iCount] = (u32) in[iCount];
                        }
                    } else if (componentTypeByteSize == 4) {
                        auto in = reinterpret_cast<u32*>(data.data());
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

        void LoadNode(RZModel* mainModel, int nodeIndex, tinygltf::Model& model, std::vector<RZMaterial*> materials, std::vector<std::vector<Graphics::RZMesh*>>& meshes)
        {
            if (nodeIndex < 0)
                return;

            auto& node = model.nodes[nodeIndex];
            auto  name = node.name;

            // Print the info about the model accrediting the author of the model (Make this persist in dist builds`)
            RAZIX_CORE_TRACE("asset.copyright   : {0}", model.asset.copyright);
            RAZIX_CORE_TRACE("asset.generator   : {0}", model.asset.generator);
            RAZIX_CORE_TRACE("asset.version     : {0}", model.asset.version);
            RAZIX_CORE_TRACE("asset.minVersion  : {0}", model.asset.minVersion);

            if (node.mesh >= 0) {
                int subIndex = 0;

                auto meshes = LoadMesh(model, model.meshes[node.mesh]);

                for (auto& mesh: meshes) {
                    auto subname = node.name;
                    mesh->setName(subname);

                    int materialIndex = model.meshes[node.mesh].primitives[subIndex].material;
                    if (materialIndex >= 0) {
                        mesh->setMaterial(materials[materialIndex]);
                    }

                    mainModel->addMesh(mesh);
                    subIndex++;
                }
            }

            // TODO: Laod child meshes
        }

        void RZModel::loadGLTF(const std::string& path)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            tinygltf::Model    model;
            tinygltf::TinyGLTF loader;
            std::string        err;
            std::string        warn;

            std::string ext = Utilities::GetFilePathExtension(path);

            // Not needed just yet
            loader.SetImageLoader(tinygltf::LoadImageData, nullptr);
            loader.SetImageWriter(tinygltf::WriteImageData, nullptr);

            bool ret;

            if (ext == "glb")    // assume binary glTF.
            {
                ret = tinygltf::TinyGLTF().LoadBinaryFromFile(&model, &err, &warn, path);
            } else    // assume ascii glTF2.0 embedded
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

            auto LoadedMaterials = LoadMaterials(model);

            std::string            name      = path.substr(path.find_last_of('/') + 1);
            auto                   meshes    = std::vector<std::vector<Graphics::RZMesh*>>();
            const tinygltf::Scene& gltfScene = model.scenes[std::max(0, model.defaultScene)];
            for (sz i = 0; i < gltfScene.nodes.size(); i++)
                LoadNode(this, gltfScene.nodes[i], model, LoadedMaterials, meshes);
        }
    }    // namespace Graphics
}    // namespace Razix