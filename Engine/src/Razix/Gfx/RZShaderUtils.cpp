// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShaderUtils.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include <vulkan/vulkan.h>
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include <d3d12shader.h>
    #include <d3dcompiler.h>
    #include <dxcapi.h>
#endif

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // Vulkan Implementation
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_VULKAN

        void VulkanReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: Implement SPIR-V reflection using spirv-reflect or similar
            RAZIX_UNUSED(stageBlob);
            RAZIX_UNUSED(outReflection);

            // Placeholder for future Vulkan shader reflection implementation
            RAZIX_CORE_WARN("[Vulkan] Shader reflection not yet implemented");
        }

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // DirectX 12 Implementation
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_DIRECTX12

        u32 DX12GetComponentCount(u32 mask)
        {
            u32 count = 0;
            while (mask) {
                count += mask & 1;
                mask >>= 1;
            }
            return count;
        }

        DXGI_FORMAT DX12GetFormatFromComponent(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount)
        {
            switch (componentType) {
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    switch (componentCount) {
                        case 1: return DXGI_FORMAT_R32_FLOAT;
                        case 2: return DXGI_FORMAT_R32G32_FLOAT;
                        case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
                        case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                        default: return DXGI_FORMAT_UNKNOWN;
                    }
                case D3D_REGISTER_COMPONENT_SINT32:
                    switch (componentCount) {
                        case 1: return DXGI_FORMAT_R32_SINT;
                        case 2: return DXGI_FORMAT_R32G32_SINT;
                        case 3: return DXGI_FORMAT_R32G32B32_SINT;
                        case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
                        default: return DXGI_FORMAT_UNKNOWN;
                    }
                case D3D_REGISTER_COMPONENT_UINT32:
                    switch (componentCount) {
                        case 1: return DXGI_FORMAT_R32_UINT;
                        case 2: return DXGI_FORMAT_R32G32_UINT;
                        case 3: return DXGI_FORMAT_R32G32B32_UINT;
                        case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
                        default: return DXGI_FORMAT_UNKNOWN;
                    }
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        }

        u32 DX12GetDxgiFormatSize(DXGI_FORMAT format)
        {
            switch (format) {
                case DXGI_FORMAT_R32_FLOAT: return 4;
                case DXGI_FORMAT_R32G32_FLOAT: return 8;
                case DXGI_FORMAT_R32G32B32_FLOAT: return 12;
                case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
                case DXGI_FORMAT_R32_SINT: return 4;
                case DXGI_FORMAT_R32G32_SINT: return 8;
                case DXGI_FORMAT_R32G32B32_SINT: return 12;
                case DXGI_FORMAT_R32G32B32A32_SINT: return 16;
                case DXGI_FORMAT_R32_UINT: return 4;
                case DXGI_FORMAT_R32G32_UINT: return 8;
                case DXGI_FORMAT_R32G32B32_UINT: return 12;
                case DXGI_FORMAT_R32G32B32A32_UINT: return 16;
                default: return 0;
            }
        }

        rz_gfx_descriptor_type DX12ConvertInputTypeToDescriptorType(D3D_SHADER_INPUT_TYPE inputType)
        {
            switch (inputType) {
                case D3D_SIT_CBUFFER:
                    return RZ_GFX_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                case D3D_SIT_TBUFFER:
                    return RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED;    // TBuffer is legacy; often structured
                case D3D_SIT_TEXTURE:
                    return RZ_GFX_DESCRIPTOR_TYPE_TEXTURE;
                case D3D_SIT_SAMPLER:
                    return RZ_GFX_DESCRIPTOR_TYPE_SAMPLER;
                case D3D_SIT_UAV_RWTYPED:
                    return RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED;
                case D3D_SIT_STRUCTURED:
                    return RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED;
                case D3D_SIT_UAV_RWSTRUCTURED:
                    return RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED;
                case D3D_SIT_BYTEADDRESS:
                    return RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS;
                case D3D_SIT_UAV_RWBYTEADDRESS:
                    return RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS;
                case D3D_SIT_UAV_APPEND_STRUCTURED:
                    return RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED;
                case D3D_SIT_UAV_CONSUME_STRUCTURED:
                    return RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED;
                case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                    return RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER;
                case D3D_SIT_RTACCELERATIONSTRUCTURE:
                    return RZ_GFX_DESCRIPTOR_TYPE_RT_ACCELERATION_STRUCTURE;
                case D3D_SIT_UAV_FEEDBACKTEXTURE:
                    return RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE;    // Use RW_TEXTURE as a fallback
                default:
                    return RZ_GFX_DESCRIPTOR_TYPE_NONE;
            }
        }

        void DX12ReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            IDxcUtils* shaderReflectionUtils = nullptr;
            HRESULT    hr                    = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&shaderReflectionUtils));
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("[DX12] Failed to create DxcUtils instance for shader reflection: {0}", hr);
                return;
            }

            IDxcBlobEncoding* blob = NULL;
            hr                     = shaderReflectionUtils->CreateBlob(stageBlob->bytecode, (u32) stageBlob->size, CP_UTF8, &blob);
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("[DX12] Failed to create DxcBlobEncoding for shader reflection: {0}", hr);
                shaderReflectionUtils->Release();
                return;
            }

            DxcBuffer shaderSourceBuffer = {};
            shaderSourceBuffer.Ptr       = blob->GetBufferPointer();
            shaderSourceBuffer.Size      = blob->GetBufferSize();
            shaderSourceBuffer.Encoding  = 0;

            ID3D12ShaderReflection* shaderReflection = nullptr;
            hr                                       = shaderReflectionUtils->CreateReflection(&shaderSourceBuffer, IID_PPV_ARGS(&shaderReflection));
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("[DX12] Failed to create shader reflection: {0}", hr);
                blob->Release();
                shaderReflectionUtils->Release();
                return;
            }

            // Get the shader desc reflection data
            D3D12_SHADER_DESC shaderDesc = {};
            hr                           = shaderReflection->GetDesc(&shaderDesc);
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("[DX12] Failed to get shader description: {0}", hr);
                shaderReflection->Release();
                blob->Release();
                shaderReflectionUtils->Release();
                return;
            }

            // Reflect input parameters for vertex shaders
            if (stageBlob->stage == RZ_GFX_SHADER_STAGE_VERTEX) {
                outReflection->pInputElements = (rz_gfx_input_element*) Memory::RZMalloc(sizeof(rz_gfx_input_element) * shaderDesc.InputParameters);

                for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
                    D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
                    hr = shaderReflection->GetInputParameterDesc(i, &paramDesc);
                    if (FAILED(hr)) {
                        RAZIX_CORE_ERROR("[DX12] Failed to get input parameter description: {0}", hr);
                        continue;
                    }

                    u32         componentCount = DX12GetComponentCount(paramDesc.Mask);
                    DXGI_FORMAT format         = DX12GetFormatFromComponent(paramDesc.ComponentType, componentCount);
                    u32         formatSize     = DX12GetDxgiFormatSize(format);

                    if (format == DXGI_FORMAT_UNKNOWN || formatSize == 0) {
                        RAZIX_CORE_ERROR("[DX12] Unsupported format or size for input parameter: {0}", paramDesc.SemanticName);
                        continue;
                    }

                    rz_gfx_input_element inputParam = {0};
                    inputParam.pSemanticName        = paramDesc.SemanticName;
                    inputParam.semanticIndex        = paramDesc.SemanticIndex;
                    inputParam.format               = (rz_gfx_format) format;
                    inputParam.inputSlot            = paramDesc.Stream;
                    inputParam.alignedByteOffset    = D3D12_APPEND_ALIGNED_ELEMENT;
                    inputParam.inputClass           = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    inputParam.instanceStepRate     = 0;

                    outReflection->elementCount++;
                    outReflection->pInputElements[i] = inputParam;
                }
            }

            // Reflect descriptor tables and root constants
            rz_gfx_descriptor_table_desc* tableDescs = (rz_gfx_descriptor_table_desc*) Memory::RZMalloc(sizeof(rz_gfx_descriptor_table_desc) * shaderDesc.BoundResources);
            uint32_t                      tableCount = 0;

            // Initialize descriptor tables to invalid state
            for (uint32_t i = 0; i < shaderDesc.BoundResources; i++) {
                tableDescs[i].tableIndex = 0xffffffff;
            }

            for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc = {0};
                hr                                    = shaderReflection->GetResourceBindingDesc(i, &bindDesc);
                if (FAILED(hr)) {
                    RAZIX_CORE_ERROR("[DX12] Failed to get resource binding desc at index {0}", i);
                    continue;
                }

                // Check for push constant
                if (strstr(bindDesc.Name, RAZIX_PUSH_CONSTANT_REFLECTION_NAME_DX12) != NULL) {
                    // Push Constant Block
                    rz_gfx_root_constant_desc* rootConstants =
                        (rz_gfx_root_constant_desc*) Memory::RZRealloc(outReflection->rootSignatureDesc.pRootConstantsDesc,
                            sizeof(rz_gfx_root_constant_desc) * (outReflection->rootSignatureDesc.rootConstantCount + 1));
                    outReflection->rootSignatureDesc.pRootConstantsDesc = rootConstants;

                    rz_gfx_root_constant_desc* rc = &outReflection->rootSignatureDesc.pRootConstantsDesc[outReflection->rootSignatureDesc.rootConstantCount++];
                    rc->location.binding          = bindDesc.BindPoint;
                    rc->location.space            = bindDesc.Space;
                    rc->sizeInBytes               = bindDesc.BindCount * 16;    // Assumption: HLSL constant buffer is 16 bytes per register
                    rc->offsetInBytes             = 0;
                    rc->shaderStage               = stageBlob->stage;

                    continue;
                }

                // Regular Descriptor
                // Try to find an existing table for this space
                rz_gfx_descriptor_table_desc* table = NULL;
                for (uint32_t t = 0; t < tableCount; ++t) {
                    if (tableDescs[t].tableIndex == bindDesc.Space) {
                        table = &tableDescs[t];
                        break;
                    }
                }

                // If not found, create a new descriptor table for this register space
                if (!table) {
                    table                  = &tableDescs[bindDesc.Space];
                    table->tableIndex      = bindDesc.Space;
                    table->descriptorCount = 0;
                    table->pDescriptors    = NULL;
                    tableCount++;
                }

                // Add descriptor to the table
                rz_gfx_descriptor* descriptors =
                    (rz_gfx_descriptor*) Memory::RZRealloc((void*) table->pDescriptors, sizeof(rz_gfx_descriptor) * (table->descriptorCount + 1));
                table->pDescriptors = descriptors;

                rz_gfx_descriptor* desc = &table->pDescriptors[table->descriptorCount++];
                memset(desc, 0, sizeof(rz_gfx_descriptor));
                desc->pName            = bindDesc.Name;
                desc->location.binding = bindDesc.BindPoint;
                desc->location.space   = bindDesc.Space;
                desc->type             = DX12ConvertInputTypeToDescriptorType(bindDesc.Type);
                desc->arraySize        = bindDesc.BindCount;
                desc->sizeInBytes      = 0;
                desc->offsetInBytes    = 0;
                desc->memberCount      = 1;
            }

            // Store the descriptor tables
            outReflection->rootSignatureDesc.descriptorTableCount  = tableCount;
            outReflection->rootSignatureDesc.pDescriptorTablesDesc = tableDescs;

            // Cleanup
            shaderReflection->Release();
            blob->Release();
            shaderReflectionUtils->Release();
        }

