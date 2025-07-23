// clang-format off
#include "rzxpch.h"
// clang-format on
#include "GfxUtil.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Utilities/RZStringUtilities.h"

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

        static std::string ShaderBinaryFileExtension = "";
        static std::string ShaderBinaryFileDirectory = "";

        static const std::unordered_map<std::string, rz_gfx_shader_stage> kStageMap = {
            {"vertex", RZ_GFX_SHADER_STAGE_VERTEX},
            {"fragment", RZ_GFX_SHADER_STAGE_PIXEL},
            {"pixel", RZ_GFX_SHADER_STAGE_PIXEL},
            {"geometry", RZ_GFX_SHADER_STAGE_GEOMETRY},
            {"compute", RZ_GFX_SHADER_STAGE_COMPUTE},
            {"tesscontrol", RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL},
            {"tesseval", RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION},
            {"mesh", RZ_GFX_SHADER_STAGE_MESH},
            {"task", RZ_GFX_SHADER_STAGE_TASK},
            {"raygen", RZ_GFX_SHADER_STAGE_RAY_GEN},
            {"miss", RZ_GFX_SHADER_STAGE_RAY_MISS},
            {"closesthit", RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT},
            {"anyhit", RZ_GFX_SHADER_STAGE_RAY_ANY_HIT}};

        rz_gfx_shader_desc ParseRZSF(const std::string& filePath)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_TRACE("Parsing .rzsf shader : {0}", filePath);

            std::map<rz_gfx_shader_stage, std::string> shaders;
            std::vector<std::string>                   shader_defines;
            rz_gfx_shader_desc                         desc       = {};
            std::string                                rzsfSource = RZVirtualFileSystem::Get().readTextFile(filePath);

            // Break the shader into lines
            std::vector<std::string> lines = Razix::Utilities::GetLines(rzsfSource);
            rz_gfx_shader_stage      stage = rz_gfx_shader_stage::RZ_GFX_SHADER_STAGE_NONE;

            switch (rzGfxCtx_GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case RZ_RENDER_API_VULKAN:
                    ShaderBinaryFileExtension = ".spv";
                    ShaderBinaryFileDirectory = "Compiled/SPIRV/";
                    break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case RZ_RENDER_API_D3D12:
                    ShaderBinaryFileExtension = ".cso";
                    ShaderBinaryFileDirectory = "Compiled/CSO/";
                    break;
#endif
                    // TODO: Add more shader types here (*.ags etc)
                default: break;
            }

            for (const std::string& raw_line: lines) {
                std::string line = Utilities::TrimWhitespaces(raw_line);

                if (Razix::Utilities::StartsWith(line, "#shader")) {
                    for (const auto& [key, val]: kStageMap) {
                        if (Razix::Utilities::StringContains(line, key)) {
                            stage          = val;
                            shaders[stage] = "";    // initialize empty source for stage
                            break;
                        }
                    }
                } else if (Razix::Utilities::StartsWith(line, "#ifdef")) {
                    std::string condition = line.substr(7);    // skip "#ifdef "
                    condition             = Razix::Utilities::RemoveSpaces(condition);
                    auto defines          = Razix::Utilities::SplitString(condition, "||");
                    shader_defines.insert(shader_defines.end(), defines.begin(), defines.end());
                } else if (Razix::Utilities::StartsWith(line, "#include")) {
                    std::string includePath = Utilities::TrimWhitespaces(line.substr(9));    // skip "#include "
                    includePath += ShaderBinaryFileExtension;
                    std::string fullPath = ShaderBinaryFileDirectory + includePath;
                    shaders[stage] += fullPath;
                }
            }

            for (const auto& [stage, includePath]: shaders) {
                std::string virtualPath = "//RazixContent/Shaders/" + includePath;
                std::string outPath;

                // Resolve to actual file path
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                uint32_t bytecodeSize = (uint32_t) RZFileSystem::GetFileSize(outPath);
                u8*      bytecode     = RZVirtualFileSystem::Get().readFile(virtualPath);

                if (!bytecode) {
                    RAZIX_CORE_ERROR("Failed to read shader bytecode: {0}", includePath);
                    continue;
                }

                void* heapCopy = Memory::RZMemCopyToHeap(bytecode, bytecodeSize);

                switch (stage) {
                    case RZ_GFX_SHADER_STAGE_VERTEX:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.vs.stage    = stage;
                        desc.raster.vs.bytecode = (const char*) heapCopy;
                        desc.raster.vs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_PIXEL:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.ps.stage    = stage;
                        desc.raster.ps.bytecode = (const char*) heapCopy;
                        desc.raster.ps.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_GEOMETRY:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.gs.stage    = stage;
                        desc.raster.gs.bytecode = (const char*) heapCopy;
                        desc.raster.gs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL:
                        desc.pipelineType        = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.tcs.stage    = stage;
                        desc.raster.tcs.bytecode = (const char*) heapCopy;
                        desc.raster.tcs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION:
                        desc.pipelineType        = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.tes.stage    = stage;
                        desc.raster.tes.bytecode = (const char*) heapCopy;
                        desc.raster.tes.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_COMPUTE:
                        desc.pipelineType        = RZ_GFX_PIPELINE_TYPE_COMPUTE;
                        desc.compute.cs.stage    = stage;
                        desc.compute.cs.bytecode = (const char*) heapCopy;
                        desc.compute.cs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_MESH:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.mesh.mesh.stage    = stage;
                        desc.mesh.mesh.bytecode = (const char*) heapCopy;
                        desc.mesh.mesh.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TASK:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.mesh.task.stage    = stage;
                        desc.mesh.task.bytecode = (const char*) heapCopy;
                        desc.mesh.task.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_GEN:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.rgen.stage    = stage;
                        desc.raytracing.rgen.bytecode = (const char*) heapCopy;
                        desc.raytracing.rgen.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_MISS:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.miss.stage    = stage;
                        desc.raytracing.miss.bytecode = (const char*) heapCopy;
                        desc.raytracing.miss.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.chit.stage    = stage;
                        desc.raytracing.chit.bytecode = (const char*) heapCopy;
                        desc.raytracing.chit.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_ANY_HIT:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.ahit.stage    = stage;
                        desc.raytracing.ahit.bytecode = (const char*) heapCopy;
                        desc.raytracing.ahit.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_CALLABLE:
                        desc.pipelineType                 = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.callable.stage    = stage;
                        desc.raytracing.callable.bytecode = (const char*) heapCopy;
                        desc.raytracing.callable.size     = bytecodeSize;
                        break;

                    default:
                        RAZIX_CORE_WARN("Unknown or unsupported shader stage: {0}", stage);
                        break;
                }
                // bytecode will be freed by ResourceManager, RHI does no alloc/free it's our responsibility to manage memory!
            }

            return desc;
        }

        void FreeRZSFBytecodeAlloc(rz_gfx_shader* shader)
        {
            RAZIX_ASSERT(shader != NULL, "Shader is NULL, cannot Memory::RZFree bytecode allocation");
            rz_gfx_shader_desc* desc = &shader->resource.desc.shaderDesc;

            switch (desc->pipelineType) {
                case RZ_GFX_PIPELINE_TYPE_GRAPHICS: {
                    if (desc->raster.vs.bytecode) {
                        RAZIX_ASSERT(desc->raster.vs.size > 0, "VS bytecode has invalid size");
                        Memory::RZFree((void*) desc->raster.vs.bytecode);
                        desc->raster.vs.bytecode = NULL;
                    }
                    if (desc->raster.ps.bytecode) {
                        RAZIX_ASSERT(desc->raster.ps.size > 0, "PS bytecode has invalid size");
                        Memory::RZFree((void*) desc->raster.ps.bytecode);
                        desc->raster.ps.bytecode = NULL;
                    }
                    if (desc->raster.gs.bytecode) {
                        RAZIX_ASSERT(desc->raster.gs.size > 0, "GS bytecode has invalid size");
                        Memory::RZFree((void*) desc->raster.gs.bytecode);
                        desc->raster.gs.bytecode = NULL;
                    }
                    if (desc->raster.tcs.bytecode) {
                        RAZIX_ASSERT(desc->raster.tcs.size > 0, "TCS bytecode has invalid size");
                        Memory::RZFree((void*) desc->raster.tcs.bytecode);
                        desc->raster.tcs.bytecode = NULL;
                    }
                    if (desc->raster.tes.bytecode) {
                        RAZIX_ASSERT(desc->raster.tes.size > 0, "TES bytecode has invalid size");
                        Memory::RZFree((void*) desc->raster.tes.bytecode);
                        desc->raster.tes.bytecode = NULL;
                    }
                    if (desc->mesh.task.bytecode) {
                        RAZIX_ASSERT(desc->mesh.task.size > 0, "Task shader bytecode has invalid size");
                        Memory::RZFree((void*) desc->mesh.task.bytecode);
                        desc->mesh.task.bytecode = NULL;
                    }
                    if (desc->mesh.mesh.bytecode) {
                        RAZIX_ASSERT(desc->mesh.mesh.size > 0, "Mesh shader bytecode has invalid size");
                        Memory::RZFree((void*) desc->mesh.mesh.bytecode);
                        desc->mesh.mesh.bytecode = NULL;
                    }
                    if (desc->mesh.ps.bytecode) {
                        RAZIX_ASSERT(desc->mesh.ps.size > 0, "PS bytecode has invalid size");
                        Memory::RZFree((void*) desc->mesh.ps.bytecode);
                        desc->mesh.ps.bytecode = NULL;
                    }
                    break;
                    break;
                }

                case RZ_GFX_PIPELINE_TYPE_COMPUTE: {
                    if (desc->compute.cs.bytecode) {
                        RAZIX_ASSERT(desc->compute.cs.size > 0, "CS bytecode has invalid size");
                        Memory::RZFree((void*) desc->compute.cs.bytecode);
                        desc->compute.cs.bytecode = NULL;
                    }
                    break;
                }

                case RZ_GFX_PIPELINE_TYPE_RAYTRACING: {
                    if (desc->raytracing.rgen.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.rgen.size > 0, "RGEN bytecode has invalid size");
                        Memory::RZFree((void*) desc->raytracing.rgen.bytecode);
                        desc->raytracing.rgen.bytecode = NULL;
                    }
                    if (desc->raytracing.miss.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.miss.size > 0, "MISS bytecode has invalid size");
                        Memory::RZFree((void*) desc->raytracing.miss.bytecode);
                        desc->raytracing.miss.bytecode = NULL;
                    }
                    if (desc->raytracing.chit.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.chit.size > 0, "CHIT bytecode has invalid size");
                        Memory::RZFree((void*) desc->raytracing.chit.bytecode);
                        desc->raytracing.chit.bytecode = NULL;
                    }
                    if (desc->raytracing.ahit.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.ahit.size > 0, "AHIT bytecode has invalid size");
                        Memory::RZFree((void*) desc->raytracing.ahit.bytecode);
                        desc->raytracing.ahit.bytecode = NULL;
                    }
                    if (desc->raytracing.callable.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.callable.size > 0, "CALLABLE bytecode has invalid size");
                        Memory::RZFree((void*) desc->raytracing.callable.bytecode);
                        desc->raytracing.callable.bytecode = NULL;
                    }
                    break;
                }
                default:
                    RAZIX_CORE_WARN("[D3D12 Shader] Invalid or unhandled pipeline type during destruction.");
                    break;
            }
        }

        typedef void (*ReflectShaderBlobBackendFn)(const rz_gfx_shader_stage_blob*, rz_gfx_shader_reflection*);

