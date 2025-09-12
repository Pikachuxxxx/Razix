// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShaderUtils.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/Gfx/RZGfxUtil.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include <volk.h>
    #include <spirv_reflect.h>
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

            SpvReflectShaderModule module;
            SpvReflectResult       result = spvReflectCreateShaderModule(stageBlob->size, stageBlob->bytecode, &module);
            RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV shader! check previous logs for shader name");
            (void)result;  // Suppress unused variable warning in release builds
#if 0
            // Reflect input parameters for vertex shaders
            if (stageBlob->stage == RZ_GFX_SHADER_STAGE_VERTEX) {
                // Calculate new total count
                u32 newElementCount = outReflection->elementCount + shaderDesc.InputParameters;

                // Reallocate to accommodate new elements
                outReflection->pInputElements = (rz_gfx_input_element*) Memory::RZRealloc(
                    outReflection->pInputElements,
                    sizeof(rz_gfx_input_element) * newElementCount);

                u32 currentElementIndex = outReflection->elementCount;
                u32 currentOffset       = 0;
                for (sz i = 0; i < module.input_variable_count; i++) {
                SpvReflectInterfaceVariable inputVar = *module.input_variables[i];

                if (inputVar.semantic == NULL && inputVar.name == NULL)
                    break;

                if (inputVar.semantic && std::string(inputVar.semantic) == "SV_VertexID")
                    break;

                if (inputVar.name && (std::string(inputVar.name) == "gl_VertexIndex" || std::string(inputVar.name) == "vs_out"))
                    break;                    

                    rz_gfx_input_element* inputParam = &outReflection->pInputElements[currentElementIndex];
                    inputParam->pSemanticName        = strdup(paramDesc.SemanticName);
                    inputParam->semanticIndex        = paramDesc.SemanticIndex;
                    inputParam->format               = DX12DXXGIToEngineFormat(format);
                    inputParam->inputSlot            = paramDesc.Stream;
                    inputParam->alignedByteOffset    = currentOffset;
                    inputParam->inputClass           = (rz_gfx_input_class) D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    inputParam->instanceStepRate     = 0;

                    currentOffset += formatSize;
                    currentElementIndex++;
                    outReflection->elementCount++;
                }
            }
