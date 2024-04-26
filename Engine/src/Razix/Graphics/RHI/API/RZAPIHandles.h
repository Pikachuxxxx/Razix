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
        class RZMaterial;
        class RZCommandBuffer;
        class RZDescriptorSet;
        class RZMesh;

        // Handles for Engine API types (Graphics)
        using RZTextureHandle       = RZHandle<Graphics::RZTexture>;
        using RZVertexBufferHandle  = RZHandle<Graphics::RZVertexBuffer>;
        using RZIndexBufferHandle   = RZHandle<Graphics::RZIndexBuffer>;
        using RZStorageBufferHandle = RZHandle<Graphics::RZStorageBuffer>;
        using RZUniformBufferHandle = RZHandle<Graphics::RZUniformBuffer>;
        using RZPipelineHandle      = RZHandle<Graphics::RZPipeline>;
        using RZShaderHandle        = RZHandle<Graphics::RZShader>;
        using RZCommandBufferHandle = RZHandle<Graphics::RZCommandBuffer>;
        using RZDescriptorSetHandle = RZHandle<Graphics::RZDescriptorSet>;

        using RZMaterialHandle = RZHandle<Graphics::RZMaterial>;
        using RZMeshHandle     = RZHandle<Graphics::RZMesh>;

        using RZNullHandle = RZHandle<void>;

    }    // namespace Graphics
}    // namespace Razix