#ifdef RAZIX_RENDER_API_VULKAN
        static void vk_reflect_shader_blob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection)
        {
            RAZIX_UNUSED(stageBlob);
            RAZIX_UNUSED(outReflection);
        }
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
        // Mask represents the number of components in binary form.
        // ex. float3 would be represented by 7 (0xb111), float2 by 3
        static u32 dx12_get_component_count(u32 mask)
        {
            u32 count = 0;
            while (mask) {
                count += mask & 1;
                mask >>= 1;
            }
            return count;
        }

        DXGI_FORMAT dx12_get_format_from_component(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount)
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

        u32 dx12_get_dxgi_format_size(DXGI_FORMAT format)
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

        rz_gfx_descriptor_type dx12_convert_input_type_to_descriptor_type(D3D_SHADER_INPUT_TYPE inputType)
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

        static void dx12_reflect_shader_blob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection)
        {
            IDxcUtils* shaderReflectionUtils = nullptr;
            HRESULT    hr                    = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&shaderReflectionUtils));
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("Failed to create DxcUtils instance for shader reflection: {0}", hr);
                return;
            }

            IDxcBlobEncoding* blob = NULL;
            hr                     = shaderReflectionUtils->CreateBlob(stageBlob->bytecode, (u32) stageBlob->size, CP_UTF8, &blob);
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("Failed to create DxcBlobEncoding for shader reflection: {0}", hr);
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
                RAZIX_CORE_ERROR("Failed to create shader reflection: {0}", hr);
                blob->Release();
                shaderReflectionUtils->Release();
                return;
            }
            // Get the shader desc reflection data
            D3D12_SHADER_DESC shaderDesc = {};
            hr                           = shaderReflection->GetDesc(&shaderDesc);
            if (FAILED(hr)) {
                RAZIX_CORE_ERROR("Failed to get shader description: {0}", hr);
                shaderReflection->Release();
                blob->Release();
                shaderReflectionUtils->Release();
                return;
            }

            if (stageBlob->stage == RZ_GFX_SHADER_STAGE_VERTEX) {
                // Reflect input parameters
                outReflection->pInputElements = (rz_gfx_input_element*) Memory::RZMalloc(sizeof(rz_gfx_input_element) * shaderDesc.InputParameters);
                u32 tableCount                = 0;

                for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
                    D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
                    hr = shaderReflection->GetInputParameterDesc(i, &paramDesc);
                    if (FAILED(hr)) {
                        RAZIX_CORE_ERROR("Failed to get input parameter description: {0}", hr);
                        continue;
                    }

                    u32         componentCount = dx12_get_component_count(paramDesc.Mask);
                    DXGI_FORMAT format         = dx12_get_format_from_component(paramDesc.ComponentType, componentCount);
                    u32         formatSize     = dx12_get_dxgi_format_size(format);

                    if (format == DXGI_FORMAT_UNKNOWN || formatSize == 0) {
                        RAZIX_CORE_ERROR("Unsupported format or size for input parameter: {0}", paramDesc.SemanticName);
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

            for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc = {0};
                hr                                    = shaderReflection->GetResourceBindingDesc(i, &bindDesc);
                if (FAILED(hr)) {
                    RAZIX_CORE_ERROR("Failed to get resource binding desc at index {0}", i);
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
                    table                  = &tableDescs[tableCount++];
                    table->tableIndex      = bindDesc.Space;
                    table->descriptorCount = 0;
                    table->pDescriptors    = NULL;
                }

                // Add descriptor to the table
                rz_gfx_descriptor* descriptors =
                    (rz_gfx_descriptor*) Memory::RZRealloc(table->pDescriptors, sizeof(rz_gfx_descriptor) * (table->descriptorCount + 1));
                table->pDescriptors = descriptors;

                rz_gfx_descriptor* desc = &table->pDescriptors[table->descriptorCount++];
                memset(desc, 0, sizeof(rz_gfx_descriptor));
                desc->pName            = bindDesc.Name;
                desc->location.binding = bindDesc.BindPoint;
                desc->location.space   = bindDesc.Space;
                desc->type             = dx12_convert_input_type_to_descriptor_type(bindDesc.Type);
                desc->arraySize        = bindDesc.BindCount;
                desc->sizeInBytes      = 0;
                desc->offsetInBytes    = 0;
                desc->memberCount      = 0;
            }

            // Store the descriptor tables
            outReflection->rootSignatureDesc.descriptorTableCount  = tableCount;
            outReflection->rootSignatureDesc.pDescriptorTablesDesc = tableDescs;
        }
