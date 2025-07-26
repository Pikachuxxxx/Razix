#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // Shader Reflection Function Pointer Types
        //-----------------------------------------------------------------------------------
        
        typedef void (*ReflectShaderBlobBackendFn)(const rz_gfx_shader_stage_blob*, rz_gfx_shader_reflection*);

        //-----------------------------------------------------------------------------------
        // Vulkan Shader Reflection
        //-----------------------------------------------------------------------------------
        
        #ifdef RAZIX_RENDER_API_VULKAN
        
        /**
         * Reflect Vulkan SPIR-V shader blob
         * @param stageBlob Shader stage blob to reflect
         * @param outReflection Output reflection data
         */
        void VulkanReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection);
        
        #endif // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // DirectX 12 Shader Reflection
        //-----------------------------------------------------------------------------------
        
        #ifdef RAZIX_RENDER_API_DIRECTX12

        /**
         * Get component count from mask (for DirectX shader reflection)
         * @param mask Component mask in binary form
         * @return Number of components
         */
        u32 DX12GetComponentCount(u32 mask);

        /**
         * Convert DirectX component type and count to DXGI format
         * @param componentType D3D register component type
         * @param componentCount Number of components
         * @return DXGI format enum value
         */
        DXGI_FORMAT DX12GetFormatFromComponent(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount);

        /**
         * Get size in bytes for DXGI format
         * @param format DXGI format
         * @return Size in bytes
         */
        u32 DX12GetDxgiFormatSize(DXGI_FORMAT format);

        /**
         * Convert DirectX shader input type to engine descriptor type
         * @param inputType D3D shader input type
         * @return Engine descriptor type
         */
        rz_gfx_descriptor_type DX12ConvertInputTypeToDescriptorType(D3D_SHADER_INPUT_TYPE inputType);

        /**
         * Reflect DirectX 12 shader blob using DXC
         * @param stageBlob Shader stage blob to reflect
         * @param outReflection Output reflection data
         */
        void DX12ReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection);
        
        #endif // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Common Shader Utilities
        //-----------------------------------------------------------------------------------

        /**
         * Get appropriate reflection function for current render API
         * @return Function pointer for shader reflection
         */
        ReflectShaderBlobBackendFn GetShaderReflectionFunction();

    } // namespace Gfx
} // namespace Razix