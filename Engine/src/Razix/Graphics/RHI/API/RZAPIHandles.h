#pragma once

#include "Razix/Core/RZHandle.h"

namespace Razix {
    namespace Graphics {

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
        using RZTextureHandle           = RZHandle<Graphics::RZTexture>;
        using RZVertexBufferHandle      = RZHandle<Graphics::RZVertexBuffer>;
        using RZIndexBufferHandle       = RZHandle<Graphics::RZIndexBuffer>;
        using RZStorageBufferHandle     = RZHandle<Graphics::RZStorageBuffer>;
        using RZUniformBufferHandle     = RZHandle<Graphics::RZUniformBuffer>;
        using RZPipelineHandle          = RZHandle<Graphics::RZPipeline>;
        using RZShaderHandle            = RZHandle<Graphics::RZShader>;
        using RZDrawCommandBufferHandle = RZHandle<Graphics::RZDrawCommandBuffer>;
        using RZCommandPoolHandle       = RZHandle<Graphics::RZCommandPool>;
        using RZDescriptorSetHandle     = RZHandle<Graphics::RZDescriptorSet>;
        // High-level graphics primitives Handles
        using RZMeshHandle     = RZHandle<Graphics::RZMesh>;
        using RZMaterialHandle = RZHandle<Graphics::RZMaterial>;
        using RZDecalHandle    = RZHandle<Graphics::RZDecal>;
        // Null Handle
        using RZNullHandle = RZHandle<void>;

    }    // namespace Graphics
}    // namespace Razix