#endif

        rz_gfx_shader_reflection ReflectShader(const rz_gfx_shader* shader)
        {
            rz_gfx_shader_reflection reflection = {0};
            RAZIX_ASSERT(shader != NULL, "Shader is NULL, cannot reflect");
            const rz_gfx_shader_desc* desc = &shader->resource.desc.shaderDesc;
            RAZIX_ASSERT(desc != NULL, "Shader descriptor is NULL, cannot reflect");

            ReflectShaderBlobBackendFn reflectShaderBlobFn = NULL;
            if (rzGfxCtx_GetRenderAPI() == RZ_RENDER_API_VULKAN) {
#ifdef RAZIX_RENDER_API_VULKAN
                reflectShaderBlobFn = vk_reflect_shader_blob;
#endif
            } else if (rzGfxCtx_GetRenderAPI() == RZ_RENDER_API_D3D12) {
#ifdef RAZIX_RENDER_API_DIRECTX12
                reflectShaderBlobFn = dx12_reflect_shader_blob;
#endif
            } else {
                RAZIX_CORE_ERROR("Unsupported Render API for shader reflection: {0}", rzGfxCtx_GetRenderAPIString());
                return reflection;
            }

            static_assert(RZ_RENDER_API_COUNT == 2, "Update reflectShaderBlobFn table!");

            switch (desc->pipelineType) {
                case RZ_GFX_PIPELINE_TYPE_GRAPHICS: {
                    if (desc->raster.vs.bytecode) {
                        RAZIX_ASSERT(desc->raster.vs.size > 0, "VS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.vs, &reflection);
                    }
                    if (desc->raster.ps.bytecode) {
                        RAZIX_ASSERT(desc->raster.ps.size > 0, "PS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.ps, &reflection);
                    }
                    if (desc->raster.gs.bytecode) {
                        RAZIX_ASSERT(desc->raster.gs.size > 0, "GS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.gs, &reflection);
                    }
                    if (desc->raster.tcs.bytecode) {
                        RAZIX_ASSERT(desc->raster.tcs.size > 0, "TCS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.tcs, &reflection);
                    }
                    if (desc->raster.tes.bytecode) {
                        RAZIX_ASSERT(desc->raster.tes.size > 0, "TES bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.tes, &reflection);
                    }
                    if (desc->mesh.task.bytecode) {
                        RAZIX_ASSERT(desc->mesh.task.size > 0, "Task shader bytecode has invalid size");
                        reflectShaderBlobFn(&desc->mesh.task, &reflection);
                    }
                    if (desc->mesh.mesh.bytecode) {
                        RAZIX_ASSERT(desc->mesh.mesh.size > 0, "Mesh shader bytecode has invalid size");
                        reflectShaderBlobFn(&desc->mesh.mesh, &reflection);
                    }
                    if (desc->mesh.ps.bytecode) {
                        RAZIX_ASSERT(desc->mesh.ps.size > 0, "PS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->mesh.ps, &reflection);
                    }
                    break;
                }
                case RZ_GFX_PIPELINE_TYPE_COMPUTE: {
                    if (desc->compute.cs.bytecode) {
                        RAZIX_ASSERT(desc->compute.cs.size > 0, "CS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->compute.cs, &reflection);
                    }
                    break;
                }
                case RZ_GFX_PIPELINE_TYPE_RAYTRACING: {
                    if (desc->raytracing.rgen.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.rgen.size > 0, "RGEN bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.rgen, &reflection);
                    }
                    if (desc->raytracing.miss.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.miss.size > 0, "MISS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.miss, &reflection);
                    }
                    if (desc->raytracing.chit.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.chit.size > 0, "CHIT bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.chit, &reflection);
                    }
                    if (desc->raytracing.ahit.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.ahit.size > 0, "AHIT bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.ahit, &reflection);
                    }
                    if (desc->raytracing.callable.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.callable.size > 0, "CALLABLE bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.callable, &reflection);
                    }
                    break;
                }
                default:
                    RAZIX_RHI_LOG_WARN("[D3D12 Shader] Invalid or unhandled pipeline type during destruction.");
                    break;
            }
            return reflection;
        }

#if 0

        static std::unordered_map<std::string, Razix::Gfx::CullMode> CullModeStringMap = {
            {"Back", Razix::Gfx::CullMode::Back},
            {"Front", Razix::Gfx::CullMode::Front},
            {"FrontBack", Razix::Gfx::CullMode::FrontBack},
            {"None", Razix::Gfx::CullMode::None},
        };

        static std::unordered_map<std::string, Razix::Gfx::TextureFormat> TextureFormatStringMap = {
            {"R8", Razix::Gfx::TextureFormat::R8},
            {"R32_INT", Razix::Gfx::TextureFormat::R32_INT},
            {"R32_UINT", Razix::Gfx::TextureFormat::R32_UINT},
            {"R32F", Razix::Gfx::TextureFormat::R32F},
            {"RG8", Razix::Gfx::TextureFormat::RG8},
            {"RGB8", Razix::Gfx::TextureFormat::RGB8},
            {"RGBA8", Razix::Gfx::TextureFormat::RGBA8},
            {"RGB16", Razix::Gfx::TextureFormat::RGB16},
            {"RGBA16", Razix::Gfx::TextureFormat::RGBA16},
            {"RGB32", Razix::Gfx::TextureFormat::RGB32},
            {"RGBA32", Razix::Gfx::TextureFormat::RGBA32},
            {"RGBA32F", Razix::Gfx::TextureFormat::RGBA32F},
            {"RGB", Razix::Gfx::TextureFormat::RGB},
            {"RGBA", Razix::Gfx::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Razix::Gfx::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Razix::Gfx::TextureFormat::DEPTH32F},
            {"STENCIL", Razix::Gfx::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Razix::Gfx::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Razix::Gfx::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Razix::Gfx::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Razix::Gfx::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Razix::Gfx::TextureFormat::R11G11B10A2_SFLOAT}};

        static std::unordered_map<std::string, Razix::Gfx::Wrapping> TextureWrapModeStringMap = {
            {"REPEAT", Razix::Gfx::Wrapping::kRepeat},
            {"MIRRORED_REPEAT", Razix::Gfx::Wrapping::kMirroredRepeat},
            {"CLAMP_TO_EDGE", Razix::Gfx::Wrapping::kClampToEdge},
            {"CLAMP_TO_BORDER", Razix::Gfx::Wrapping::kClampToBorder}};

        static std::unordered_map<std::string, Razix::Gfx::Filtering::Mode> TextureFiletingModeStringMap = {
            {"LINEAR", Razix::Gfx::Filtering::Mode::kFilterModeLinear},
            {"NEAREST", Razix::Gfx::Filtering::Mode::kFilterModeNearest}};

        static std::unordered_map<std::string, Razix::Gfx::BlendFactor>
            BlendFactorStringMap = {
                {"Zero", Razix::Gfx::BlendFactor::Zero},
                {"One", Razix::Gfx::BlendFactor::One},
                {"SrcColor", Razix::Gfx::BlendFactor::SrcColor},
                {"OneMinusSrcColor", Razix::Gfx::BlendFactor::OneMinusSrcColor},
                {"DstColor", Razix::Gfx::BlendFactor::DstColor},
                {"OneMinusDstColor", Razix::Gfx::BlendFactor::OneMinusDstColor},
                {"SrcAlpha", Razix::Gfx::BlendFactor::SrcAlpha},
                {"OneMinusSrcAlpha", Razix::Gfx::BlendFactor::OneMinusSrcAlpha},
                {"DstAlpha", Razix::Gfx::BlendFactor::DstAlpha},
                {"OneMinusDstAlpha", Razix::Gfx::BlendFactor::OneMinusDstAlpha},
                {"ConstantColor", Razix::Gfx::BlendFactor::ConstantColor},
                {"OneMinusConstantColor", Razix::Gfx::BlendFactor::OneMinusConstantColor},
                {"ConstantAlpha", Razix::Gfx::BlendFactor::ConstantAlpha},
                {"OneMinusConstantAlpha", Razix::Gfx::BlendFactor::OneMinusConstantAlpha},
                {"SrcAlphaSaturate", Razix::Gfx::BlendFactor::SrcAlphaSaturate}};

        static std::unordered_map<std::string, Razix::Gfx::BufferUsage> BufferUsageStringMap = {
            {"Static", Razix::Gfx::BufferUsage::Static},
            {"PersistentStream", Razix::Gfx::BufferUsage::PersistentStream},
            {"Staging", Razix::Gfx::BufferUsage::Staging},
            {"ReadBack", Razix::Gfx::BufferUsage::ReadBack},
            {"IndirectDrawArgs", Razix::Gfx::BufferUsage::IndirectDrawArgs}};

        static std::unordered_map<std::string, Razix::Gfx::rz_gfx_shader_stage> rz_gfx_shader_stageStringMap = {
            {"NONE", Razix::Gfx::rz_gfx_shader_stage::kNone},
            {"VERTEX", Razix::Gfx::rz_gfx_shader_stage::kVertex},
            {"PIXEL", Razix::Gfx::rz_gfx_shader_stage::kPixel},
            {"COMPUTE", Razix::Gfx::rz_gfx_shader_stage::kCompute},
            {"GEOMETRY", Razix::Gfx::rz_gfx_shader_stage::kGeometry},
            {"TCS", Razix::Gfx::rz_gfx_shader_stage::kTesselationControl},
            {"TES", Razix::Gfx::rz_gfx_shader_stage::kTesselationEvaluation}};

        static std::unordered_map<std::string, Gfx::TextureType> StringTextureTypeMap = {

            {"Texture_1D", Gfx::TextureType::k1D},
            {"Texture_2D", Gfx::TextureType::k2D},
            {"Texture_2DArray", Gfx::TextureType::k2DArray},
            {"Texture_3D", Gfx::TextureType::k3D},
            {"Texture_Depth", Gfx::TextureType::kDepth},
            {"Texture_CubeMap", Gfx::TextureType::kCubeMap},
            {"Texture_CubeMapArray", Gfx::TextureType::kCubeMapArray},
            {"Texture_SwapchainImage", Gfx::TextureType::kSwapchainImage}};

        static std::unordered_map<std::string, Gfx::TextureFormat> StringTextureFormatMap = {
            {"RGBA32F", Gfx::TextureFormat::RGBA32F},
            {"R8", Gfx::TextureFormat::R8},
            {"R32_INT", Gfx::TextureFormat::R32_INT},
            {"R32_UINT", Gfx::TextureFormat::R32_UINT},
            {"R32F", Gfx::TextureFormat::R32F},
            {"RG8", Gfx::TextureFormat::RG8},
            {"RG16F", Gfx::TextureFormat::RG16F},
            {"RGB8", Gfx::TextureFormat::RGB8},
            {"RGBA8", Gfx::TextureFormat::RGBA8},
            {"RGB16", Gfx::TextureFormat::RGB16},
            {"RGBA16", Gfx::TextureFormat::RGBA16},
            {"RGBA16F", Gfx::TextureFormat::RGBA16F},
            {"RGB32", Gfx::TextureFormat::RGB32},
            {"RGBA32", Gfx::TextureFormat::RGBA32},
            {"RGB", Gfx::TextureFormat::RGB},
            {"RGBA", Gfx::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Gfx::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Gfx::TextureFormat::DEPTH32F},
            {"STENCIL", Gfx::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Gfx::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Gfx::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Gfx::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Gfx::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Gfx::TextureFormat::R11G11B10A2_SFLOAT},
            {"NONE", Gfx::TextureFormat::NONE}};

        static std::unordered_map<std::string, Razix::Gfx::CompareOp>
            CompareOpStringMap = {
                {"Never", Razix::Gfx::CompareOp::Never},
                {"Less", Razix::Gfx::CompareOp::Less},
                {"Equal", Razix::Gfx::CompareOp::Equal},
                {"LessOrEqual", Razix::Gfx::CompareOp::LessOrEqual},
                {"Greater", Razix::Gfx::CompareOp::Greater},
                {"NotEqual", Razix::Gfx::CompareOp::NotEqual},
                {"GreaterOrEqual", Razix::Gfx::CompareOp::GreaterOrEqual},
                {"Always", Razix::Gfx::CompareOp::Always}};

        static std::unordered_map<std::string, Razix::Gfx::BlendOp> BlendOpStringMap = {
            {"Add", Razix::Gfx::BlendOp::Add},
            {"Subtract", Razix::Gfx::BlendOp::Subtract},
            {"ReverseSubtract", Razix::Gfx::BlendOp::ReverseSubtract},
            {"Min", Razix::Gfx::BlendOp::Min},
            {"Max", Razix::Gfx::BlendOp::Max}};

        static std::unordered_map<std::string, Razix::Gfx::DrawType> DrawTypeStringMap = {
            {"Triangle", Razix::Gfx::DrawType::Triangle},
            {"Line", Razix::Gfx::DrawType::Line},
            {"Point", Razix::Gfx::DrawType::Point}};

        static std::unordered_map<std::string, Razix::Gfx::PolygonMode> PolygonModeStringMap = {
            {"Fill", Razix::Gfx::PolygonMode::Fill},
            {"Line", Razix::Gfx::PolygonMode::Line},
            {"Point", Razix::Gfx::PolygonMode::Point}};

       

        //-----------------------------------------------------------------------------------

        float4 ClearColorFromPreset(ClearColorPresets preset)
        {
            switch (preset) {
                case Razix::Gfx::ClearColorPresets::OpaqueBlack:
                    return float4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::OpaqueWhite:
                    return float4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::TransparentBlack:
                    return float4(0.0f, 0.0f, 0.0f, 0.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::TransparentWhite:
                    return float4(1.0f, 1.0f, 1.0f, 0.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::Pink:
                    return float4(1.0f, 0.0f, 1.0f, 1.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::DepthZeroToOne:
                    return float4(0.0f, 1.0f, 0.0f, 0.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::DepthOneToZero:
                    return float4(1.0f, 0.0f, 0.0f, 0.0f);
                    break;
                default:
                    return float4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
            }
        }

        //-----------------------------------------------------------------------------------

        std::string RZTextureDesc::TypeToString(TextureType type)
        {
            switch (type) {
                case Razix::Gfx::TextureType::k2D:
                    return "2D";
                    break;
                case Razix::Gfx::TextureType::k3D:
                    return "3D";
                    break;
                case Razix::Gfx::TextureType::kCubeMap:
                    return "CubeMap";
                    break;
                case Razix::Gfx::TextureType::kDepth:
                    return "Depth";
                    break;
                case Razix::Gfx::TextureType::kSwapchainImage:
                    return "Swapchain Image";
                    break;
                case TextureType::k1D:
                    return "1D";
                    break;
                case TextureType::k2DArray:
                    return "2D Array";
                    break;
                case TextureType::kCubeMapArray:
                    return "CubeMap Array";
                    break;
                default:
                    return "NONE";
                    break;
            }
            return "NONE";
        }

        //-----------------------------------------------------------------------------------

        Razix::Gfx::TextureFormat RZTextureDesc::StringToFormat(const std::string& str)
        {
            RAZIX_ASSERT((StringTextureFormatMap.size() == (u32) TextureFormat::COUNT), "TextureFormat string enum map has missing values");
            return StringTextureFormatMap[str];
        }

        Razix::Gfx::TextureType RZTextureDesc::StringToType(const std::string& str)
        {
            return StringTextureTypeMap[str];
        }

        Razix::Gfx::CompareOp StringToCompareOp(const std::string& str)
        {
            return CompareOpStringMap[str];
        }

        Razix::Gfx::BlendOp StringToBlendOp(const std::string& str)
        {
            return BlendOpStringMap[str];
        }

        Razix::Gfx::DrawType StringToDrawType(const std::string& str)
        {
            return DrawTypeStringMap[str];
        }

        Razix::Gfx::PolygonMode StringToPolygonMode(const std::string& str)
        {
            return PolygonModeStringMap[str];
        }

        Razix::Gfx::CullMode StringToCullMode(const std::string& str)
        {
            return CullModeStringMap[str];
        }

        TextureFormat StringToTextureFormat(const std::string& str)
        {
            return TextureFormatStringMap[str];
        }

        Razix::Gfx::Wrapping StringToWrapping(const std::string& str)
        {
            return TextureWrapModeStringMap[str];
        }

        Razix::Gfx::Filtering::Mode StringToFilteringMode(const std::string& str)
        {
            return TextureFiletingModeStringMap[str];
        }

        Razix::Gfx::BlendFactor StringToBlendFactor(const std::string& str)
        {
            return BlendFactorStringMap[str];
        }

        Razix::Gfx::BufferUsage StringToBufferUsage(const std::string& str)
        {
            return BufferUsageStringMap[str];
        }

        Razix::Gfx::rz_gfx_shader_stage StringTorz_gfx_shader_stage(const std::string& str)
        {
            return rz_gfx_shader_stageStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        u32 BufferLayoutElement::getCount()
        {
            switch (format) {
                case Razix::Gfx::BufferFormat::R8_INT:
                case Razix::Gfx::BufferFormat::R8_UINT:
                case Razix::Gfx::BufferFormat::R32_INT:
                case Razix::Gfx::BufferFormat::R32_UINT:
                case Razix::Gfx::BufferFormat::R32_FLOAT:
                    return 1;
                    break;
                case Razix::Gfx::BufferFormat::R32G32_INT:
                case Razix::Gfx::BufferFormat::R32G32_UINT:
                case Razix::Gfx::BufferFormat::R32G32_FLOAT:
                    return 2;
                    break;
                case Razix::Gfx::BufferFormat::R32G32B32_INT:
                case Razix::Gfx::BufferFormat::R32G32B32_UINT:
                case Razix::Gfx::BufferFormat::R32G32B32_FLOAT:
                    return 3;
                    break;
                case Razix::Gfx::BufferFormat::R32G32B32A32_INT:
                case Razix::Gfx::BufferFormat::R32G32B32A32_UINT:
                case Razix::Gfx::BufferFormat::R32G32B32A32_FLOAT:
                    return 4;
                    break;
                default:
                    return 0;
                    break;
            }
        }

        void RZBufferLayout::pushImpl(const std::string& name, BufferFormat format, u32 size, bool Normalised)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Elements.push_back({name, format, m_Stride, Normalised});
            m_Stride += size;
        }, mo
#endif

    }    // namespace Gfx
}    // namespace Razix
