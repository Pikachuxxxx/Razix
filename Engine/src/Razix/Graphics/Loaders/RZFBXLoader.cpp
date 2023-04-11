// clang-format off
#include "rzxpch.h"
// clang-format on

#include "Razix/Core/OS/RZFileSystem.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h" 
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"
#include "Razix/Graphics/Materials/RZMaterial.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <OpenFBX/ofbx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Razix {
    namespace Graphics {

        const u32 MAX_PATH_LENGTH = 260;

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
        f32       fbx_scale   = 1.0f;

        static ofbx::Vec3 operator-(const ofbx::Vec3& a, const ofbx::Vec3& b)
        {
            return {a.x - b.x, a.y - b.y, a.z - b.z};
        }

        static ofbx::Vec2 operator-(const ofbx::Vec2& a, const ofbx::Vec2& b)
        {
            return {a.x - b.x, a.y - b.y};
        }

        glm::vec3 FixOrientation(const glm::vec3& v)
        {
            switch (orientation) {
                case Orientation::Y_UP:
                    return glm::vec3(v.x, v.y, v.z);
                case Orientation::Z_UP:
                    return glm::vec3(v.x, v.z, -v.y);
                case Orientation::Z_MINUS_UP:
                    return glm::vec3(v.x, -v.z, v.y);
                case Orientation::X_MINUS_UP:
                    return glm::vec3(v.y, -v.x, v.z);
                case Orientation::X_UP:
                    return glm::vec3(-v.y, v.x, v.z);
            }
            return glm::vec3(v.x, v.y, v.z);
        }

        glm::quat FixOrientation(const glm::quat& v)
        {
            switch (orientation) {
                case Orientation::Y_UP:
                    return glm::quat(v.x, v.y, v.z, v.w);
                case Orientation::Z_UP:
                    return glm::quat(v.x, v.z, -v.y, v.w);
                case Orientation::Z_MINUS_UP:
                    return glm::quat(v.x, -v.z, v.y, v.w);
                case Orientation::X_MINUS_UP:
                    return glm::quat(v.y, -v.x, v.z, v.w);
                case Orientation::X_UP:
                    return glm::quat(-v.y, v.x, v.z, v.w);
            }
            return glm::quat(v.x, v.y, v.z, v.w);
        }

        static void computeTangents(ofbx::Vec3* out, int vertex_count, const ofbx::Vec3* vertices, const ofbx::Vec3* normals, const ofbx::Vec2* uvs)
        {
            for (int i = 0; i < vertex_count; i += 3) {
                const ofbx::Vec3 v0  = vertices[i + 0];
                const ofbx::Vec3 v1  = vertices[i + 1];
                const ofbx::Vec3 v2  = vertices[i + 2];
                const ofbx::Vec2 uv0 = uvs[i + 0];
                const ofbx::Vec2 uv1 = uvs[i + 1];
                const ofbx::Vec2 uv2 = uvs[i + 2];

                const ofbx::Vec3 dv10  = v1 - v0;
                const ofbx::Vec3 dv20  = v2 - v0;
                const ofbx::Vec2 duv10 = uv1 - uv0;
                const ofbx::Vec2 duv20 = uv2 - uv0;

                const f32 dir = duv20.x * duv10.y - duv20.y * duv10.x < 0 ? -1.f : 1.f;
                ofbx::Vec3  tangent;
                tangent.x     = (dv20.x * duv10.y - dv10.x * duv20.y) * dir;
                tangent.y     = (dv20.y * duv10.y - dv10.y * duv20.y) * dir;
                tangent.z     = (dv20.z * duv10.y - dv10.z * duv20.y) * dir;
                const f32 l = 1 / sqrtf(f32(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z));
                tangent.x *= l;
                tangent.y *= l;
                tangent.z *= l;
                out[i + 0] = tangent;
                out[i + 1] = tangent;
                out[i + 2] = tangent;
            }
        }

        glm::vec2 ToGLMVector(const ofbx::Vec2& vec)
        {
            return glm::vec2(f32(vec.x), f32(vec.y));
        }

        glm::vec3 ToGLMVector(const ofbx::Vec3& vec)
        {
            return glm::vec3(f32(vec.x), f32(vec.y), f32(vec.z));
        }

        glm::vec4 ToGLMVector(const ofbx::Vec4& vec)
        {
            return glm::vec4(f32(vec.x), f32(vec.y), f32(vec.z), f32(vec.w));
        }

        glm::vec4 ToGLMVector(const ofbx::Color& vec)
        {
            return glm::vec4(f32(vec.r), f32(vec.g), f32(vec.b), 1.0f);
        }

        glm::quat ToGLMQuat(const ofbx::Quat& quat)
        {
            return glm::quat(f32(quat.x), f32(quat.y), f32(quat.z), f32(quat.w));
        }

        void RZModel::loadFBX(const std::string& path)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::string err;
            std::string pathCopy = path;
            pathCopy             = Utilities::BackSlashesToSlashes(pathCopy);
            m_FBXModelDirectory  = pathCopy.substr(0, pathCopy.find_last_of('/'));

            std::string name = m_FBXModelDirectory.substr(m_FBXModelDirectory.find_last_of('/') + 1);

            std::string ext  = Utilities::GetFilePathExtension(path);
            int64_t     size = RZFileSystem::GetFileSize(path);
            auto        data = RZFileSystem::ReadFile(path);

            RAZIX_CORE_TRACE("Loading model Name : {0}, Size : {1}", name, size);

            if (data == nullptr) {
                RAZIX_CORE_ERROR("Failed to load fbx file!");
                return;
            }

            const bool     ignoreGeometry = false;
            const uint64_t flags          = ignoreGeometry ? (uint64_t) ofbx::LoadFlags::IGNORE_GEOMETRY : (uint64_t) ofbx::LoadFlags::TRIANGULATE;

            // Load the scene that has models, materials and animation data
            ofbx::IScene* scene = ofbx::load(data, u32(size), flags);

            err = ofbx::getError();

            if (!err.empty() || !scene)
                RAZIX_CORE_ERROR(err);

            const ofbx::GlobalSettings* settings = scene->getGlobalSettings();
            switch (settings->UpAxis) {
                case ofbx::UpVector_AxisX:
                    orientation = Orientation::X_UP;
                    break;
                case ofbx::UpVector_AxisY:
                    orientation = Orientation::Y_UP;
                    break;
                case ofbx::UpVector_AxisZ:
                    orientation = Orientation::Z_UP;
                    break;
            }

            // Iterate through the sub-meshes in the model
            int c = scene->getMeshCount();
            for (int i = 0; i < c; ++i) {
                // Get the FBX mesh
                const ofbx::Mesh* fbx_mesh = (const ofbx::Mesh*) scene->getMesh(i);
                auto              geom     = fbx_mesh->getGeometry();

                auto numIndices   = geom->getIndexCount();
                int  vertex_count = geom->getVertexCount();

                const ofbx::Vec3* vertices = geom->getVertices();
                const ofbx::Vec3* normals  = geom->getNormals();
                const ofbx::Vec3* tangents = geom->getTangents();
                const ofbx::Vec4* colours  = geom->getColors();
                const ofbx::Vec2* uvs      = geom->getUVs();

                // Filling in the Razix vertex data to create Razix Mesh
                Graphics::RZVertex* tempvertices = new Graphics::RZVertex[vertex_count];
                u32*           indicesArray = new u32[numIndices];

                // IDK wtf is this
                auto indices = geom->getFaceIndices();

                // Generate tangents for the model
                ofbx::Vec3* generatedTangents = nullptr;
                if (!tangents && normals && uvs) {
                    generatedTangents = new ofbx::Vec3[vertex_count];
                    computeTangents(generatedTangents, vertex_count, vertices, normals, uvs);
                    tangents = generatedTangents;
                }

                // Fill in the vertex data using vertices, normals and vertex colors and uv cooridinates
                for (int i = 0; i < vertex_count; ++i) {
                    ofbx::Vec3 cp = vertices[i];

                    auto& vertex    = tempvertices[i];
                    vertex.Position = glm::vec3(f32(cp.x), f32(cp.y), f32(cp.z));
                    FixOrientation(vertex.Position);

                    if (normals)
                        vertex.Normal = glm::vec3(f32(normals[i].x), f32(normals[i].y), f32(normals[i].z));
                    if (uvs)
                        vertex.TexCoords = glm::vec2(f32(uvs[i].x), 1.0f - f32(uvs[i].y));
                    if (colours)
                        vertex.Color = glm::vec4(f32(colours[i].x), f32(colours[i].y), f32(colours[i].z), f32(colours[i].w));
                    //if (tangents)
                    //  vertex.Tangent = glm::vec3(f32(tangents[i].x), f32(tangents[i].y), f32(tangents[i].z));

                    FixOrientation(vertex.Normal);
                    //FixOrientation(vertex.Tangent);
                }

                // Fix the indices count properly
                for (int i = 0; i < numIndices; i++) {
                    int index       = (i % 3 == 2) ? (-indices[i] - 1) : indices[i];
                    indicesArray[i] = index;
                }

                // Create the vertex buffer to hold the vertex data of the mesh
                Graphics::RZVertexBuffer* vb = Graphics::RZVertexBuffer::Create(sizeof(Graphics::RZVertex) * vertex_count, tempvertices, BufferUsage::STATIC RZ_DEBUG_NAME_TAG_STR_E_ARG(name));

                // Create the Index buffer to hold the index data
                Graphics::RZIndexBuffer* ib = Graphics::RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG(name)  indicesArray, numIndices);

                // TODO: Query and iterate through the materials and create mat+ texture resources

                // Create the mesh
                // TODO: Set the local transformation i.e. Update the Transform Component
                Graphics::RZMesh* mesh = new Graphics::RZMesh(vb, ib, vertex_count, numIndices);

                mesh->setName(fbx_mesh->name);
                m_Meshes.push_back(mesh);

                delete[] tempvertices;
                delete[] indicesArray;
            }
        }
    }    // namespace Graphics
}    // namespace Razix