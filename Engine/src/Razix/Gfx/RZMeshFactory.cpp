// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMeshFactory.h"

#include "Razix/Gfx/RZMesh.h"

#include "Razix/Gfx/RHI/RHI.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define NUM_QUAD_VERTS    4
#define NUM_CUBE_VERTS    24
#define NUM_PYRAMID_VERTS 16

namespace Razix {
    namespace Gfx {

        RZMesh CreatePlane(f32 width = 1.0f, f32 height = 1.0f, const float4& color = float4(1.0f, 1.0f, 1.0f, 1.0f))
        {
            RZVertex data{};
            data.Position.resize(NUM_QUAD_VERTS);
            data.Color.resize(NUM_QUAD_VERTS);
            data.UV.resize(NUM_QUAD_VERTS);
            data.Normal.resize(NUM_QUAD_VERTS);
            data.Tangent.resize(NUM_QUAD_VERTS);

            float3 normal = float3(0.0f, 1.0f, 0.0f);

            data.Position[0] = float3(-width / 2.0f, -1.0f, -height / 2.0f);
            data.Color[0]    = color;
            data.UV[0]       = float2(0.0f, 0.0f);
            data.Normal[0]   = normal;

            data.Position[1] = float3(-width / 2.0f, -1.0f, height / 2.0f);
            data.Color[1]    = color;
            data.UV[1]       = float2(0.0f, 1.0f);
            data.Normal[1]   = normal;

            data.Position[2] = float3(width / 2.0f, -1.0f, height / 2.0f);
            data.Color[2]    = color;
            data.UV[2]       = float2(1.0f, 1.0f);
            data.Normal[2]   = normal;

            data.Position[3] = float3(width / 2.0f, -1.0f, -height / 2.0f);
            data.Color[3]    = color;
            data.UV[3]       = float2(1.0f, 0.0f);
            data.Normal[3]   = normal;

            RZDynamicArray<u32> indices = {0, 1, 2, 2, 3, 0};

            return RZMesh(data, indices);
        }

        RZMesh CreateCube()
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
            data.Tangent.resize(NUM_CUBE_VERTS);

