#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Gfx/RHI/RHI.h"

// StructNames + Assertion [Source] : https://twitter.com/SebAaltonen/status/1597135035811106816

namespace Razix {
    namespace Gfx {

#define RAZIX_MAX_SHADER_SOURCE_SIZE 1024 * 1024
#define RAZIX_MAX_LINE_LENGTH        1024
#define RAZIX_MAX_SHADER_STAGES      RZ_GFX_SHADER_STAGE_COUNT
#define RAZIX_MAX_INCLUDE_DEPTH      16

        // Engine shaders
        enum class ShaderBuiltin : u32
        {
            //------------------------------
            Default,
            Skybox,
            ProceduralSkybox,
            GBuffer,
            PBRDeferredLighting,
            //----------------
            VisibilityBufferFill,
            //----------------
            DeferredDecals,
            Composition,
            DepthPreTest,
            CSM,
            EnvToCubemap,
            GenerateIrradianceMap,
            GeneratePreFilteredMap,
            Sprite,
            SpriteTextured,
            DebugPoint,
            DebugLine,
            ImGui,
            // Post Processing FX
            SSAO,
            GaussianBlur,
            ColorGrading,
            TAAResolve,
            FXAA,
            Tonemap,
            //---
            COUNT
        };

        rz_gfx_shader_desc       ParseRZSF(const std::string& filePath);
        void                     FreeRZSFBytecodeAlloc(rz_gfx_shader* shader);
        rz_gfx_shader_reflection ReflectShader(const rz_gfx_shader* shader);

        // Single time command buffer
        rz_gfx_cmdbuf_handle BeginSingleTimeCommandBuffer(const std::string& name, float4 color);
        void                 EndSingleTimeCommandBuffer(rz_gfx_cmdbuf_handle cmdBuf);

        //-----------------------------------------------------------------------------------
        // High-Level Rendering helper structs
        // [Source]: https://github.com/skaarj1989/SupernovaEngine

        /**
         * Drawable is a conversion of an actor/entity that will be rendered onto the scene
         */
        // struct Drawable
        // {
        //     RZMaterialHandle material;
        //     RZMeshHandle     mesh;
        //     uint32_t         transformID;
        // };

        //using Drawables = std::vector<Drawable>;

        /**
         * Drawables are batched by their common VB/IB and material pools
         */
        //  struct Batch
        //  {
        //      RZVertexBufferHandle vertexBuffer;
        //      RZIndexBufferHandle  indexBuffer;
        //      RZPipelineHandle     pso;
        //  };

        //  using Batches = std::vector<Batch>;

        /**
         * DrawData is use for Bindless Rendering 
         */
        // struct DrawData
        // {
        //     u32 drawBatchIdx  = 0;
        //     u32 drawableIdx   = 0;
        //     u32 vertexCount   = 0;
        //     u32 vertexOffset  = 0;
        //     u32 indexCount    = 0;
        //     u32 indexOffset   = 0;
        //     u32 instanceCount = 0;
        //     u32 _padding      = 0;
        // };

        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
