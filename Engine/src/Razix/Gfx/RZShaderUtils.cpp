// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShaderUtils.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/Gfx/RZGfxUtil.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

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
            // TODO: Starting with table per resource? Fuck optimize this
            rz_gfx_descriptor_table_layout* tableLayouts = (rz_gfx_descriptor_table_layout*) Memory::RZMalloc(sizeof(rz_gfx_descriptor_table_layout) * shaderDesc.BoundResources);
            uint32_t                        tableCount   = 0;

            // Initialize descriptor tables to invalid state
            for (uint32_t i = 0; i < shaderDesc.BoundResources; i++) {
                tableLayouts[i].tableIndex = 0xffffffff;
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
                rz_gfx_descriptor_table_layout* tableLayout = NULL;
                for (uint32_t t = 0; t < tableCount; ++t) {
                    if (tableLayouts[t].tableIndex == bindDesc.Space) {
                        tableLayout = &tableLayouts[t];
                        break;
                    }
                }

                // If not found, create a new descriptor table for this register space
                if (!tableLayout) {
                    tableLayout                  = &tableLayouts[bindDesc.Space];
                    tableLayout->tableIndex      = bindDesc.Space;
                    tableLayout->descriptorCount = 0;
                    tableLayout->pDescriptors    = NULL;
                    tableCount++;
                }

                // Add descriptor to the table
                rz_gfx_descriptor* descriptors =
                    (rz_gfx_descriptor*) Memory::RZRealloc((void*) tableLayout->pDescriptors, sizeof(rz_gfx_descriptor) * (tableLayout->descriptorCount + 1));
                tableLayout->pDescriptors = descriptors;

                rz_gfx_descriptor* desc = &tableLayout->pDescriptors[tableLayout->descriptorCount++];
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
            outReflection->rootSignatureDesc.descriptorTableLayoutsCount = tableCount;
            outReflection->rootSignatureDesc.pDescriptorTableLayouts     = tableLayouts;

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
            if (reflection->rootSignatureDesc.pDescriptorTableLayouts) {
                for (uint32_t i = 0; i < reflection->rootSignatureDesc.descriptorTableLayoutsCount; ++i) {
                    rz_gfx_descriptor_table_layout* tableLayout = &reflection->rootSignatureDesc.pDescriptorTableLayouts[i];
                    if (tableLayout->pDescriptors) {
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
                        memcpy(dstTable->pDescriptors, srcTable->pDescriptors, sizeof(rz_gfx_descriptor) * srcTable->descriptorCount);
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

        //-----------------------------------------------------------------------------------
        // Shader Bind Map for Descriptor table management
        //-----------------------------------------------------------------------------------

        RZShaderBindMap& RZShaderBindMap::RegisterBindMap(const rz_gfx_shader_handle& shaderHandle)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(rz_handle_is_valid(&shaderHandle), "[ShaderBindMap] Invalid shader handle provided to register bind map!");

            RZShaderBindMap& bindMap   = Gfx::RZResourceManager::Get().getShaderBindMap(shaderHandle);
            bindMap.m_ShaderHandle     = shaderHandle;
            rz_gfx_shader* shader      = RZResourceManager::Get().getShaderResource(bindMap.m_ShaderHandle);
            bindMap.m_ShaderReflection = Gfx::ReflectShader(shader);

            return bindMap;
        }

        RZShaderBindMap& RZShaderBindMap::Create(void* where, const rz_gfx_shader_handle& shaderHandle)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(where != NULL, "[ShaderBindMap] Invalid memory location provided to create shader bind map!");
            RAZIX_ASSERT(rz_handle_is_valid(&shaderHandle), "[ShaderBindMap] Invalid shader handle provided to register bind map!");

            RZShaderBindMap* bindMap    = new (where) RZShaderBindMap(shaderHandle);
            bindMap->m_ShaderHandle     = shaderHandle;
            rz_gfx_shader* shader       = RZResourceManager::Get().getShaderResource(bindMap->m_ShaderHandle);
            bindMap->m_ShaderReflection = Gfx::ReflectShader(shader);

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
                        RAZIX_ASSERT(false, "[ShaderBindMap] Descriptor table layout at index {0} is null!", t);
                        validated   = false;
                        m_LastError = BIND_MAP_VALIDATION_INVALID_DESCRIPTOR;
                        return *this;
                    }
                    // For each descriptor in the table, check if it is in the m_ResourceViewHandleRefs map
                    for (u32 d = 0; d < tableLayout->descriptorCount; d++) {
                        const rz_gfx_descriptor* desc = &tableLayout->pDescriptors[d];
                        if (desc == NULL || desc->pName == NULL) {
                            RAZIX_ASSERT(false, "[ShaderBindMap] Descriptor is null/name in table index {0}!", t);
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
                                RAZIX_ASSERT(false, "[ShaderBindMap] Missing descriptor {0} idx {1} in table index {2}!", desc->pName, d, t);
                                validated   = false;
                                m_LastError = BIND_MAP_VALIDATION_DESCRIPTOR_MISMATCH;
                                return *this;
                            }
                        }
                    }

                    if (validDescriptorCount != tableLayout->descriptorCount) {
                        RAZIX_ASSERT(false, "[ShaderBindMap] Some descriptor resource views are missing in table index {0}! Validated {1}/{2} descriptors!", t, validDescriptorCount, tableLayout->descriptorCount);
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
                    RAZIX_ASSERT(false, "[ShaderBindMap] Shader handle is invalid! Cannot validate shader bind map!");
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
                dirty = false;

                // Build the descriptor tables using validated data from m_TableBuilderResViewRefs
            }
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::clear()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_DescriptorTables.clear();
            m_BlacklistDescriptors.clear();
            m_ResourceViewHandleRefs.clear();
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::clearBlacklist()
        {
            m_BlacklistDescriptors.clear();
            return *this;
        }

        RZShaderBindMap& RZShaderBindMap::destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            FreeShaderReflectionMemAllocs(&m_ShaderReflection);
            m_DescriptorTables.clear();
            m_BlacklistDescriptors.clear();
            m_ResourceViewHandleRefs.clear();

            Gfx::FreeShaderReflectionMemAllocs(&m_ShaderReflection);

            // Destroy Tables created by this bind map

            return *this;
        }

        void RZShaderBindMap::bind(rz_gfx_cmdbuf_handle cmdBufHandle)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(validated && built, "[ShaderBindMap] Shader Bind Map is not validated or built! Cannot bind!");
            if (dirty) {
                RAZIX_CORE_WARN("[ShaderBindMap] Shader Bind Map is dirty! Building it again before binding... Please build it properly auto building might result in catastrophic failures");
                build();
            }
        }
    }    // namespace Gfx
}    // namespace Razix