            data.Position[0]  = float3(1.0f, 1.0f, 1.0f);
            data.Color[0]     = float4(0.0f);
            data.Normal[0]    = float3(0.0f, 0.0f, 1.0f);
            data.Position[1]  = float3(-1.0f, 1.0f, 1.0f);
            data.Color[1]     = float4(0.0f);
            data.Normal[1]    = float3(0.0f, 0.0f, 1.0f);
            data.Position[2]  = float3(-1.0f, -1.0f, 1.0f);
            data.Color[2]     = float4(0.0f);
            data.Normal[2]    = float3(0.0f, 0.0f, 1.0f);
            data.Position[3]  = float3(1.0f, -1.0f, 1.0f);
            data.Color[3]     = float4(0.0f);
            data.Normal[3]    = float3(0.0f, 0.0f, 1.0f);
            data.Position[4]  = float3(1.0f, 1.0f, 1.0f);
            data.Color[4]     = float4(0.0f);
            data.Normal[4]    = float3(1.0f, 0.0f, 0.0f);
            data.Position[5]  = float3(1.0f, -1.0f, 1.0f);
            data.Color[5]     = float4(0.0f);
            data.Normal[5]    = float3(1.0f, 0.0f, 0.0f);
            data.Position[6]  = float3(1.0f, -1.0f, -1.0f);
            data.Color[6]     = float4(0.0f);
            data.Normal[6]    = float3(1.0f, 0.0f, 0.0f);
            data.Position[7]  = float3(1.0f, 1.0f, -1.0f);
            data.Color[7]     = float4(0.0f);
            data.UV[7]        = float2(0.0f, 1.0f);
            data.Normal[7]    = float3(1.0f, 0.0f, 0.0f);
            data.Position[8]  = float3(1.0f, 1.0f, 1.0f);
            data.Color[8]     = float4(0.0f);
            data.Normal[8]    = float3(0.0f, 1.0f, 0.0f);
            data.Position[9]  = float3(1.0f, 1.0f, -1.0f);
            data.Color[9]     = float4(0.0f);
            data.Normal[9]    = float3(0.0f, 1.0f, 0.0f);
            data.Position[10] = float3(-1.0f, 1.0f, -1.0f);
            data.Color[10]    = float4(0.0f);
            data.UV[10]       = float2(0.0f, 1.0f);
            data.Normal[10]   = float3(0.0f, 1.0f, 0.0f);
            data.Position[11] = float3(-1.0f, 1.0f, 1.0f);
            data.Color[11]    = float4(0.0f);
            data.Normal[11]   = float3(0.0f, 1.0f, 0.0f);
            data.Position[12] = float3(-1.0f, 1.0f, 1.0f);
            data.Color[12]    = float4(0.0f);
            data.Normal[12]   = float3(-1.0f, 0.0f, 0.0f);
            data.Position[13] = float3(-1.0f, 1.0f, -1.0f);
            data.Color[13]    = float4(0.0f);
            data.Normal[13]   = float3(-1.0f, 0.0f, 0.0f);
            data.Position[14] = float3(-1.0f, -1.0f, -1.0f);
            data.Color[14]    = float4(0.0f);
            data.Normal[14]   = float3(-1.0f, 0.0f, 0.0f);
            data.Position[15] = float3(-1.0f, -1.0f, 1.0f);
            data.Color[15]    = float4(0.0f);
            data.Normal[15]   = float3(-1.0f, 0.0f, 0.0f);
            data.Position[16] = float3(-1.0f, -1.0f, -1.0f);
            data.Color[16]    = float4(0.0f);
            data.Normal[16]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[17] = float3(1.0f, -1.0f, -1.0f);
            data.Color[17]    = float4(0.0f);
            data.Normal[17]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[18] = float3(1.0f, -1.0f, 1.0f);
            data.Color[18]    = float4(0.0f);
            data.Normal[18]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[19] = float3(-1.0f, -1.0f, 1.0f);
            data.Color[19]    = float4(0.0f);
            data.Normal[19]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[20] = float3(1.0f, -1.0f, -1.0f);
            data.Color[20]    = float4(0.0f);
            data.Normal[20]   = float3(0.0f, 0.0f, -1.0f);
            data.Position[21] = float3(-1.0f, -1.0f, -1.0f);
            data.Color[21]    = float4(0.0f);
            data.Normal[21]   = float3(0.0f, 0.0f, -1.0f);
            data.Position[22] = float3(-1.0f, 1.0f, -1.0f);
            data.Color[22]    = float4(0.0f);
            data.Normal[22]   = float3(0.0f, 0.0f, -1.0f);
            data.Position[23] = float3(1.0f, 1.0f, -1.0f);
            data.Color[23]    = float4(0.0f);
            data.Normal[23]   = float3(0.0f, 0.0f, -1.0f);

            // UVs
            for (int i = 0; i < 6; i++) {
                data.UV[i * 4 + 0] = float2(0.0f, 0.0f);
                data.UV[i * 4 + 1] = float2(1.0f, 0.0f);
                data.UV[i * 4 + 2] = float2(1.0f, 1.0f);
                data.UV[i * 4 + 3] = float2(0.0f, 1.0f);
            }

            RZDynamicArray<u32> indices = {
                0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

            return RZMesh(data, indices);
        }

        RZMesh CreateSphere(u32 xSegments = 64, u32 ySegments = 64)
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