#endif
            // Placeholder for future Vulkan shader reflection implementation
            RAZIX_CORE_ERROR("[Vulkan] Shader reflection not yet implemented");
            RAZIX_UNIMPLEMENTED_METHOD("[Vulkan] Shader reflection not yet implemented");
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
                    return RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
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

        rz_gfx_format DX12DXXGIToEngineFormat(DXGI_FORMAT format)
        {
            switch (format) {
                // Undefined
                case DXGI_FORMAT_UNKNOWN:
                    return RZ_GFX_FORMAT_UNDEFINED;

                // 8-bit formats
                case DXGI_FORMAT_R8_UNORM:
                    return RZ_GFX_FORMAT_R8_UNORM;
                case DXGI_FORMAT_R8_UINT:
                    return RZ_GFX_FORMAT_R8_UINT;
                case DXGI_FORMAT_R8G8_UNORM:
                    return RZ_GFX_FORMAT_R8G8_UNORM;
                case DXGI_FORMAT_R8G8B8A8_UNORM:
                    return RZ_GFX_FORMAT_R8G8B8A8_UNORM;
                case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                    return RZ_GFX_FORMAT_R8G8B8A8_SRGB;
                case DXGI_FORMAT_B8G8R8A8_UNORM:
                    return RZ_GFX_FORMAT_B8G8R8A8_UNORM;
                case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                    return RZ_GFX_FORMAT_B8G8R8A8_SRGB;

                // 16-bit formats
                case DXGI_FORMAT_R16_UNORM:
                    return RZ_GFX_FORMAT_R16_UNORM;
                case DXGI_FORMAT_R16_FLOAT:
                    return RZ_GFX_FORMAT_R16_FLOAT;
                case DXGI_FORMAT_R16G16_UNORM:
                    return RZ_GFX_FORMAT_R16G16_UNORM;
                case DXGI_FORMAT_R16G16_FLOAT:
                    return RZ_GFX_FORMAT_R16G16_FLOAT;
                case DXGI_FORMAT_R16G16B16A16_UNORM:
                    return RZ_GFX_FORMAT_R16G16B16A16_UNORM;
                case DXGI_FORMAT_R16G16B16A16_FLOAT:
                    return RZ_GFX_FORMAT_R16G16B16A16_FLOAT;

                // 32-bit signed integer formats
                case DXGI_FORMAT_R32_SINT:
                    return RZ_GFX_FORMAT_R32_SINT;
                case DXGI_FORMAT_R32G32_SINT:
                    return RZ_GFX_FORMAT_R32G32_SINT;
                case DXGI_FORMAT_R32G32B32_SINT:
                    return RZ_GFX_FORMAT_R32G32B32_SINT;
                case DXGI_FORMAT_R32G32B32A32_SINT:
                    return RZ_GFX_FORMAT_R32G32B32A32_SINT;

                // 32-bit unsigned integer formats
                case DXGI_FORMAT_R32_UINT:
                    return RZ_GFX_FORMAT_R32_UINT;
                case DXGI_FORMAT_R32G32_UINT:
                    return RZ_GFX_FORMAT_R32G32_UINT;
                case DXGI_FORMAT_R32G32B32_UINT:
                    return RZ_GFX_FORMAT_R32G32B32_UINT;
                case DXGI_FORMAT_R32G32B32A32_UINT:
                    return RZ_GFX_FORMAT_R32G32B32A32_UINT;

                // 32-bit float formats
                case DXGI_FORMAT_R32_FLOAT:
                    return RZ_GFX_FORMAT_R32_FLOAT;
                case DXGI_FORMAT_R32G32_FLOAT:
                    return RZ_GFX_FORMAT_R32G32_FLOAT;
                case DXGI_FORMAT_R32G32B32_FLOAT:
                    return RZ_GFX_FORMAT_R32G32B32_FLOAT;
                case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    return RZ_GFX_FORMAT_R32G32B32A32_FLOAT;

                // Packed formats
                case DXGI_FORMAT_R11G11B10_FLOAT:
                    return RZ_GFX_FORMAT_R11G11B10_FLOAT;

                // Depth-stencil formats
                case DXGI_FORMAT_D16_UNORM:
                    return RZ_GFX_FORMAT_D16_UNORM;
                case DXGI_FORMAT_D24_UNORM_S8_UINT:
                    return RZ_GFX_FORMAT_D24_UNORM_S8_UINT;
                case DXGI_FORMAT_D32_FLOAT:
                    return RZ_GFX_FORMAT_D32_FLOAT;
                case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
                    return RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT;

                // Block compression formats
                case DXGI_FORMAT_BC1_UNORM:
                    return RZ_GFX_FORMAT_BC1_RGBA_UNORM;
                case DXGI_FORMAT_BC3_UNORM:
                    return RZ_GFX_FORMAT_BC3_RGBA_UNORM;
                case DXGI_FORMAT_BC6H_UF16:
                    return RZ_GFX_FORMAT_BC6_UNORM;
                case DXGI_FORMAT_BC7_UNORM:
                    return RZ_GFX_FORMAT_BC7_UNORM;
                case DXGI_FORMAT_BC7_UNORM_SRGB:
                    return RZ_GFX_FORMAT_BC7_SRGB;

                // Unsupported or unmapped formats
                default:
                    RAZIX_CORE_WARN("[DX12] Unsupported DXGI format conversion: {0}", (u32) format);
                    return RZ_GFX_FORMAT_UNDEFINED;
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
                // Calculate new total count
                u32 newElementCount = outReflection->elementCount + shaderDesc.InputParameters;

                // Reallocate to accommodate new elements
                outReflection->pInputElements = (rz_gfx_input_element*) Memory::RZRealloc(
                    outReflection->pInputElements,
                    sizeof(rz_gfx_input_element) * newElementCount);

                u32 currentElementIndex = outReflection->elementCount;
                u32 currentOffset       = 0;
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

                    if (!strcmp(paramDesc.SemanticName, "SV_INSTANCEID") || !strcmp(paramDesc.SemanticName, "SV_VERTEXID"))
                        continue;    // Skip system values

                    rz_gfx_input_element* inputParam = &outReflection->pInputElements[currentElementIndex];
                    inputParam->pSemanticName        = strdup(paramDesc.SemanticName);
                    inputParam->semanticIndex        = paramDesc.SemanticIndex;
                    inputParam->format               = DX12DXXGIToEngineFormat(format);
                    inputParam->inputSlot            = paramDesc.Stream;
                    inputParam->alignedByteOffset    = currentOffset;
                    inputParam->inputClass           = (rz_gfx_input_class) D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    inputParam->instanceStepRate     = 0;

                    currentOffset += formatSize;
                    currentElementIndex++;
                    outReflection->elementCount++;
                }
            }

            // Process bound resources and merge with existing descriptor tables
            for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc = {0};
                hr                                    = shaderReflection->GetResourceBindingDesc(i, &bindDesc);
                if (FAILED(hr)) {
                    RAZIX_CORE_ERROR("[DX12] Failed to get resource binding desc at index {0}", i);
                    continue;
                }

                // Check for push constant
                if (strstr(bindDesc.Name, RAZIX_PUSH_CONSTANT_REFLECTION_NAME_DX12) != NULL) {
                    // Append to existing root constants
                    outReflection->rootSignatureDesc.pRootConstantsDesc =
                        (rz_gfx_root_constant_desc*) Memory::RZRealloc(
                            outReflection->rootSignatureDesc.pRootConstantsDesc,
                            sizeof(rz_gfx_root_constant_desc) * (outReflection->rootSignatureDesc.rootConstantCount + 1));

                    rz_gfx_root_constant_desc* rc = &outReflection->rootSignatureDesc.pRootConstantsDesc[outReflection->rootSignatureDesc.rootConstantCount];
                    rc->location.binding          = bindDesc.BindPoint;
                    rc->location.space            = bindDesc.Space;
                    rc->sizeInBytes               = bindDesc.BindCount * 16;    // Assumption: HLSL constant buffer is 16 bytes per register
                    rc->offsetInBytes             = 0;
                    rc->shaderStage               = stageBlob->stage;

                    outReflection->rootSignatureDesc.rootConstantCount++;
                    continue;
                }

                // Find existing table or create new one
                rz_gfx_descriptor_table_layout* targetTable      = nullptr;
                u32                             targetTableIndex = 0;

                // Search for existing table with matching space
                for (u32 t = 0; t < outReflection->rootSignatureDesc.descriptorTableLayoutsCount; ++t) {
                    if (outReflection->rootSignatureDesc.pDescriptorTableLayouts[t].tableIndex == bindDesc.Space) {
                        targetTable      = &outReflection->rootSignatureDesc.pDescriptorTableLayouts[t];
                        targetTableIndex = t;
                        break;
                    }
                }

                // If no existing table found, create a new one
                if (!targetTable) {
                    // Reallocate table layouts array
                    outReflection->rootSignatureDesc.pDescriptorTableLayouts =
                        (rz_gfx_descriptor_table_layout*) Memory::RZRealloc(
                            outReflection->rootSignatureDesc.pDescriptorTableLayouts,
                            sizeof(rz_gfx_descriptor_table_layout) * (outReflection->rootSignatureDesc.descriptorTableLayoutsCount + 1));

                    targetTableIndex = outReflection->rootSignatureDesc.descriptorTableLayoutsCount;
                    targetTable      = &outReflection->rootSignatureDesc.pDescriptorTableLayouts[targetTableIndex];

                    // Initialize new table
                    targetTable->tableIndex      = bindDesc.Space;
                    targetTable->descriptorCount = 0;
                    targetTable->pDescriptors    = nullptr;

                    outReflection->rootSignatureDesc.descriptorTableLayoutsCount++;
                }

                // Add descriptor to the table (append to existing descriptors)
                targetTable->pDescriptors = (rz_gfx_descriptor*) Memory::RZRealloc(
                    (void*) targetTable->pDescriptors,
                    sizeof(rz_gfx_descriptor) * (targetTable->descriptorCount + 1));

                rz_gfx_descriptor* desc = &targetTable->pDescriptors[targetTable->descriptorCount];
                memset(desc, 0, sizeof(rz_gfx_descriptor));
                desc->pName            = strdup(bindDesc.Name);
                desc->location.binding = bindDesc.BindPoint;
                desc->location.space   = bindDesc.Space;
                desc->type             = DX12ConvertInputTypeToDescriptorType(bindDesc.Type);
                desc->arraySize        = bindDesc.BindCount;
                desc->sizeInBytes      = 0;
                desc->offsetInBytes    = 0;
                desc->memberCount      = 1;

                targetTable->descriptorCount++;
            }

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
                for (u32 i = 0; i < reflection->elementCount; ++i) {
                    if (reflection->pInputElements[i].pSemanticName)
                        free((void*) reflection->pInputElements[i].pSemanticName);
                }
                Memory::RZFree(reflection->pInputElements);
                reflection->pInputElements = NULL;
            }
            if (reflection->rootSignatureDesc.pDescriptorTableLayouts) {
                for (u32 i = 0; i < reflection->rootSignatureDesc.descriptorTableLayoutsCount; ++i) {
                    rz_gfx_descriptor_table_layout* tableLayout = &reflection->rootSignatureDesc.pDescriptorTableLayouts[i];
                    if (tableLayout->pDescriptors) {
                        for (u32 j = 0; j < tableLayout->descriptorCount; ++j) {
                            if (tableLayout->pDescriptors[j].pName)
                                free((void*) tableLayout->pDescriptors[j].pName);
                        }
                        Memory::RZFree(tableLayout->pDescriptors);
                        tableLayout->pDescriptors = NULL;
                    }
                    // I don't think we want to free the resource views here because they are managed by the descriptor tables
                }

                if (reflection->rootSignatureDesc.pRootConstantsDesc) {
                    Memory::RZFree(reflection->rootSignatureDesc.pRootConstantsDesc);
                    reflection->rootSignatureDesc.pRootConstantsDesc = NULL;
                }

                Memory::RZFree(reflection->rootSignatureDesc.pDescriptorTableLayouts);
                reflection->rootSignatureDesc.pDescriptorTableLayouts = NULL;
            }
            reflection->elementCount = 0;
        }

        void CopyReflectedRootSigDesc(const rz_gfx_shader_reflection* src, rz_gfx_root_signature_desc* dst)
        {
            if (!src || !dst) return;

            // Do a deep copy of the descriptor tables and root constants
            if (src->rootSignatureDesc.pDescriptorTableLayouts) {
                dst->pDescriptorTableLayouts = (rz_gfx_descriptor_table_layout*) Memory::RZMalloc(
                    sizeof(rz_gfx_descriptor_table_layout) * src->rootSignatureDesc.descriptorTableLayoutsCount);
                memcpy(dst->pDescriptorTableLayouts, src->rootSignatureDesc.pDescriptorTableLayouts, sizeof(rz_gfx_descriptor_table_layout) * src->rootSignatureDesc.descriptorTableLayoutsCount);

                // for each descriptor table, copy the descriptors
                for (uint32_t i = 0; i < src->rootSignatureDesc.descriptorTableLayoutsCount; ++i) {
                    rz_gfx_descriptor_table_layout* srcTable = &src->rootSignatureDesc.pDescriptorTableLayouts[i];
                    rz_gfx_descriptor_table_layout* dstTable = &dst->pDescriptorTableLayouts[i];
                    if (srcTable->pDescriptors) {
                        dstTable->pDescriptors = (rz_gfx_descriptor*) Memory::RZMalloc(
                            sizeof(rz_gfx_descriptor) * srcTable->descriptorCount);
                        for (uint32_t j = 0; j < srcTable->descriptorCount; ++j) {
                            dstTable->pDescriptors[j] = srcTable->pDescriptors[j];

                            if (srcTable->pDescriptors[j].pName) {
                                dstTable->pDescriptors[j].pName = strdup(srcTable->pDescriptors[j].pName);
                            } else {
                                dstTable->pDescriptors[j].pName = NULL;
                            }
                        }
                    } else {
                        dstTable->pDescriptors = NULL;
                    }
                }

            } else {
                dst->pDescriptorTableLayouts = NULL;
            }
            if (src->rootSignatureDesc.pRootConstantsDesc) {
                dst->pRootConstantsDesc = (rz_gfx_root_constant_desc*) Memory::RZMalloc(
                    sizeof(rz_gfx_root_constant_desc) * src->rootSignatureDesc.rootConstantCount);
                memcpy(dst->pRootConstantsDesc, src->rootSignatureDesc.pRootConstantsDesc, sizeof(rz_gfx_root_constant_desc) * src->rootSignatureDesc.rootConstantCount);
            } else {
                dst->pRootConstantsDesc = NULL;
            }

            // Copy other relevant data
            dst->descriptorTableLayoutsCount = src->rootSignatureDesc.descriptorTableLayoutsCount;
            dst->rootConstantCount           = src->rootSignatureDesc.rootConstantCount;
        }

        void FreeRootSigDescMemAllocs(rz_gfx_root_signature_desc* rootSigDesc)
        {
            if (rootSigDesc->pDescriptorTableLayouts) {
                for (uint32_t i = 0; i < rootSigDesc->descriptorTableLayoutsCount; ++i) {
                    rz_gfx_descriptor_table_layout* tableLayout = &rootSigDesc->pDescriptorTableLayouts[i];
                    if (tableLayout->pDescriptors) {
                        for (u32 j = 0; j < tableLayout->descriptorCount; ++j) {
                            if (tableLayout->pDescriptors[j].pName)
                                free((void*) tableLayout->pDescriptors[j].pName);
                        }
                        Memory::RZFree(tableLayout->pDescriptors);
                        tableLayout->pDescriptors = NULL;
                    }
                    // I don't think we want to free the resource views here because they are managed by the descriptor tables
                }
                Memory::RZFree(rootSigDesc->pDescriptorTableLayouts);
                rootSigDesc->pDescriptorTableLayouts = NULL;
            }
            if (rootSigDesc->pRootConstantsDesc) {
                Memory::RZFree(rootSigDesc->pRootConstantsDesc);
                rootSigDesc->pRootConstantsDesc = NULL;
            }
            rootSigDesc->descriptorTableLayoutsCount = 0;
            rootSigDesc->rootConstantCount           = 0;
        }

        void CopyReflectedInputElements(const rz_gfx_shader_reflection* src, rz_gfx_input_element** dst, u32* elementCount)
        {
            if (!src) return;
            // Allocate memory for input elements
            *elementCount = src->elementCount;
            if (*elementCount > 0) {
                *dst = (rz_gfx_input_element*) Memory::RZMalloc(sizeof(rz_gfx_input_element) * (*elementCount));
                for (u32 i = 0; i < *elementCount; i++) {
                    (*dst)[i].pSemanticName     = strdup(src->pInputElements[i].pSemanticName);    // Deep copy the string
                    (*dst)[i].semanticIndex     = src->pInputElements[i].semanticIndex;
                    (*dst)[i].format            = src->pInputElements[i].format;
                    (*dst)[i].inputSlot         = src->pInputElements[i].inputSlot;
                    (*dst)[i].alignedByteOffset = src->pInputElements[i].alignedByteOffset;
                    (*dst)[i].inputClass        = src->pInputElements[i].inputClass;
                    (*dst)[i].instanceStepRate  = src->pInputElements[i].instanceStepRate;
                }
            } else {
                dst = NULL;
            }
        }

        void FreeInputElementsMemAllocs(rz_gfx_input_element* inputElements, u32 numElements)
        {
            if (inputElements) {
                for (u32 i = 0; i < numElements; ++i) {
                    if (inputElements[i].pSemanticName)
                        free((void*) inputElements[i].pSemanticName);
                }
                Memory::RZFree(inputElements);
                inputElements = NULL;
            }
        }

        //-----------------------------------------------------------------------------------
        // Shader Bind Map for Descriptor table management
        //-----------------------------------------------------------------------------------

        RZShaderBindMap& RZShaderBindMap::RegisterBindMap(const rz_gfx_shader_handle& shaderHandle)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(rz_handle_is_valid(&shaderHandle), "[ShaderBindMap] Invalid shader handle provided to register bind map!");

            RZShaderBindMap& bindMap   = Gfx::RZResourceManager::Get().getShaderBindMap(shaderHandle);
            rz_gfx_shader*   shader    = Gfx::RZResourceManager::Get().getShaderResource(shaderHandle);
            bindMap.m_ShaderHandle     = shaderHandle;
            bindMap.m_ShaderReflection = Gfx::ReflectShader(shader);
            bindMap.m_LastError        = BIND_MAP_VALIDATION_FAILED;

            return bindMap;
        }

        RZShaderBindMap& RZShaderBindMap::Create(void* where, const rz_gfx_shader_handle& shaderHandle)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(where != NULL, "[ShaderBindMap] Invalid memory location provided to create shader bind map!");
            RAZIX_ASSERT(rz_handle_is_valid(&shaderHandle), "[ShaderBindMap] Invalid shader handle provided to register bind map!");

            RZShaderBindMap* bindMap = new (where) RZShaderBindMap(shaderHandle);
            return *bindMap;
        }

        RZShaderBindMap& RZShaderBindMap::setResourceView(const std::string& shaderResName, const rz_gfx_resource_view& resourceView)
        {
            setResourceView(shaderResName, resourceView.resource.handle);
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::setResourceView(const std::string& shaderResName, const rz_gfx_resource_view_handle& resourceViewHandle)
        {
            RAZIX_ASSERT(rz_handle_is_valid(&resourceViewHandle), "[ShaderBindMap] Invalid resource view handle provided to register bind map!");

            dirty                                   = true;
            m_ResourceViewHandleRefs[shaderResName] = resourceViewHandle;
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::setDescriptorTable(const rz_gfx_descriptor_table& descriptorTable)
        {
            dirty = true;
            setDescriptorTable(descriptorTable.resource.handle);
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::setDescriptorTable(const rz_gfx_descriptor_table_handle& descriptorTableHandle)
        {
            RAZIX_ASSERT(rz_handle_is_valid(&descriptorTableHandle), "[ShaderBindMap] Invalid descriptor table handle provided to register bind map!");

            dirty = true;
            m_DescriptorTables.push_back(descriptorTableHandle);
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::setDescriptorBlacklist(const DescriptorBlacklist& blacklist)
        {
            dirty = true;
            m_BlacklistDescriptors.push_back(blacklist);
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::setDescriptorBlacklist(const std::string& name, const std::vector<std::string>& blacklistNames)
        {
            dirty = true;
            DescriptorBlacklist blacklist;
            blacklist.name           = name;
            blacklist.blacklistNames = blacklistNames;
            m_BlacklistDescriptors.push_back(blacklist);
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::validate()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (validated && !dirty) {
                RAZIX_CORE_WARN("[ShaderBindMap] Shader Bind Map is already validated and not dirty! No need to validate again!");
                return *this;
            }

            // validate resource views provided against shader reflection data
            // We do this so by making a mock m_TableBuilderResViewRefs and try to fill it with resviews from
            // m_ResourceViewHandleRefs and then validate against the shader reflection data for any missing data
            // This is the validation step:
            // 1. go thought all the descriptors in each table in the reflection data
            // 2. check if the descriptor is in the m_ResourceViewHandleRefs map and it not blacklisted
            // 3. if it is not in the map, then we have a missing descriptor and report it
            // 4. if it is in the map, then we have a valid descriptor and we can build the descriptor table with that resource view for that table index
            // 5. once each of table is built, we can then validate the resource views to make sure nothing is missing and that table is complete and valid,
            // 6. if anything is missing from that table that is not in the blacklist, we can print errors and report it
            // 7. Do this for all tables and build the m_TableBuilderResViewRefs completely

            if (rz_handle_is_valid(&m_ShaderHandle)) {
                rz_gfx_shader* shader = RZResourceManager::Get().getShaderResource(m_ShaderHandle);
                RAZIX_ASSERT(shader != nullptr, "[ShaderBindMap] Shader resource is null! Cannot validate shader bind map!");

                // Validate the resource views against the shader reflection data
                for (u32 t = 0; t < m_ShaderReflection.rootSignatureDesc.descriptorTableLayoutsCount; t++) {
                    // Assume table is invalid until proven otherwise
                    validated = false;

                    const rz_gfx_descriptor_table_layout* tableLayout          = &m_ShaderReflection.rootSignatureDesc.pDescriptorTableLayouts[t];
                    u32                                   validDescriptorCount = 0;

                    if (tableLayout->pDescriptors == NULL) {
                        RAZIX_CORE_ERROR("[ShaderBindMap] Descriptor table layout at index {0} is null!", t);
                        validated   = false;
                        m_LastError = BIND_MAP_VALIDATION_INVALID_DESCRIPTOR;
                        return *this;
                    }
                    // For each descriptor in the table, check if it is in the m_ResourceViewHandleRefs map
                    for (u32 d = 0; d < tableLayout->descriptorCount; d++) {
                        const rz_gfx_descriptor* desc = &tableLayout->pDescriptors[d];
                        if (desc == NULL || desc->pName == NULL) {
                            RAZIX_CORE_ERROR("[ShaderBindMap] Descriptor is null/name in table index {0}!", t);
                            validated   = false;
                            m_LastError = BIND_MAP_VALIDATION_INVALID_DESCRIPTOR;
                            return *this;
                        }
                        // Check if the descriptor is in the m_ResourceViewHandleRefs map
                        auto it = m_ResourceViewHandleRefs.find(desc->pName);
                        if (it != m_ResourceViewHandleRefs.end()) {
                            // Found the descriptor, add it to the m_TableBuilderResViewRefs map
                            m_TableBuilderResViewRefs[t].push_back({desc->pName, it->second});
                            validDescriptorCount++;
                        } else {
                            // Descriptor not found, check against blacklist
                            bool blacklisted = false;
                            for (const auto& blacklist: m_BlacklistDescriptors) {
                                if (std::find(blacklist.blacklistNames.begin(), blacklist.blacklistNames.end(), desc->pName) != blacklist.blacklistNames.end()) {
                                    RAZIX_CORE_WARN("[ShaderBindMap] Descriptor {0} idx {1} is blacklisted in table index {2}!", desc->pName, d, t);
                                    blacklisted = true;
                                    // We can still count this as a valid descriptor
                                    validDescriptorCount++;
                                    break;
                                }
                            }
                            if (!blacklisted) {
                                RAZIX_CORE_ERROR("[ShaderBindMap] Missing descriptor {0} idx {1} in table index {2}!", desc->pName, d, t);
                                validated   = false;
                                m_LastError = BIND_MAP_VALIDATION_DESCRIPTOR_MISMATCH;
                                return *this;
                            }
                        }
                    }

                    if (validDescriptorCount != tableLayout->descriptorCount) {
                        RAZIX_CORE_ERROR("[ShaderBindMap] Some descriptor resource views are missing in table index {0}! Validated {1}/{2} descriptors!", t, validDescriptorCount, tableLayout->descriptorCount);
                        validated   = false;
                        m_LastError = BIND_MAP_VALIDATION_UNFULFILLED_DESCRIPTORS;
                        return *this;
                    } else {
                        RAZIX_CORE_INFO("[ShaderBindMap] All descriptors are valid in table index {0}!", t);
                        validated = true;
                    }
                }

                RAZIX_ASSERT(validated, "[ShaderBindMap] Shader Bind Map validation failed! Some descriptors are missing or invalid! Check previous logs.");

                if (validated) {
                    RAZIX_CORE_INFO("[ShaderBindMap] Shader Bind Map validation successful! All descriptors are valid and mapped correctly to resource views! You can safely build the descriptor tables now.");
                    m_LastError = BIND_MAP_VALIDATION_SUCCESS;
                } else {
                    RAZIX_ASSERT(false, "[ShaderBindMap] Shader validation failed! Cannot build shader bind map!");
                    return *this;
                    validated   = false;
                    m_LastError = BIND_MAP_VALIDATION_FAILED;
                }
            }
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::build()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (!validated) {
                RAZIX_CORE_WARN("[ShaderBindMap] Shader Bind Map is not validated! Validating now.... Do it manually using validate() to avoid perf hits!");
                validate();
            }

            if (dirty) {
                for (u32 i = 0; i < m_TableBuilderResViewRefs.size(); i++) {
                    const std::vector<NamedResView>& resViews = m_TableBuilderResViewRefs[i];
                    if (resViews.empty()) {
                        RAZIX_CORE_WARN("[ShaderBindMap] No resource views found for table index {0}! Skipping... Probably blacklisted? or validation messed up! Please Check Logs!", i);
                        continue;
                    }

                    rz_gfx_descriptor_table_desc desc                      = {};
                    desc.tableIndex                                        = i;
                    desc.resourceViewsCount                                = (u32) resViews.size();
                    rz_gfx_descriptor_heap_handle globalResourceHeapHandle = RZEngine::Get().getWorldRenderer().getResourceHeap();
                    desc.pHeap                                             = RZResourceManager::Get().getDescriptorHeapResource(globalResourceHeapHandle);

                    rz_gfx_resource_view* pResViews = (rz_gfx_resource_view*) Memory::RZMalloc(sizeof(rz_gfx_resource_view) * resViews.size());
                    for (u32 j = 0; j < resViews.size(); j++) {
                        const NamedResView& resView = resViews[j];
                        RAZIX_ASSERT(rz_handle_is_valid(&resView.resourceViewHandle), "[ShaderBindMap] Invalid resource view handle for resource view {0} in table index {1}!", resView.name, i);
                        pResViews[j] = *RZResourceManager::Get().getResourceViewResource(resView.resourceViewHandle);
                    }
                    desc.pResourceViews = pResViews;

                    std::string                   shaderName            = RZResourceManager::Get().getShaderResource(m_ShaderHandle)->resource.pName;
                    std::string                   tableName             = "ShaderBindMap.DescriptorTable." + std::to_string(i) + "_" + shaderName;
                    rz_gfx_descriptor_heap_handle descriptorTableHandle = RZResourceManager::Get().createDescriptorTable(tableName.c_str(), desc);
                    m_DescriptorTables.push_back(descriptorTableHandle);
                    Memory::RZFree(pResViews);

                    RAZIX_CORE_INFO("[ShaderBindMap] Created descriptor table {0} for table index {1} with {2} resource views", tableName, i, resViews.size());
                }
                dirty = false;
                RAZIX_CORE_INFO("[ShaderBindMap] Shader Bind Map built successfully! Created {0} descriptor tables for shader {1}", m_DescriptorTables.size(), RZResourceManager::Get().getShaderResource(m_ShaderHandle)->resource.pName);
            }
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::clear()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_DescriptorTables.clear();
            m_BlacklistDescriptors.clear();
            m_ResourceViewHandleRefs.clear();
            dirty     = true;
            validated = false;
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::clearBlacklist()
        {
            m_BlacklistDescriptors.clear();
            dirty     = true;
            validated = false;
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy Tables created by this bind map (resviews are not this BindMaps responsibility)
            for (auto& tableHandle: m_DescriptorTables)
                RZResourceManager::Get().destroyDescriptorTable(tableHandle);

            m_DescriptorTables.clear();
            m_BlacklistDescriptors.clear();
            m_ResourceViewHandleRefs.clear();

            Gfx::FreeShaderReflectionMemAllocs(&m_ShaderReflection);
            m_ShaderReflection = {};
            m_ShaderHandle     = {};
            dirty              = true;
            validated          = false;
            m_LastError        = BIND_MAP_VALIDATION_FAILED;

            return *this;
        }

        void RZShaderBindMap::bind(rz_gfx_cmdbuf_handle cmdBufHandle, rz_gfx_pipeline_type pipelineType)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(validated && built, "[ShaderBindMap] Shader Bind Map is not validated or built! Cannot bind!");
            if (dirty) {
                RAZIX_CORE_WARN("[ShaderBindMap] Shader Bind Map is dirty! Building it again before binding... Please build it properly auto building might result in catastrophic failures");
                build();
            }

            RAZIX_ASSERT(rz_handle_is_valid(&cmdBufHandle), "[ShaderBindMap] Invalid command buffer handle provided to bind shader bind map!");
            rzRHI_BindDescriptorTablesContainer(cmdBufHandle, pipelineType, m_DescriptorTables);
        }

        RZShaderBindMap::RZShaderBindMap(rz_gfx_shader_handle shaderHandle)
            : m_ShaderHandle(shaderHandle)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(rz_handle_is_valid(&shaderHandle), "[ShaderBindMap] Invalid shader handle provided to create shader bind map!");
            rz_gfx_shader* shader = Gfx::RZResourceManager::Get().getShaderResource(m_ShaderHandle);
            m_ShaderReflection    = Gfx::ReflectShader(shader);
            m_LastError           = BIND_MAP_VALIDATION_FAILED;
        }

    }    // namespace Gfx
}    // namespace Razix
