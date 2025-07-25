// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGfxUtil.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

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
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

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

        rz_gfx_cmdbuf_handle BeginSingleTimeCommandBuffer(const std::string& name, float4 color)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // create resources
            rz_gfx_cmdpool_desc poolDesc  = {};
            poolDesc.poolType             = RZ_GFX_CMDPOOL_TYPE_GRAPHICS;
            rz_gfx_cmdpool_handle cmdPool = Gfx::RZResourceManager::Get().createCommandPool("SingleTimeCmdPool", poolDesc);
            rz_gfx_cmdbuf_desc    desc    = {0};
            desc.pool                     = RZResourceManager::Get().getCommandPoolResource(cmdPool);
            rz_gfx_cmdbuf_handle cmdBuf   = RZResourceManager::Get().createCommandBuffer("SingleTimeCmdBuffer", desc);

            rzRHI_BeginCmdBuf(cmdBuf);
            // TODO: Add a Gfx cmd label

            return cmdBuf;
        }

        void EndSingleTimeCommandBuffer(rz_gfx_cmdbuf_handle cmdBuf)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: Add a Gfx cmd label

            rzRHI_EndCmdBuf(cmdBuf);

            // Submit for execution
            rzRHI_SubmitCmdBuf(cmdBuf);

            // wait for work to be done!
            rz_gfx_syncobj flushSyncobj = {};
            rzRHI_CreateSyncobj(&flushSyncobj, RZ_GFX_SYNCOBJ_TYPE_CPU);
            rz_gfx_syncpoint workDone = 0;
            rzRHI_FlushGPUWork(&flushSyncobj, &workDone);

            // cleanup
            rzRHI_DestroySyncobj(&flushSyncobj);
            rz_gfx_cmdbuf* cmdBufRes = RZResourceManager::Get().getCommandBufferResource(cmdBuf);
            RZResourceManager::Get().destroyCommandBuffer(cmdBuf);
            RZResourceManager::Get().destroyCommandPool((rz_gfx_cmdpool_handle) cmdBufRes->resource.desc.cmdbufDesc.pool->resource.handle);
        }
    }    // namespace Gfx
}    // namespace Razix