                    data.Position.push_back(float3(x, y, z));
                    data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
                    data.UV.push_back(float2(s, t));
                    data.Normal.push_back(normalize(float3(x, y, z)));
                    data.Tangent.push_back(float3(0.0f));
                }
            }

            RZDynamicArray<u32> indices;
            u16                 k1, k2;
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

            return RZMesh(data, indices);
        }

        RZMesh CreatePyramid()
        {
            //         apex
            //        /|\
            //       / | \
            //      /  |  \
            //   b3 ----+---- b2
            //    |    /|    |
            //    |   / |    |
            //   b0 ---+----- b1
            RZVertex data{};
            data.Position.resize(NUM_PYRAMID_VERTS);
            data.Color.resize(NUM_PYRAMID_VERTS);
            data.UV.resize(NUM_PYRAMID_VERTS);
            data.Normal.resize(NUM_PYRAMID_VERTS);
            data.Tangent.resize(NUM_PYRAMID_VERTS);

            float3 apex = float3(0.0f, 1.0f, 0.0f);
            float3 b0   = float3(-1.0f, -1.0f, -1.0f);
            float3 b1   = float3(1.0f, -1.0f, -1.0f);
            float3 b2   = float3(1.0f, -1.0f, 1.0f);
            float3 b3   = float3(-1.0f, -1.0f, 1.0f);

            // Base (facing down)
            data.Position[0] = b0;
            data.Color[0]    = float4(0.0f);
            data.UV[0]       = float2(0.0f, 0.0f);
            data.Normal[0]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[1] = b1;
            data.Color[1]    = float4(0.0f);
            data.UV[1]       = float2(1.0f, 0.0f);
            data.Normal[1]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[2] = b2;
            data.Color[2]    = float4(0.0f);
            data.UV[2]       = float2(1.0f, 1.0f);
            data.Normal[2]   = float3(0.0f, -1.0f, 0.0f);
            data.Position[3] = b3;
            data.Color[3]    = float4(0.0f);
            data.UV[3]       = float2(0.0f, 1.0f);
            data.Normal[3]   = float3(0.0f, -1.0f, 0.0f);

            // Front face (+z)
            data.Position[4] = apex;
            data.Color[4]    = float4(0.0f);
            data.UV[4]       = float2(0.5f, 0.0f);
            data.Normal[4]   = normalize(float3(0.0f, 1.0f, 2.0f));
            data.Position[5] = b3;
            data.Color[5]    = float4(0.0f);
            data.UV[5]       = float2(0.0f, 1.0f);
            data.Normal[5]   = normalize(float3(0.0f, 1.0f, 2.0f));
            data.Position[6] = b2;
            data.Color[6]    = float4(0.0f);
            data.UV[6]       = float2(1.0f, 1.0f);
            data.Normal[6]   = normalize(float3(0.0f, 1.0f, 2.0f));

            // Right face (+x)
            data.Position[7] = apex;
            data.Color[7]    = float4(0.0f);
            data.UV[7]       = float2(0.5f, 0.0f);
            data.Normal[7]   = normalize(float3(2.0f, 1.0f, 0.0f));
            data.Position[8] = b2;
            data.Color[8]    = float4(0.0f);
            data.UV[8]       = float2(0.0f, 1.0f);
            data.Normal[8]   = normalize(float3(2.0f, 1.0f, 0.0f));
            data.Position[9] = b1;
            data.Color[9]    = float4(0.0f);
            data.UV[9]       = float2(1.0f, 1.0f);
            data.Normal[9]   = normalize(float3(2.0f, 1.0f, 0.0f));

            // Back face (-z)
            data.Position[10] = apex;
            data.Color[10]    = float4(0.0f);
            data.UV[10]       = float2(0.5f, 0.0f);
            data.Normal[10]   = normalize(float3(0.0f, 1.0f, -2.0f));
            data.Position[11] = b1;
            data.Color[11]    = float4(0.0f);
            data.UV[11]       = float2(0.0f, 1.0f);
            data.Normal[11]   = normalize(float3(0.0f, 1.0f, -2.0f));
            data.Position[12] = b0;
            data.Color[12]    = float4(0.0f);
            data.UV[12]       = float2(1.0f, 1.0f);
            data.Normal[12]   = normalize(float3(0.0f, 1.0f, -2.0f));

            // Left face (-x)
            data.Position[13] = apex;
            data.Color[13]    = float4(0.0f);
            data.UV[13]       = float2(0.5f, 0.0f);
            data.Normal[13]   = normalize(float3(-2.0f, 1.0f, 0.0f));
            data.Position[14] = b0;
            data.Color[14]    = float4(0.0f);
            data.UV[14]       = float2(0.0f, 1.0f);
            data.Normal[14]   = normalize(float3(-2.0f, 1.0f, 0.0f));
            data.Position[15] = b3;
            data.Color[15]    = float4(0.0f);
            data.UV[15]       = float2(1.0f, 1.0f);
            data.Normal[15]   = normalize(float3(-2.0f, 1.0f, 0.0f));

            RZDynamicArray<u32> indices = {
                0, 1, 2, 0, 2, 3,
                4, 5, 6,
                7, 8, 9,
                10, 11, 12,
                13, 14, 15};

            return RZMesh(data, indices);
        }

        RZMesh CreateCylinder(u32 sectorCount = 32, f32 radius = 1.0f, f32 height = 2.0f)
        {
            RZVertex data{};

            f32 halfHeight = height * 0.5f;
            f32 sectorStep = static_cast<f32>(2 * M_PI / sectorCount);

            // Side surface: two rings of vertices with radially outward normals
            for (u32 ring = 0; ring < 2; ++ring) {
                f32 y = (ring == 0) ? halfHeight : -halfHeight;
                f32 v = (ring == 0) ? 0.0f : 1.0f;

                for (u32 i = 0; i <= sectorCount; ++i) {
                    f32 sectorAngle = i * sectorStep;
                    f32 x           = radius * cosf(sectorAngle);
                    f32 z           = radius * sinf(sectorAngle);
                    f32 u           = static_cast<f32>(i) / static_cast<f32>(sectorCount);

                    data.Position.push_back(float3(x, y, z));
                    data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
                    data.UV.push_back(float2(u, v));
                    data.Normal.push_back(normalize(float3(x, 0.0f, z)));
                }
            }

            RZDynamicArray<u32> indices;
            u32                 k1, k2;
            for (u32 i = 0; i < sectorCount; ++i) {
                k1 = i;
                k2 = sectorCount + 1 + i;

                indices.push_back(k1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);

                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
                indices.push_back(k2);
            }

            // Top cap (normal +y)
            u32 topCenterIdx = static_cast<u32>(data.Position.size());
            data.Position.push_back(float3(0.0f, halfHeight, 0.0f));
            data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
            data.UV.push_back(float2(0.5f, 0.5f));
            data.Normal.push_back(float3(0.0f, 1.0f, 0.0f));

            u32 topRingStart = static_cast<u32>(data.Position.size());
            for (u32 i = 0; i <= sectorCount; ++i) {
                f32 sectorAngle = i * sectorStep;
                f32 x           = radius * cosf(sectorAngle);
                f32 z           = radius * sinf(sectorAngle);

                data.Position.push_back(float3(x, halfHeight, z));
                data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
                data.UV.push_back(float2(0.5f + 0.5f * cosf(sectorAngle), 0.5f + 0.5f * sinf(sectorAngle)));
                data.Normal.push_back(float3(0.0f, 1.0f, 0.0f));
            }

            for (u32 i = 0; i < sectorCount; ++i) {
                indices.push_back(topCenterIdx);
                indices.push_back(topRingStart + i + 1);
                indices.push_back(topRingStart + i);
            }

            // Bottom cap (normal -y)
            u32 bottomCenterIdx = static_cast<u32>(data.Position.size());
            data.Position.push_back(float3(0.0f, -halfHeight, 0.0f));
            data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
            data.UV.push_back(float2(0.5f, 0.5f));
            data.Normal.push_back(float3(0.0f, -1.0f, 0.0f));

            u32 bottomRingStart = static_cast<u32>(data.Position.size());
            for (u32 i = 0; i <= sectorCount; ++i) {
                f32 sectorAngle = i * sectorStep;
                f32 x           = radius * cosf(sectorAngle);
                f32 z           = radius * sinf(sectorAngle);

                data.Position.push_back(float3(x, -halfHeight, z));
                data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
                data.UV.push_back(float2(0.5f + 0.5f * cosf(sectorAngle), 0.5f - 0.5f * sinf(sectorAngle)));
                data.Normal.push_back(float3(0.0f, -1.0f, 0.0f));
            }

            for (u32 i = 0; i < sectorCount; ++i) {
                indices.push_back(bottomCenterIdx);
                indices.push_back(bottomRingStart + i);
                indices.push_back(bottomRingStart + i + 1);
            }

            data.Tangent.resize(data.Position.size());

            return RZMesh(data, indices);
        }

        RZMesh CreateCapsule(u32 sectorCount = 32, u32 stackCount = 16, f32 radius = 0.5f, f32 height = 1.0f)
        {
            RZVertex data{};

            f32 sectorStep = static_cast<f32>(2 * M_PI / sectorCount);
            f32 stackStep  = static_cast<f32>(M_PI / stackCount);
            f32 halfHeight = height * 0.5f;

            for (u32 i = 0; i <= stackCount; ++i) {
                f32 stackAngle = static_cast<f32>(M_PI / 2 - i * stackStep);    // starting from pi/2 to -pi/2
                f32 xz         = radius * cos(stackAngle);                      // r * cos(u), ring radius at this stack
                f32 y          = radius * sin(stackAngle);                      // r * sin(u), pole (up) component
                f32 yOffset    = (i <= stackCount / 2) ? halfHeight : -halfHeight;

                // add (sectorCount+1) vertices per stack
                // the first and last vertices have same position and normal, but different tex coords
                for (u32 j = 0; j <= sectorCount; ++j) {
                    f32 sectorAngle = j * sectorStep;    // starting from 0 to 2pi

                    f32 x = xz * cosf(sectorAngle);
                    f32 z = xz * sinf(sectorAngle);

                    // vertex tex coord (s, t) range between [0, 1]
                    f32 s = static_cast<f32>(j) / static_cast<f32>(sectorCount);
                    f32 t = static_cast<f32>(i) / static_cast<f32>(stackCount);

                    data.Position.push_back(float3(x, y + yOffset, z));
                    data.Color.push_back(float4(1.0f, 1.0f, 1.0f, 1.0f));
                    data.UV.push_back(float2(s, t));
                    data.Normal.push_back(normalize(float3(x, y, z)));
                }
            }

            RZDynamicArray<u32> indices;
            u16                 k1, k2;
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

            data.Tangent.resize(data.Position.size());

            return RZMesh(data, indices);
        }

