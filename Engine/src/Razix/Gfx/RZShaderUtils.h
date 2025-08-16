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

        void VulkanReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection);

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // DirectX 12 Shader Reflection
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_DIRECTX12

        u32                    DX12GetComponentCount(u32 mask);
        DXGI_FORMAT            DX12GetFormatFromComponent(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount);
        u32                    DX12GetDxgiFormatSize(DXGI_FORMAT format);
        rz_gfx_descriptor_type DX12ConvertInputTypeToDescriptorType(D3D_SHADER_INPUT_TYPE inputType);
        void                   DX12ReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection);

#endif    // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Common Shader Utilities
        //-----------------------------------------------------------------------------------

        ReflectShaderBlobBackendFn GetShaderReflectionFunction();
        void                       FreeShaderReflectionMemAllocs(rz_gfx_shader_reflection* reflection);
        void                       CopyReflectedRootSigDesc(const rz_gfx_shader_reflection* src, rz_gfx_root_signature_desc* dst);

    }    // namespace Gfx
}    // namespace Razix