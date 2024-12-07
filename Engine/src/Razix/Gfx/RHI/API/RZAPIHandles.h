#pragma once

#include "Razix/Core/RZHandle.h"

namespace Razix {
    namespace Gfx {

        // Forward declaration
        class RZTexture;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZStorageBuffer;
        class RZUniformBuffer;
        class RZPipeline;
        class RZShader;
        class RZDrawCommandBuffer;
        class RZCommandPool;
        class RZDescriptorSet;

        class RZMesh;
        class RZMaterial;
        struct RZDecal;

        // Handles for Engine API types (Graphics)
        using RZTextureHandle           = RZHandle<Gfx::RZTexture>;
        using RZVertexBufferHandle      = RZHandle<Gfx::RZVertexBuffer>;
        using RZIndexBufferHandle       = RZHandle<Gfx::RZIndexBuffer>;
        using RZStorageBufferHandle     = RZHandle<Gfx::RZStorageBuffer>;
        using RZUniformBufferHandle     = RZHandle<Gfx::RZUniformBuffer>;
        using RZPipelineHandle          = RZHandle<Gfx::RZPipeline>;
        using RZShaderHandle            = RZHandle<Gfx::RZShader>;
        using RZDrawCommandBufferHandle = RZHandle<Gfx::RZDrawCommandBuffer>;
        using RZCommandPoolHandle       = RZHandle<Gfx::RZCommandPool>;
        using RZDescriptorSetHandle     = RZHandle<Gfx::RZDescriptorSet>;
        // High-level graphics primitives Handles
        using RZMeshHandle     = RZHandle<Gfx::RZMesh>;
        using RZMaterialHandle = RZHandle<Gfx::RZMaterial>;
        using RZDecalHandle    = RZHandle<Gfx::RZDecal>;
        // Null Handle
        using RZNullHandle = RZHandle<void>;

    }    // namespace Gfx
}    // namespace Razix