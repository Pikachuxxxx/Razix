// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMeshFactory.h"

#include "Razix/Gfx/RZMesh.h"

#if WIP_REFACTOR

//#include "Razix/Gfx/RZShaderLibrary.h"
//
//#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
//#include "Razix/Gfx/RHI/API/RZTexture.h"
//#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"
//
//#include "Razix/Gfx/Materials/RZMaterial.h"

    #define _USE_MATH_DEFINES
    #include <math.h>

namespace Razix {
    namespace Gfx {

        static RZMesh* CreatePlane(f32 width = 10.0f, f32 height = 10.0f, const float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f));
        static RZMesh* CreateCube();
        static RZMesh* CreatePyramid();
        static RZMesh* CreateSphere(u32 xSegments = 64, u32 ySegments = 64);
        static RZMesh* CreateIcoSphere(u32 radius = 1, u32 subdivision = 64);
        static RZMesh* CreateCapsule(f32 radius = 1.0f, f32 midHeight = 1.0f, int radialSegments = 64, int rings = 8);
        static RZMesh* CreateCylinder(f32 bottomRadius = 1.0f, f32 topRadius = 1.0f, f32 height = 1.0f, int radialSegments = 64, int rings = 8);

        // Materials are not duplicating when we have single instance
        //static RZMesh* planeMesh  = nullptr;
        //static RZMesh* cubeMesh   = nullptr;
        //static RZMesh* sphereMesh = nullptr;

        RZMesh* CreatePrimitive(MeshPrimitive primitive)
        {
            switch (primitive) {
                case MeshPrimitive::Plane: {
                    return CreatePlane();
                }
                case MeshPrimitive::Cube: {
                    return CreateCube();
                }
                case MeshPrimitive::Sphere: {
                    return CreateSphere();
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
                    return CreateCube();
                }
            }
            return nullptr;
        }

    #define NUM_QUAD_VERTS 4
    #define NUM_CUBE_VERTS 24

        RZMesh* CreatePlane(f32 width, f32 height, const float4 color /*= float4(1.0f, 1.0f, 1.0f, 1.0f)*/)
        {
            RZVertex data{};
            data.Position.resize(NUM_QUAD_VERTS);
            data.Color.resize(NUM_QUAD_VERTS);
            data.UV.resize(NUM_QUAD_VERTS);
            data.Normal.resize(NUM_QUAD_VERTS);

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

            u32 indices[6]{
                0, 1, 2, 2, 3, 0};

            RZMesh* mesh = new RZMesh(data, indices, 6);

            auto        shader          = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GBuffer);
            RZMaterial* gbufferMaterial = new RZMaterial(shader);
            gbufferMaterial->createDescriptorSet();
            mesh->setMaterial(gbufferMaterial);

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

            u32 indices[36]{
                0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

            RZMesh* mesh = new RZMesh(data, indices, 36);

            auto        shader          = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GBuffer);
            RZMaterial* gbufferMaterial = new RZMaterial(shader);
            gbufferMaterial->createDescriptorSet();
            mesh->setMaterial(gbufferMaterial);

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

                    data.Position.push_back(float3(x, y, z));
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

            RZMesh* mesh = new RZMesh(data, indices);

            auto        shader          = Gfx::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GBuffer);
            RZMaterial* gbufferMaterial = new RZMaterial(shader);
            gbufferMaterial->createDescriptorSet();
            mesh->setMaterial(gbufferMaterial);

            return mesh;
        }
    }    // namespace Gfx
}    // namespace Razix

#endif    // WIP_REFACTOR