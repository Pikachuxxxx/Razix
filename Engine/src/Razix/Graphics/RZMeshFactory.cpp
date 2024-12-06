// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMeshFactory.h"

#include "Razix/Graphics/RZMesh.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Razix {
    namespace Graphics {
        namespace MeshFactory {

            static RZMesh* ScreenMesh = nullptr;
            static RZMesh* planeMesh  = nullptr;
            static RZMesh* cubeMesh   = nullptr;
            static RZMesh* sphereMesh = nullptr;

#if 0
            RZMesh* CreatePrimitive(MeshPrimitive primitive)
            {
                switch (primitive) {
                    case MeshPrimitive::Plane: {
                        if (planeMesh == nullptr)
                            planeMesh = CreatePlane();
                        return planeMesh;
                    }
                    case MeshPrimitive::ScreenQuad: {
                        if (ScreenMesh == nullptr)
                            ScreenMesh = CreateScreenQuad();
                        return ScreenMesh;
                    }
                    case MeshPrimitive::Cube: {
                        if (cubeMesh == nullptr)
                            cubeMesh = CreateCube();
                        return cubeMesh;
                    }
                    case MeshPrimitive::Pyramid:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    case MeshPrimitive::Sphere: {
                        if (sphereMesh == nullptr)
                            sphereMesh = CreateSphere();
                        return sphereMesh;
                    }
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
#endif

            RZMesh* CreatePrimitive(MeshPrimitive primitive)
            {
                switch (primitive) {
                    case MeshPrimitive::Plane:
                        return CreatePlane();
                        break;
                    case MeshPrimitive::ScreenQuad:
                        return CreateScreenQuad();
                        break;
                    case MeshPrimitive::Cube:
                        return CreateCube();
                        break;
                    case MeshPrimitive::Pyramid:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    case MeshPrimitive::Sphere:
                        return CreateSphere();
                        break;
                    case MeshPrimitive::Capsule:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    case MeshPrimitive::Cylinder:
                        RAZIX_UNIMPLEMENTED_METHOD
                        break;
                    default:
                        break;
                }
                return nullptr;
            }

            RZMesh* CreatePlane(f32 width, f32 height, const glm::vec4 color /*= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)*/)
            {
                RZVertex* data = new RZVertex[4];

                glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data[0].Position  = glm::vec3(-width / 2.0f, -1.0f, -height / 2.0f);
                data[0].Color     = color;
                data[0].UV = glm::vec2(0.0f, 0.0f);
                data[0].Normal    = normal;

                data[1].Position  = glm::vec3(-width / 2.0f, -1.0f, height / 2.0f);
                data[1].Color     = color;
                data[1].UV = glm::vec2(0.0f, 1.0f);
                data[1].Normal    = normal;

                data[2].Position  = glm::vec3(width / 2.0f, -1.0f, height / 2.0f);
                data[2].Color     = color;
                data[2].UV = glm::vec2(1.0f, 1.0f);
                data[2].Normal    = normal;

                data[3].Position  = glm::vec3(width / 2.0f, -1.0f, -height / 2.0f);
                data[3].Color     = color;
                data[3].UV = glm::vec2(1.0f, 0.0f);
                data[3].Normal    = normal;

                RZVertexBuffer*      vb = RZVertexBuffer::Create(4 * sizeof(RZVertex), data, BufferUsage::Static RZ_DEBUG_NAME_TAG_STR_E_ARG("Plane"));
                RZVertexBufferLayout layout;
                layout.push<glm::vec3>("Position");
                layout.push<glm::vec4>("Color");
                layout.push<glm::vec2>("TexCoords");
                layout.push<glm::vec3>("Normal");
                layout.push<glm::vec3>("Tangent");
                vb->AddBufferLayout(layout);
                delete[] data;

                u32 indices[6]{
                    0, 1, 2, 2, 3, 0};

                RZIndexBuffer* ib = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Plane") indices, 6);

                RZMesh* mesh = new RZMesh(vb, ib, 4, 6);

                auto        shader                  = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
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
                RZVertex* data = new RZVertex[24];

                data[0].Position = glm::vec3(1.0f, 1.0f, 1.0f);
                data[0].Color    = glm::vec4(0.0f);
                data[0].Normal   = glm::vec3(0.0f, 0.0f, 1.0f);

                data[1].Position = glm::vec3(-1.0f, 1.0f, 1.0f);
                data[1].Color    = glm::vec4(0.0f);
                data[1].Normal   = glm::vec3(0.0f, 0.0f, 1.0f);

                data[2].Position = glm::vec3(-1.0f, -1.0f, 1.0f);
                data[2].Color    = glm::vec4(0.0f);
                data[2].Normal   = glm::vec3(0.0f, 0.0f, 1.0f);

                data[3].Position = glm::vec3(1.0f, -1.0f, 1.0f);
                data[3].Color    = glm::vec4(0.0f);
                data[3].Normal   = glm::vec3(0.0f, 0.0f, 1.0f);

                data[4].Position = glm::vec3(1.0f, 1.0f, 1.0f);
                data[4].Color    = glm::vec4(0.0f);
                data[4].Normal   = glm::vec3(1.0f, 0.0f, 0.0f);

                data[5].Position = glm::vec3(1.0f, -1.0f, 1.0f);
                data[5].Color    = glm::vec4(0.0f);
                data[5].Normal   = glm::vec3(1.0f, 0.0f, 0.0f);

                data[6].Position = glm::vec3(1.0f, -1.0f, -1.0f);
                data[6].Color    = glm::vec4(0.0f);
                data[6].Normal   = glm::vec3(1.0f, 0.0f, 0.0f);

                data[7].Position  = glm::vec3(1.0f, 1.0f, -1.0f);
                data[7].Color     = glm::vec4(0.0f);
                data[7].UV = glm::vec2(0.0f, 1.0f);
                data[7].Normal    = glm::vec3(1.0f, 0.0f, 0.0f);

                data[8].Position = glm::vec3(1.0f, 1.0f, 1.0f);
                data[8].Color    = glm::vec4(0.0f);
                data[8].Normal   = glm::vec3(0.0f, 1.0f, 0.0f);

                data[9].Position = glm::vec3(1.0f, 1.0f, -1.0f);
                data[9].Color    = glm::vec4(0.0f);
                data[9].Normal   = glm::vec3(0.0f, 1.0f, 0.0f);

                data[10].Position  = glm::vec3(-1.0f, 1.0f, -1.0f);
                data[10].Color     = glm::vec4(0.0f);
                data[10].UV = glm::vec2(0.0f, 1.0f);
                data[10].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);

                data[11].Position = glm::vec3(-1.0f, 1.0f, 1.0f);
                data[11].Color    = glm::vec4(0.0f);
                data[11].Normal   = glm::vec3(0.0f, 1.0f, 0.0f);

                data[12].Position = glm::vec3(-1.0f, 1.0f, 1.0f);
                data[12].Color    = glm::vec4(0.0f);
                data[12].Normal   = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[13].Position = glm::vec3(-1.0f, 1.0f, -1.0f);
                data[13].Color    = glm::vec4(0.0f);
                data[13].Normal   = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[14].Position = glm::vec3(-1.0f, -1.0f, -1.0f);
                data[14].Color    = glm::vec4(0.0f);
                data[14].Normal   = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[15].Position = glm::vec3(-1.0f, -1.0f, 1.0f);
                data[15].Color    = glm::vec4(0.0f);
                data[15].Normal   = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[16].Position = glm::vec3(-1.0f, -1.0f, -1.0f);
                data[16].Color    = glm::vec4(0.0f);
                data[16].Normal   = glm::vec3(0.0f, -1.0f, 0.0f);

                data[17].Position = glm::vec3(1.0f, -1.0f, -1.0f);
                data[17].Color    = glm::vec4(0.0f);
                data[17].Normal   = glm::vec3(0.0f, -1.0f, 0.0f);

                data[18].Position = glm::vec3(1.0f, -1.0f, 1.0f);
                data[18].Color    = glm::vec4(0.0f);
                data[18].Normal   = glm::vec3(0.0f, -1.0f, 0.0f);

                data[19].Position = glm::vec3(-1.0f, -1.0f, 1.0f);
                data[19].Color    = glm::vec4(0.0f);
                data[19].Normal   = glm::vec3(0.0f, -1.0f, 0.0f);

                data[20].Position = glm::vec3(1.0f, -1.0f, -1.0f);
                data[20].Color    = glm::vec4(0.0f);
                data[20].Normal   = glm::vec3(0.0f, 0.0f, -1.0f);

                data[21].Position = glm::vec3(-1.0f, -1.0f, -1.0f);
                data[21].Color    = glm::vec4(0.0f);
                data[21].Normal   = glm::vec3(0.0f, 0.0f, -1.0f);

                data[22].Position = glm::vec3(-1.0f, 1.0f, -1.0f);
                data[22].Color    = glm::vec4(0.0f);
                data[22].Normal   = glm::vec3(0.0f, 0.0f, -1.0f);

                data[23].Position = glm::vec3(1.0f, 1.0f, -1.0f);
                data[23].Color    = glm::vec4(0.0f);
                data[23].Normal   = glm::vec3(0.0f, 0.0f, -1.0f);

                for (int i = 0; i < 6; i++) {
                    data[i * 4 + 0].UV = glm::vec2(0.0f, 0.0f);
                    data[i * 4 + 1].UV = glm::vec2(1.0f, 0.0f);
                    data[i * 4 + 2].UV = glm::vec2(1.0f, 1.0f);
                    data[i * 4 + 3].UV = glm::vec2(0.0f, 1.0f);
                }

                RZVertexBuffer* vb = RZVertexBuffer::Create(24 * sizeof(RZVertex), data, BufferUsage::Static RZ_DEBUG_NAME_TAG_STR_E_ARG("Cube"));
                delete[] data;

                u32 indices[36]{
                    0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};
                RZIndexBuffer* ib = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Cube") indices, 36);

                RZMesh* mesh = new RZMesh(vb, ib, 24, 36);

                auto        shader                  = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
                RZMaterial* forwardRendererMaterial = new RZMaterial(shader);
                forwardRendererMaterial->createDescriptorSet();
                mesh->setMaterial(forwardRendererMaterial);

                return mesh;
            }

            RZMesh* CreateSphere(u32 xSegments /*= 64*/, u32 ySegments /*= 64*/)
            {
                auto data = std::vector<RZVertex>();

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

                        Graphics::RZVertex vertex;
                        vertex.Position  = glm::vec3(x, y, z);
                        vertex.UV = glm::vec2(s, t);
                        vertex.Normal    = glm::normalize(glm::vec3(x, y, z));

                        data.emplace_back(vertex);
                    }
                }

                RZVertexBuffer* vb = RZVertexBuffer::Create(sizeof(RZVertex) * int(data.size()), data.data(), BufferUsage::Static RZ_DEBUG_NAME_TAG_STR_E_ARG("Sphere"));

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

                RZIndexBuffer* ib = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Sphere") indices.data(), static_cast<u32>(indices.size()));

                RZMesh* mesh = new RZMesh(vb, ib, static_cast<u32>(data.size()), static_cast<u32>(indices.size()));

                auto        shader                  = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);
                RZMaterial* forwardRendererMaterial = new RZMaterial(shader);
                forwardRendererMaterial->createDescriptorSet();
                mesh->setMaterial(forwardRendererMaterial);

                return mesh;
            }

            RZMesh* CreateScreenQuad()
            {
                RZSimpleVertex* data = new RZSimpleVertex[4];
                data[0].Position     = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
                data[0].TexCoords    = glm::vec2(0.0f, 0.0f);

                data[1].Position  = glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);
                data[1].TexCoords = glm::vec2(1.0f, 0.0f);

                data[2].Position  = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
                data[2].TexCoords = glm::vec2(1.0f, 1.0f);

                data[3].Position  = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
                data[3].TexCoords = glm::vec2(0.0f, 1.0f);

                RZVertexBuffer*      vb = RZVertexBuffer::Create(4 * sizeof(RZSimpleVertex), data, BufferUsage::Static RZ_DEBUG_NAME_TAG_STR_E_ARG("Screen Quad VB"));
                RZVertexBufferLayout layout;
                layout.push<glm::vec4>("Position");
                layout.push<glm::vec2>("TexCoords");
                vb->AddBufferLayout(layout);
                delete[] data;

                u32 indices[6] = {0, 1, 2, 2, 3, 0};

                RZIndexBuffer* ib = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Screen Quad IB") indices, 6);

                RZMesh* mesh = new RZMesh(vb, ib, 4, 6);
                // Set the material
                auto shader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Composition);

                RZMaterial* compositePassMaterial = new RZMaterial(shader);
                compositePassMaterial->createDescriptorSet();
                mesh->setMaterial(compositePassMaterial);

                return mesh;
            }
        }    // namespace MeshFactory
    }        // namespace Graphics
}    // namespace Razix