#define NUM_MESH_PRIMITIVES (static_cast<size_t>(MeshPrimitive::kCylinder) + 1)

        static RZMesh s_PrimitiveCache[NUM_MESH_PRIMITIVES];
        static bool   s_IsPrimitiveCached[NUM_MESH_PRIMITIVES] = {};

        RZMesh CreatePrimitive(MeshPrimitive primitive)
        {
            size_t idx = static_cast<size_t>(primitive);
            RAZIX_CORE_ASSERT(idx < NUM_MESH_PRIMITIVES, "Invalid mesh primitive type");

            if (s_IsPrimitiveCached[idx])
                return s_PrimitiveCache[idx];

            switch (primitive) {
                case MeshPrimitive::kPlane:
                    s_PrimitiveCache[idx] = CreatePlane();
                    break;
                case MeshPrimitive::kCube:
                    s_PrimitiveCache[idx] = CreateCube();
                    break;
                case MeshPrimitive::kSphere:
                    s_PrimitiveCache[idx] = CreateSphere();
                    break;
                case MeshPrimitive::kPyramid:
                    s_PrimitiveCache[idx] = CreatePyramid();
                    break;
                case MeshPrimitive::kCapsule:
                    s_PrimitiveCache[idx] = CreateCapsule();
                    break;
                case MeshPrimitive::kCylinder:
                    s_PrimitiveCache[idx] = CreateCylinder();
                    break;
                default:
                    RAZIX_CORE_WARN("Unknown mesh primitive type, returning cube mesh by default");
                    s_PrimitiveCache[idx] = CreateCube();
                    break;
            }
            s_IsPrimitiveCached[idx] = true;
            return s_PrimitiveCache[idx];
        }
    }    // namespace Gfx
}    // namespace Razix
