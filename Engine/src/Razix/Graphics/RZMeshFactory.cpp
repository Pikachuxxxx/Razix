#include "rzxpch.h"
#include "RZMeshFactory.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZTexture.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Razix {
    namespace Graphics {
        namespace MeshFactory {
            RZMesh* CreatePrimitive(MeshPrimitive primitive)
            {
                switch (primitive) {
                    case MeshPrimitive::Plane:
                        return CreatePlane();
                        break;
                    case MeshPrimitive::Quad:
                        RAZIX_UNIMPLEMENTED_METHOD
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

            RZMesh* CreateQuad()
            {
                return nullptr;
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
                data[0].Color = glm::vec4(0.0f);
                data[0].Normal = glm::vec3(0.0f, 0.0f, 1.0f);

                data[1].Position = glm::vec3(-1.0f, 1.0f, 1.0f);
                data[1].Color = glm::vec4(0.0f);
                data[1].Normal = glm::vec3(0.0f, 0.0f, 1.0f);

                data[2].Position = glm::vec3(-1.0f, -1.0f, 1.0f);
                data[2].Color = glm::vec4(0.0f);
                data[2].Normal = glm::vec3(0.0f, 0.0f, 1.0f);

                data[3].Position = glm::vec3(1.0f, -1.0f, 1.0f);
                data[3].Color = glm::vec4(0.0f);
                data[3].Normal = glm::vec3(0.0f, 0.0f, 1.0f);

                data[4].Position = glm::vec3(1.0f, 1.0f, 1.0f);
                data[4].Color = glm::vec4(0.0f);
                data[4].Normal = glm::vec3(1.0f, 0.0f, 0.0f);

                data[5].Position = glm::vec3(1.0f, -1.0f, 1.0f);
                data[5].Color = glm::vec4(0.0f);
                data[5].Normal = glm::vec3(1.0f, 0.0f, 0.0f);

                data[6].Position = glm::vec3(1.0f, -1.0f, -1.0f);
                data[6].Color = glm::vec4(0.0f);
                data[6].Normal = glm::vec3(1.0f, 0.0f, 0.0f);

                data[7].Position = glm::vec3(1.0f, 1.0f, -1.0f);
                data[7].Color = glm::vec4(0.0f);
                data[7].TexCoords = glm::vec2(0.0f, 1.0f);
                data[7].Normal = glm::vec3(1.0f, 0.0f, 0.0f);

                data[8].Position = glm::vec3(1.0f, 1.0f, 1.0f);
                data[8].Color = glm::vec4(0.0f);
                data[8].Normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data[9].Position = glm::vec3(1.0f, 1.0f, -1.0f);
                data[9].Color = glm::vec4(0.0f);
                data[9].Normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data[10].Position = glm::vec3(-1.0f, 1.0f, -1.0f);
                data[10].Color = glm::vec4(0.0f);
                data[10].TexCoords = glm::vec2(0.0f, 1.0f);
                data[10].Normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data[11].Position = glm::vec3(-1.0f, 1.0f, 1.0f);
                data[11].Color = glm::vec4(0.0f);
                data[11].Normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data[12].Position = glm::vec3(-1.0f, 1.0f, 1.0f);
                data[12].Color = glm::vec4(0.0f);
                data[12].Normal = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[13].Position = glm::vec3(-1.0f, 1.0f, -1.0f);
                data[13].Color = glm::vec4(0.0f);
                data[13].Normal = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[14].Position = glm::vec3(-1.0f, -1.0f, -1.0f);
                data[14].Color = glm::vec4(0.0f);
                data[14].Normal = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[15].Position = glm::vec3(-1.0f, -1.0f, 1.0f);
                data[15].Color = glm::vec4(0.0f);
                data[15].Normal = glm::vec3(-1.0f, 0.0f, 0.0f);

                data[16].Position = glm::vec3(-1.0f, -1.0f, -1.0f);
                data[16].Color = glm::vec4(0.0f);
                data[16].Normal = glm::vec3(0.0f, -1.0f, 0.0f);

                data[17].Position = glm::vec3(1.0f, -1.0f, -1.0f);
                data[17].Color = glm::vec4(0.0f);
                data[17].Normal = glm::vec3(0.0f, -1.0f, 0.0f);

                data[18].Position = glm::vec3(1.0f, -1.0f, 1.0f);
                data[18].Color = glm::vec4(0.0f);
                data[18].Normal = glm::vec3(0.0f, -1.0f, 0.0f);

                data[19].Position = glm::vec3(-1.0f, -1.0f, 1.0f);
                data[19].Color = glm::vec4(0.0f);
                data[19].Normal = glm::vec3(0.0f, -1.0f, 0.0f);

                data[20].Position = glm::vec3(1.0f, -1.0f, -1.0f);
                data[20].Color = glm::vec4(0.0f);
                data[20].Normal = glm::vec3(0.0f, 0.0f, -1.0f);

                data[21].Position = glm::vec3(-1.0f, -1.0f, -1.0f);
                data[21].Color = glm::vec4(0.0f);
                data[21].Normal = glm::vec3(0.0f, 0.0f, -1.0f);

                data[22].Position = glm::vec3(-1.0f, 1.0f, -1.0f);
                data[22].Color = glm::vec4(0.0f);
                data[22].Normal = glm::vec3(0.0f, 0.0f, -1.0f);

                data[23].Position = glm::vec3(1.0f, 1.0f, -1.0f);
                data[23].Color = glm::vec4(0.0f);
                data[23].Normal = glm::vec3(0.0f, 0.0f, -1.0f);

                for (int i = 0; i < 6; i++) {
                    data[i * 4 + 0].TexCoords = glm::vec2(0.0f, 0.0f);
                    data[i * 4 + 1].TexCoords = glm::vec2(1.0f, 0.0f);
                    data[i * 4 + 2].TexCoords = glm::vec2(1.0f, 1.0f);
                    data[i * 4 + 3].TexCoords = glm::vec2(0.0f, 1.0f);
                }

                RZVertexBuffer* vb = RZVertexBuffer::Create(24 * sizeof(RZVertex), data,BufferUsage::STATIC, "Cube");
                delete[] data;

                uint16_t indices[36]{
                    0, 1, 2,
                    0, 2, 3,
                    4, 5, 6,
                    4, 6, 7,
                    8, 9, 10,
                    8, 10, 11,
                    12, 13, 14,
                    12, 14, 15,
                    16, 17, 18,
                    16, 18, 19,
                    20, 21, 22,
                    20, 22, 23
                };

                RZIndexBuffer* ib = RZIndexBuffer::Create(indices, 36, "Cube");

                return new RZMesh(vb, ib, 24, 36);
            }

            RZMesh* CreateSphere(uint32_t xSegments /*= 64*/, uint32_t ySegments /*= 64*/)
            {
                auto data = std::vector<RZVertex>();

                float sectorCount = static_cast<float>(xSegments);
                float stackCount = static_cast<float>(ySegments);
                float sectorStep = 2 * M_PI / sectorCount;
                float stackStep = M_PI / stackCount;
                float radius = 1.0f;

                for (int i = 0; i <= stackCount; ++i) {
                    float stackAngle = M_PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
                    float xy = radius * cos(stackAngle); // r * cos(u)
                    float z = radius * sin(stackAngle); // r * sin(u)

                    // add (sectorCount+1) vertices per stack
                    // the first and last vertices have same position and normal, but different tex coords
                    for (int j = 0; j <= sectorCount; ++j) {
                        float sectorAngle = j * sectorStep; // starting from 0 to 2pi

                        // vertex position (x, y, z)
                        float x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
                        float y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

                        // vertex tex coord (s, t) range between [0, 1]
                        float s = static_cast<float>(j / sectorCount);
                        float t = static_cast<float>(i / stackCount);

                        Graphics::RZVertex vertex;
                        vertex.Position = glm::vec3(x, y, z);
                        vertex.TexCoords = glm::vec2(s, t);
                        vertex.Normal = glm::normalize(glm::vec3(x, y, z));

                        data.emplace_back(vertex);
                    }
                }

                RZVertexBuffer* vb = RZVertexBuffer::Create(sizeof(RZVertex) * int(data.size()), data.data(), BufferUsage::STATIC, "Sphere");

                std::vector<uint16_t> indices;
                uint16_t k1, k2;
                for (uint16_t i = 0; i < stackCount; ++i) {
                    k1 = i * (static_cast<uint16_t>(sectorCount) + 1U); // beginning of current stack
                    k2 = k1 + static_cast<uint16_t>(sectorCount) + 1U; // beginning of next stack

                    for (uint16_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
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

                RZIndexBuffer* ib = RZIndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()), "Sphere");

                return new RZMesh(vb, ib, data.size(), indices.size());
            }

            RZMesh* CreatePlane(float width, float height, const glm::vec4 color /*= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)*/)
            {
                RZVertex* data = new RZVertex[4];

                glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

                data[0].Position = glm::vec3(-width / 2.0f, -1.0f, -height / 2.0f);
                data[0].Color = color;
                data[0].TexCoords = glm::vec2(0.0f, 0.0f);
                data[0].Normal = normal;

                data[1].Position = glm::vec3(-width / 2.0f, -1.0f, height / 2.0f);
                data[1].Color = color;
                data[1].TexCoords = glm::vec2(0.0f, 1.0f);
                data[1].Normal = normal;

                data[2].Position = glm::vec3(width / 2.0f, -1.0f, height / 2.0f);
                data[2].Color = color;
                data[2].TexCoords = glm::vec2(1.0f, 1.0f);
                data[2].Normal = normal;

                data[3].Position = glm::vec3(width / 2.0f, -1.0f, -height / 2.0f);
                data[3].Color = color;
                data[3].TexCoords = glm::vec2(1.0f, 0.0f);
                data[3].Normal = normal;

                RZVertexBuffer* vb = RZVertexBuffer::Create(4 * sizeof(RZVertex), data, BufferUsage::STATIC, "Plane");
                delete[] data;

                uint16_t indices[6]{
                    0, 1, 2,
                    2, 3, 0
                };

                RZIndexBuffer* ib = RZIndexBuffer::Create(indices, 6, "Plane");

                return new RZMesh(vb, ib, 4, 6);
            }

            RZMesh* CreateScreenQuad()
            {
                return nullptr;
            }
        }
    }
}