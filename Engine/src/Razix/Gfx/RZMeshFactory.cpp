// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMeshFactory.h"

#include "Razix/Gfx/RZMesh.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Razix {
    namespace Gfx {
        namespace MeshFactory {

            static RZMesh* planeMesh  = nullptr;
            static RZMesh* cubeMesh   = nullptr;
            static RZMesh* sphereMesh = nullptr;

            RZMesh* CreatePrimitive(MeshPrimitive primitive)
            {
                switch (primitive) {
                    case MeshPrimitive::Plane: {
                        if (planeMesh == nullptr)
                            planeMesh = CreatePlane();
                        return planeMesh;
                    }
                    case MeshPrimitive::Cube: {
                        if (cubeMesh == nullptr)
                            cubeMesh = CreateCube();
                        return cubeMesh;
                    }
                    case MeshPrimitive::Sphere: {
                        if (sphereMesh == nullptr)
                            sphereMesh = CreateSphere();
                        return sphereMesh;
                    }
                    case MeshPrimitive::Pyramid:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    case MeshPrimitive::Capsule:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    case MeshPrimitive::Cylinder:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    default: {
                        if (cubeMesh == nullptr)
                            cubeMesh = CreateCube();
                        return cubeMesh;
                    }
                }
                return nullptr;
            }

#define NUM_QUAD_VERTS 4
#define NUM_CUBE_VERTS 24

            RZMesh* CreatePlane(f32 width, f32 height, const glm::vec4 color /*= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)*/)
            {
                RZVertex data{};
                data.Position.resize(NUM_QUAD_VERTS);
                data.Color.resize(NUM_QUAD_VERTS);
                data.UV.resize(NUM_QUAD_VERTS);
                data.Normal.resize(NUM_QUAD_VERTS);

                glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data.Position[0] = glm::vec3(-width / 2.0f, -1.0f, -height / 2.0f);
                data.Color[0]    = color;
                data.UV[0]       = glm::vec2(0.0f, 0.0f);
                data.Normal[0]   = normal;

                data.Position[1] = glm::vec3(-width / 2.0f, -1.0f, height / 2.0f);
                data.Color[1]    = color;
                data.UV[1]       = glm::vec2(0.0f, 1.0f);
                data.Normal[1]   = normal;

                data.Position[2] = glm::vec3(width / 2.0f, -1.0f, height / 2.0f);
                data.Color[2]    = color;
                data.UV[2]       = glm::vec2(1.0f, 1.0f);
                data.Normal[2]   = normal;

                data.Position[3] = glm::vec3(width / 2.0f, -1.0f, -height / 2.0f);
                data.Color[3]    = color;
                data.UV[3]       = glm::vec2(1.0f, 0.0f);
                data.Normal[3]   = normal;

                u32 indices[6]{
                    0, 1, 2, 2, 3, 0};

                RZMesh* mesh = new RZMesh(data, indices, 6);

                auto        shader                  = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
                RZMaterial* forwardRendererMaterial = new RZMaterial(shader);
                forwardRendererMaterial->createDescriptorSet();
                mesh->setMaterial(forwardRendererMaterial);

                return mesh;
            }

            RZMesh* CreateCube()
            {
                //    v6----- v5
                //   /|      /|
                //  v1------v0|
                //  | |     | |
                //  | |v7---|-|v4
                //  |/      |/
                //  v2------v3
                RZVertex data{};
                data.Position.resize(NUM_CUBE_VERTS);
                data.Color.resize(NUM_CUBE_VERTS);
                data.UV.resize(NUM_CUBE_VERTS);
                data.Normal.resize(NUM_CUBE_VERTS);

                data.Position[0]  = glm::vec3(1.0f, 1.0f, 1.0f);
                data.Color[0]     = glm::vec4(0.0f);
                data.Normal[0]    = glm::vec3(0.0f, 0.0f, 1.0f);
                data.Position[1]  = glm::vec3(-1.0f, 1.0f, 1.0f);
                data.Color[1]     = glm::vec4(0.0f);
                data.Normal[1]    = glm::vec3(0.0f, 0.0f, 1.0f);
                data.Position[2]  = glm::vec3(-1.0f, -1.0f, 1.0f);
                data.Color[2]     = glm::vec4(0.0f);
                data.Normal[2]    = glm::vec3(0.0f, 0.0f, 1.0f);
                data.Position[3]  = glm::vec3(1.0f, -1.0f, 1.0f);
                data.Color[3]     = glm::vec4(0.0f);
                data.Normal[3]    = glm::vec3(0.0f, 0.0f, 1.0f);
                data.Position[4]  = glm::vec3(1.0f, 1.0f, 1.0f);
                data.Color[4]     = glm::vec4(0.0f);
                data.Normal[4]    = glm::vec3(1.0f, 0.0f, 0.0f);
                data.Position[5]  = glm::vec3(1.0f, -1.0f, 1.0f);
                data.Color[5]     = glm::vec4(0.0f);
                data.Normal[5]    = glm::vec3(1.0f, 0.0f, 0.0f);
                data.Position[6]  = glm::vec3(1.0f, -1.0f, -1.0f);
                data.Color[6]     = glm::vec4(0.0f);
                data.Normal[6]    = glm::vec3(1.0f, 0.0f, 0.0f);
                data.Position[7]  = glm::vec3(1.0f, 1.0f, -1.0f);
                data.Color[7]     = glm::vec4(0.0f);
                data.UV[7]        = glm::vec2(0.0f, 1.0f);
                data.Normal[7]    = glm::vec3(1.0f, 0.0f, 0.0f);
                data.Position[8]  = glm::vec3(1.0f, 1.0f, 1.0f);
                data.Color[8]     = glm::vec4(0.0f);
                data.Normal[8]    = glm::vec3(0.0f, 1.0f, 0.0f);
                data.Position[9]  = glm::vec3(1.0f, 1.0f, -1.0f);
                data.Color[9]     = glm::vec4(0.0f);
                data.Normal[9]    = glm::vec3(0.0f, 1.0f, 0.0f);
                data.Position[10] = glm::vec3(-1.0f, 1.0f, -1.0f);
                data.Color[10]    = glm::vec4(0.0f);
                data.UV[10]       = glm::vec2(0.0f, 1.0f);
                data.Normal[10]   = glm::vec3(0.0f, 1.0f, 0.0f);
                data.Position[11] = glm::vec3(-1.0f, 1.0f, 1.0f);
                data.Color[11]    = glm::vec4(0.0f);
                data.Normal[11]   = glm::vec3(0.0f, 1.0f, 0.0f);
                data.Position[12] = glm::vec3(-1.0f, 1.0f, 1.0f);
                data.Color[12]    = glm::vec4(0.0f);
                data.Normal[12]   = glm::vec3(-1.0f, 0.0f, 0.0f);
                data.Position[13] = glm::vec3(-1.0f, 1.0f, -1.0f);
                data.Color[13]    = glm::vec4(0.0f);
                data.Normal[13]   = glm::vec3(-1.0f, 0.0f, 0.0f);
                data.Position[14] = glm::vec3(-1.0f, -1.0f, -1.0f);
                data.Color[14]    = glm::vec4(0.0f);
                data.Normal[14]   = glm::vec3(-1.0f, 0.0f, 0.0f);
                data.Position[15] = glm::vec3(-1.0f, -1.0f, 1.0f);
                data.Color[15]    = glm::vec4(0.0f);
                data.Normal[15]   = glm::vec3(-1.0f, 0.0f, 0.0f);
                data.Position[16] = glm::vec3(-1.0f, -1.0f, -1.0f);
                data.Color[16]    = glm::vec4(0.0f);
                data.Normal[16]   = glm::vec3(0.0f, -1.0f, 0.0f);
                data.Position[17] = glm::vec3(1.0f, -1.0f, -1.0f);
                data.Color[17]    = glm::vec4(0.0f);
                data.Normal[17]   = glm::vec3(0.0f, -1.0f, 0.0f);
                data.Position[18] = glm::vec3(1.0f, -1.0f, 1.0f);
                data.Color[18]    = glm::vec4(0.0f);
                data.Normal[18]   = glm::vec3(0.0f, -1.0f, 0.0f);
                data.Position[19] = glm::vec3(-1.0f, -1.0f, 1.0f);
                data.Color[19]    = glm::vec4(0.0f);
                data.Normal[19]   = glm::vec3(0.0f, -1.0f, 0.0f);
                data.Position[20] = glm::vec3(1.0f, -1.0f, -1.0f);
                data.Color[20]    = glm::vec4(0.0f);
                data.Normal[20]   = glm::vec3(0.0f, 0.0f, -1.0f);
                data.Position[21] = glm::vec3(-1.0f, -1.0f, -1.0f);
                data.Color[21]    = glm::vec4(0.0f);
                data.Normal[21]   = glm::vec3(0.0f, 0.0f, -1.0f);
                data.Position[22] = glm::vec3(-1.0f, 1.0f, -1.0f);
                data.Color[22]    = glm::vec4(0.0f);
                data.Normal[22]   = glm::vec3(0.0f, 0.0f, -1.0f);
                data.Position[23] = glm::vec3(1.0f, 1.0f, -1.0f);
                data.Color[23]    = glm::vec4(0.0f);
                data.Normal[23]   = glm::vec3(0.0f, 0.0f, -1.0f);

                // UVs
                for (int i = 0; i < 6; i++) {
                    data.UV[i * 4 + 0] = glm::vec2(0.0f, 0.0f);
                    data.UV[i * 4 + 1] = glm::vec2(1.0f, 0.0f);
                    data.UV[i * 4 + 2] = glm::vec2(1.0f, 1.0f);
                    data.UV[i * 4 + 3] = glm::vec2(0.0f, 1.0f);
                }

                u32 indices[36]{
                    0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

                RZMesh* mesh = new RZMesh(data, indices, 36);

//                auto        shader                  = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
//                RZMaterial* forwardRendererMaterial = new RZMaterial(shader);
//                forwardRendererMaterial->createDescriptorSet();
//                mesh->setMaterial(forwardRendererMaterial);

                return mesh;
            }

            RZMesh* CreateSphere(u32 xSegments /*= 64*/, u32 ySegments /*= 64*/)
            {
                RZVertex data{};

                f32 sectorCount = static_cast<f32>(xSegments);
                f32 stackCount  = static_cast<f32>(ySegments);
                f32 sectorStep  = static_cast<f32>(2 * M_PI / sectorCount);
                f32 stackStep   = static_cast<f32>(M_PI / stackCount);
                f32 radius      = 1.0f;

                for (int i = 0; i <= stackCount; ++i) {
                    f32 stackAngle = static_cast<f32>(M_PI / 2 - i * stackStep);    // starting from pi/2 to -pi/2
                    f32 xy         = radius * cos(stackAngle);                      // r * cos(u)
                    f32 z          = radius * sin(stackAngle);                      // r * sin(u)

                    // add (sectorCount+1) vertices per stack
                    // the first and last vertices have same position and normal, but different tex coords
                    for (int j = 0; j <= sectorCount; ++j) {
                        f32 sectorAngle = j * sectorStep;    // starting from 0 to 2pi

                        // vertex position (x, y, z)
                        f32 x = xy * cosf(sectorAngle);    // r * cos(u) * cos(v)
                        f32 y = xy * sinf(sectorAngle);    // r * cos(u) * sin(v)

                        // vertex tex coord (s, t) range between [0, 1]
                        f32 s = static_cast<f32>(j / sectorCount);
                        f32 t = static_cast<f32>(i / stackCount);

                        data.Position.push_back(glm::vec3(x, y, z));
                        data.UV.push_back(glm::vec2(s, t));
                        data.Normal.push_back(glm::normalize(glm::vec3(x, y, z)));
                    }
                }

                std::vector<u32> indices;
                u16              k1, k2;
                for (u16 i = 0; i < stackCount; ++i) {
                    k1 = i * (static_cast<u16>(sectorCount) + 1U);    // beginning of current stack
                    k2 = k1 + static_cast<u16>(sectorCount) + 1U;     // beginning of next stack

                    for (u16 j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                        // 2 triangles per sector excluding first and last stacks
                        // k1 => k2 => k1+1
                        if (i != 0) {
                            indices.push_back(k1);
                            indices.push_back(k2);
                            indices.push_back(k1 + 1);
                        }

                        // k1+1 => k2 => k2+1
                        if (i != (stackCount - 1)) {
                            indices.push_back(k1 + 1);
                            indices.push_back(k2);
                            indices.push_back(k2 + 1);
                        }
                    }
                }

                RZMesh* mesh = new RZMesh(data, indices);

                //auto        shader                  = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
                //RZMaterial* forwardRendererMaterial = new RZMaterial(shader);
                //forwardRendererMaterial->createDescriptorSet();
                //mesh->setMaterial(forwardRendererMaterial);

                return mesh;
            }
        }    // namespace MeshFactory
    }    // namespace Gfx
}    // namespace Razix
