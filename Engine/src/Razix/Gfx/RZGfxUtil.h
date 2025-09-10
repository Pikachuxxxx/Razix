#pragma once

#include "Razix/Core/RZHandle.h"
#include "Razix/Gfx/RHI/RHI.h"
#include "Razix/Gfx/RZGfxDebug.h"
#include "Razix/Gfx/RZShaderUtils.h"

#define RAZIX_INIT_RENDERER_MINIMAL 0    // Initializes the renderer with minimal features (no post-processing, no shadows, no IBL, no skybox, etc.)

// Constants
#define RAZIX_MAX_SHADER_SOURCE_SIZE 1024 * 1024
#define RAZIX_MAX_LINE_LENGTH        1024
#define RAZIX_MAX_SHADER_STAGES      RZ_GFX_SHADER_STAGE_COUNT
#define RAZIX_MAX_INCLUDE_DEPTH      16

namespace Razix {
    namespace Gfx {

        // Engine shaders enumeration
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

        //-----------------------------------------------------------------------------------
        // Core Shader Utilities
        //-----------------------------------------------------------------------------------

        RAZIX_API rz_gfx_shader_desc       ParseRZSF(const std::string& filePath);
        RAZIX_API void                     FreeRZSFBytecodeAlloc(rz_gfx_shader* shader);
        RAZIX_API rz_gfx_shader_reflection ReflectShader(const rz_gfx_shader* shader);

        //-----------------------------------------------------------------------------------
        // Command Buffer Utilities
        //-----------------------------------------------------------------------------------

        RAZIX_API rz_gfx_cmdbuf_handle BeginSingleTimeCommandBuffer(const std::string& name, float4 color);
        RAZIX_API void                 EndSingleTimeCommandBuffer(rz_gfx_cmdbuf_handle cmdBuf);

        //-----------------------------------------------------------------------------------
        // Texture Utilities (Create/Streaming/Mips/Compression/Decompression etc.)
        //-----------------------------------------------------------------------------------
        RAZIX_API rz_gfx_texture_handle CreateTextureFromFile(const std::string& filePath, bool floatingPoint = false);

        //-----------------------------------------------------------------------------------
        // High-Level Rendering Helper Structs (Future Use)
        //-----------------------------------------------------------------------------------
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
