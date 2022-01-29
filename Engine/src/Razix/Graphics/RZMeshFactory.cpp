#include "rzxpch.h"
#include "RZMeshFactory.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/API/RZTexture.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Razix {
    namespace Graphics {

        RZMesh* CreatePrimitive(MeshPrimitive primitive)
        {
            switch (primitive) {
                case MeshPrimitive::Plane:
                    break;
                case MeshPrimitive::Quad:
                    break;
                case MeshPrimitive::Cube:
                    break;
                case MeshPrimitive::Pyramid:
                    break;
                case MeshPrimitive::Sphere:
                    break;
                case MeshPrimitive::Capsule:
                    break;
                case MeshPrimitive::Cylinder:
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

            RZVertexBuffer* vb = RZVertexBuffer::Create(sizeof(RZVertex) * int(data.size()), data.data(), BufferUsage::STATIC);

            std::vector<uint32_t> indices;
            uint32_t k1, k2;
            for (uint32_t i = 0; i < stackCount; ++i) {
                k1 = i * (static_cast<uint32_t>(sectorCount) + 1U); // beginning of current stack
                k2 = k1 + static_cast<uint32_t>(sectorCount) + 1U; // beginning of next stack

                for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
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

            RZIndexBuffer* ib = RZIndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));

            return new RZMesh(vb, ib, data.size(), indices.size());
        }

        RZMesh* CreateScreenQuad()
        {
            return nullptr;
        }
    }
}