#endif    // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Common Implementation
        //-----------------------------------------------------------------------------------

        ReflectShaderBlobBackendFn GetShaderReflectionFunction()
        {
            switch (rzGfxCtx_GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case RZ_RENDER_API_VULKAN:
                    return VulkanReflectShaderBlob;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case RZ_RENDER_API_D3D12:
                    return DX12ReflectShaderBlob;
#endif
                default:
                    RAZIX_CORE_ERROR("[ShaderUtils] Unsupported Render API for shader reflection: {0}", rzGfxCtx_GetRenderAPIString());
                    return nullptr;
            }
        }

        void FreeShaderReflectionMemAllocs(rz_gfx_shader_reflection* reflection)
        {
            if (reflection->pInputElements) {
                Memory::RZFree(reflection->pInputElements);
                reflection->pInputElements = NULL;
            }
            if (reflection->rootSignatureDesc.pDescriptorTablesDesc) {
                for (uint32_t i = 0; i < reflection->rootSignatureDesc.descriptorTableCount; ++i) {
                    rz_gfx_descriptor_table_desc* tableDesc = &reflection->rootSignatureDesc.pDescriptorTablesDesc[i];
                    if (tableDesc->pDescriptors) {
                        Memory::RZFree(tableDesc->pDescriptors);
                        tableDesc->pDescriptors = NULL;
                    }
                    // I don't think we want to free the resource views here because they are managed by the descriptor tables
                }

                if (reflection->rootSignatureDesc.pRootConstantsDesc) {
                    Memory::RZFree(reflection->rootSignatureDesc.pRootConstantsDesc);
                    reflection->rootSignatureDesc.pRootConstantsDesc = NULL;
                }

                Memory::RZFree(reflection->rootSignatureDesc.pDescriptorTablesDesc);
                reflection->rootSignatureDesc.pDescriptorTablesDesc = NULL;
            }
            reflection->elementCount = 0;
        }

        void CopyReflectedRootSigDesc(const rz_gfx_shader_reflection* src, rz_gfx_root_signature_desc* dst)
        {
            if (!src || !dst) return;

            // Do a deep copy of the descriptor tables and root constants
            if (src->rootSignatureDesc.pDescriptorTablesDesc) {
                dst->pDescriptorTablesDesc = (rz_gfx_descriptor_table_desc*) Memory::RZMalloc(
                    sizeof(rz_gfx_descriptor_table_desc) * src->rootSignatureDesc.descriptorTableCount);
                memcpy(dst->pDescriptorTablesDesc, src->rootSignatureDesc.pDescriptorTablesDesc, sizeof(rz_gfx_descriptor_table_desc) * src->rootSignatureDesc.descriptorTableCount);

                // for each descriptor table, copy the descriptors
                for (uint32_t i = 0; i < src->rootSignatureDesc.descriptorTableCount; ++i) {
                    rz_gfx_descriptor_table_desc* srcTable = &src->rootSignatureDesc.pDescriptorTablesDesc[i];
                    rz_gfx_descriptor_table_desc* dstTable = &dst->pDescriptorTablesDesc[i];
                    if (srcTable->pDescriptors) {
                        dstTable->pDescriptors = (rz_gfx_descriptor*) Memory::RZMalloc(
                            sizeof(rz_gfx_descriptor) * srcTable->descriptorCount);
                        memcpy(dstTable->pDescriptors, srcTable->pDescriptors, sizeof(rz_gfx_descriptor) * srcTable->descriptorCount);
                    } else {
                        dstTable->pDescriptors = NULL;
                    }
                }

            } else {
                dst->pDescriptorTablesDesc = NULL;
            }
            if (src->rootSignatureDesc.pRootConstantsDesc) {
                dst->pRootConstantsDesc = (rz_gfx_root_constant_desc*) Memory::RZMalloc(
                    sizeof(rz_gfx_root_constant_desc) * src->rootSignatureDesc.rootConstantCount);
                memcpy(dst->pRootConstantsDesc, src->rootSignatureDesc.pRootConstantsDesc, sizeof(rz_gfx_root_constant_desc) * src->rootSignatureDesc.rootConstantCount);
            } else {
                dst->pRootConstantsDesc = NULL;
            }

            // Copy other relevant data
            dst->descriptorTableCount = src->rootSignatureDesc.descriptorTableCount;
            dst->rootConstantCount    = src->rootSignatureDesc.rootConstantCount;
        }

    }    // namespace Gfx
}    // namespace Razix