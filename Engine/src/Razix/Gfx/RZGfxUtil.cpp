// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGfxUtil.h"

#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Gfx/RZShaderUtils.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Utils/RZLoadImage.h"

namespace Razix {
    namespace Gfx {

        // Static variables for shader binary file handling
        static RZString ShaderBinaryFileExtension = "";
        static RZString ShaderBinaryFileDirectory = "";

        // Shader stage mapping for RZSF parsing
        static const std::unordered_map<RZString, rz_gfx_shader_stage> kStageMap = {
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

        //-----------------------------------------------------------------------------------
        // RZSF Parser Implementation
        //-----------------------------------------------------------------------------------

        rz_gfx_shader_desc ParseRZSF(const RZString& filePath)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_TRACE("[RZSF] Parsing shader file: {0}", filePath);

            std::map<rz_gfx_shader_stage, RZString> shaders;
            std::vector<RZString>                   shader_defines;
            rz_gfx_shader_desc                      desc       = {};
            RZString                                rzsfSource = RZVirtualFileSystem::Get().readTextFile(filePath);

            // Break the shader into lines
            std::vector<RZString> lines = GetLines(rzsfSource);
            rz_gfx_shader_stage   stage = rz_gfx_shader_stage::RZ_GFX_SHADER_STAGE_NONE;

            // Set file extensions and directories based on render API
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
                default:
                    RAZIX_CORE_WARN("[RZSF] Unknown render API, using default extensions");
                    break;
            }

            // Parse RZSF file line by line
            for (const RZString& raw_line: lines) {
                RZString line = TrimWhitespaces(raw_line);

                if (StartsWith(line, "#shader")) {
                    // Parse shader stage directive
                    for (const auto& [key, val]: kStageMap) {
                        if (StringContains(line, key)) {
                            stage          = val;
                            shaders[stage] = "";    // initialize empty source for stage
                            break;
                        }
                    }
                } else if (StartsWith(line, "#ifdef")) {
                    // Parse conditional compilation
                    RZString condition = line.substr(7);    // skip "#ifdef "
                    condition          = RemoveSpaces(condition);
                    auto defines       = SplitString(condition, "||");
                    shader_defines.insert(shader_defines.end(), defines.begin(), defines.end());
                } else if (StartsWith(line, "#include")) {
                    // Parse shader include directive
                    RZString includePath = TrimWhitespaces(line.substr(9));    // skip "#include "
                    includePath += ShaderBinaryFileExtension;
                    RZString fullPath = ShaderBinaryFileDirectory + includePath;
                    shaders[stage] += fullPath;
                }
            }

            // Load bytecode for each shader stage
            for (const auto& [stage, includePath]: shaders) {
                RZString virtualPath = "//RazixContent/Shaders/" + includePath;
                RZString outPath;

                // Resolve to actual file path
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                uint32_t bytecodeSize = (uint32_t) RZFileSystem::GetFileSize(outPath);
                u8*      bytecode     = RZVirtualFileSystem::Get().readFile(virtualPath);

                if (!bytecode) {
                    RAZIX_CORE_ERROR("[RZSF] Failed to read shader bytecode: {0}", includePath);
                    continue;
                }

                void* heapCopy = rz_mem_copy_to_heap(bytecode, bytecodeSize);

                // Assign bytecode to appropriate shader stage
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
                        desc.pipelineType         = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.mesh.stage    = stage;
                        desc.raster.mesh.bytecode = (const char*) heapCopy;
                        desc.raster.mesh.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TASK:
                        desc.pipelineType         = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.task.stage    = stage;
                        desc.raster.task.bytecode = (const char*) heapCopy;
                        desc.raster.task.size     = bytecodeSize;
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
                        RAZIX_CORE_WARN("[RZSF] Unknown or unsupported shader stage: {0}", stage);
                        break;
                }
                // bytecode will be freed by ResourceManager, RHI does no alloc/free it's our responsibility to manage memory!
            }

            return desc;
        }

        void FreeRZSFBytecodeAlloc(rz_gfx_shader* shader)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_ASSERT(shader != NULL, "[RZSF] Shader is NULL, cannot free bytecode allocation");
            rz_gfx_shader_desc* desc = &shader->resource.desc.shaderDesc;

            switch (desc->pipelineType) {
                case RZ_GFX_PIPELINE_TYPE_GRAPHICS: {
                    // Free raster pipeline stages
                    if (desc->raster.vs.bytecode) {
                        RAZIX_ASSERT(desc->raster.vs.size > 0, "[RZSF] VS bytecode has invalid size");
                        rz_free((void*) desc->raster.vs.bytecode);
                        desc->raster.vs.bytecode = NULL;
                    }
                    if (desc->raster.ps.bytecode) {
                        RAZIX_ASSERT(desc->raster.ps.size > 0, "[RZSF] PS bytecode has invalid size");
                        rz_free((void*) desc->raster.ps.bytecode);
                        desc->raster.ps.bytecode = NULL;
                    }
                    if (desc->raster.gs.bytecode) {
                        RAZIX_ASSERT(desc->raster.gs.size > 0, "[RZSF] GS bytecode has invalid size");
                        rz_free((void*) desc->raster.gs.bytecode);
                        desc->raster.gs.bytecode = NULL;
                    }
                    if (desc->raster.tcs.bytecode) {
                        RAZIX_ASSERT(desc->raster.tcs.size > 0, "[RZSF] TCS bytecode has invalid size");
                        rz_free((void*) desc->raster.tcs.bytecode);
                        desc->raster.tcs.bytecode = NULL;
                    }
                    if (desc->raster.tes.bytecode) {
                        RAZIX_ASSERT(desc->raster.tes.size > 0, "[RZSF] TES bytecode has invalid size");
                        rz_free((void*) desc->raster.tes.bytecode);
                        desc->raster.tes.bytecode = NULL;
                    }
                    if (desc->raster.task.bytecode) {
                        RAZIX_ASSERT(desc->raster.task.size > 0, "[RZSF] Task shader bytecode has invalid size");
                        rz_free((void*) desc->raster.task.bytecode);
                        desc->raster.task.bytecode = NULL;
                    }
                    if (desc->raster.mesh.bytecode) {
                        RAZIX_ASSERT(desc->raster.mesh.size > 0, "[RZSF] Mesh shader bytecode has invalid size");
                        rz_free((void*) desc->raster.mesh.bytecode);
                        desc->raster.mesh.bytecode = NULL;
                    }
                    break;
                }

                case RZ_GFX_PIPELINE_TYPE_COMPUTE: {
                    if (desc->compute.cs.bytecode) {
                        RAZIX_ASSERT(desc->compute.cs.size > 0, "[RZSF] CS bytecode has invalid size");
                        rz_free((void*) desc->compute.cs.bytecode);
                        desc->compute.cs.bytecode = NULL;
                    }
                    break;
                }

                case RZ_GFX_PIPELINE_TYPE_RAYTRACING: {
                    // Free raytracing pipeline stages
                    if (desc->raytracing.rgen.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.rgen.size > 0, "[RZSF] RGEN bytecode has invalid size");
                        rz_free((void*) desc->raytracing.rgen.bytecode);
                        desc->raytracing.rgen.bytecode = NULL;
                    }
                    if (desc->raytracing.miss.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.miss.size > 0, "[RZSF] MISS bytecode has invalid size");
                        rz_free((void*) desc->raytracing.miss.bytecode);
                        desc->raytracing.miss.bytecode = NULL;
                    }
                    if (desc->raytracing.chit.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.chit.size > 0, "[RZSF] CHIT bytecode has invalid size");
                        rz_free((void*) desc->raytracing.chit.bytecode);
                        desc->raytracing.chit.bytecode = NULL;
                    }
                    if (desc->raytracing.ahit.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.ahit.size > 0, "[RZSF] AHIT bytecode has invalid size");
                        rz_free((void*) desc->raytracing.ahit.bytecode);
                        desc->raytracing.ahit.bytecode = NULL;
                    }
                    if (desc->raytracing.callable.bytecode) {
                        RAZIX_ASSERT(desc->raytracing.callable.size > 0, "[RZSF] CALLABLE bytecode has invalid size");
                        rz_free((void*) desc->raytracing.callable.bytecode);
                        desc->raytracing.callable.bytecode = NULL;
                    }
                    break;
                }
                default:
                    RAZIX_CORE_WARN("[RZSF] Invalid or unhandled pipeline type during destruction.");
                    break;
            }
        }

        //-----------------------------------------------------------------------------------
        // Shader Reflection Implementation
        //-----------------------------------------------------------------------------------

        rz_gfx_shader_reflection ReflectShader(const rz_gfx_shader* shader)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            rz_gfx_shader_reflection reflection = {0};
            RAZIX_ASSERT(shader != NULL, "[Reflection] Shader is NULL, cannot reflect");

            const rz_gfx_shader_desc* desc = &shader->resource.desc.shaderDesc;
            RAZIX_ASSERT(desc != NULL, "[Reflection] Shader descriptor is NULL, cannot reflect");

            // Get the appropriate reflection function for the current render API
            ReflectShaderBlobBackendFn reflectShaderBlobFn = GetShaderReflectionFunction();
            if (!reflectShaderBlobFn) {
                return reflection;
            }

            // Reflect shader stages based on pipeline type
            switch (desc->pipelineType) {
                case RZ_GFX_PIPELINE_TYPE_GRAPHICS: {
                    // Reflect raster pipeline stages
                    if (desc->raster.vs.bytecode && desc->raster.vs.stage == RZ_GFX_SHADER_STAGE_VERTEX) {
                        RAZIX_ASSERT(desc->raster.vs.size > 0, "[Reflection] VS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.vs, &reflection);
                    }
                    if (desc->raster.ps.bytecode && desc->raster.ps.stage == RZ_GFX_SHADER_STAGE_PIXEL) {
                        RAZIX_ASSERT(desc->raster.ps.size > 0, "[Reflection] PS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.ps, &reflection);
                    }
                    if (desc->raster.gs.bytecode && desc->raster.gs.stage == RZ_GFX_SHADER_STAGE_GEOMETRY) {
                        RAZIX_ASSERT(desc->raster.gs.size > 0, "[Reflection] GS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.gs, &reflection);
                    }
                    if (desc->raster.tcs.bytecode && desc->raster.tcs.stage == RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL) {
                        RAZIX_ASSERT(desc->raster.tcs.size > 0, "[Reflection] TCS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.tcs, &reflection);
                    }
                    if (desc->raster.tes.bytecode && desc->raster.tes.stage == RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION) {
                        RAZIX_ASSERT(desc->raster.tes.size > 0, "[Reflection] TES bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.tes, &reflection);
                    }

                    // Reflect mesh pipeline stages
                    if (desc->raster.task.bytecode && desc->raster.task.stage == RZ_GFX_SHADER_STAGE_TASK) {
                        RAZIX_ASSERT(desc->raster.task.size > 0, "[Reflection] Task shader bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.task, &reflection);
                    }
                    if (desc->raster.mesh.bytecode && desc->raster.mesh.stage == RZ_GFX_SHADER_STAGE_MESH) {
                        RAZIX_ASSERT(desc->raster.mesh.size > 0, "[Reflection] Mesh shader bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raster.mesh, &reflection);
                    }
                    break;
                }
                case RZ_GFX_PIPELINE_TYPE_COMPUTE: {
                    if (desc->compute.cs.bytecode && desc->compute.cs.stage == RZ_GFX_SHADER_STAGE_COMPUTE) {
                        RAZIX_ASSERT(desc->compute.cs.size > 0, "[Reflection] CS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->compute.cs, &reflection);
                    }
                    break;
                }
                case RZ_GFX_PIPELINE_TYPE_RAYTRACING: {
                    // Reflect raytracing pipeline stages
                    if (desc->raytracing.rgen.bytecode && desc->raytracing.rgen.stage == RZ_GFX_SHADER_STAGE_RAY_GEN) {
                        RAZIX_ASSERT(desc->raytracing.rgen.size > 0, "[Reflection] RGEN bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.rgen, &reflection);
                    }
                    if (desc->raytracing.miss.bytecode && desc->raytracing.miss.stage == RZ_GFX_SHADER_STAGE_RAY_MISS) {
                        RAZIX_ASSERT(desc->raytracing.miss.size > 0, "[Reflection] MISS bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.miss, &reflection);
                    }
                    if (desc->raytracing.chit.bytecode && desc->raytracing.chit.stage == RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT) {
                        RAZIX_ASSERT(desc->raytracing.chit.size > 0, "[Reflection] CHIT bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.chit, &reflection);
                    }
                    if (desc->raytracing.ahit.bytecode && desc->raytracing.ahit.stage == RZ_GFX_SHADER_STAGE_RAY_ANY_HIT) {
                        RAZIX_ASSERT(desc->raytracing.ahit.size > 0, "[Reflection] AHIT bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.ahit, &reflection);
                    }
                    if (desc->raytracing.callable.bytecode && desc->raytracing.callable.stage == RZ_GFX_SHADER_STAGE_RAY_CALLABLE) {
                        RAZIX_ASSERT(desc->raytracing.callable.size > 0, "[Reflection] CALLABLE bytecode has invalid size");
                        reflectShaderBlobFn(&desc->raytracing.callable, &reflection);
                    }
                    break;
                }
                default:
                    RAZIX_CORE_WARN("[Reflection] Invalid or unhandled pipeline type during reflection.");
                    break;
            }

            return reflection;
        }

        //-----------------------------------------------------------------------------------
        // Command Buffer Utilities Implementation
        //-----------------------------------------------------------------------------------

        rz_gfx_cmdbuf_handle BeginSingleTimeCommandBuffer(const RZString& name, float4 color)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Create command pool
            rz_gfx_cmdpool_desc poolDesc  = {};
            poolDesc.poolType             = RZ_GFX_CMDPOOL_TYPE_GRAPHICS;
            rz_gfx_cmdpool_handle cmdPool = Gfx::RZResourceManager::Get().createCommandPool("SingleTimeCmdPool", poolDesc);

            // Create command buffer
            rz_gfx_cmdbuf_desc desc     = {0};
            desc.pool                   = RZResourceManager::Get().getCommandPoolResource(cmdPool);
            rz_gfx_cmdbuf_handle cmdBuf = RZResourceManager::Get().createCommandBuffer("SingleTimeCmdBuffer", desc);

            rzRHI_BeginCmdBuf(cmdBuf);
            RAZIX_MARK_BEGIN(cmdBuf, name, color);

            return cmdBuf;
        }

        void EndSingleTimeCommandBuffer(rz_gfx_cmdbuf_handle cmdBuf)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_MARK_END(cmdBuf);

            rzRHI_EndCmdBuf(cmdBuf);

            rz_gfx_syncobj flushSyncobj = {};
            rzRHI_CreateSyncobj(&flushSyncobj, RZ_GFX_SYNCOBJ_TYPE_CPU);

            // Submit for execution
            rz_gfx_submit_desc submitDesc = {};
            submitDesc.cmdCount           = 1;
            submitDesc.pCmdBufs           = RZResourceManager::Get().getCommandBufferResource(cmdBuf);
            submitDesc.pFrameSyncobj      = &flushSyncobj;    // Signal when the GPU is done for CPU to wait on it
            rzRHI_SubmitCmdBuf(submitDesc);
            // Wait for work to be done!
            rzRHI_FlushGPUWork(&flushSyncobj);

            // Cleanup
            rzRHI_DestroySyncobj(&flushSyncobj);
            rz_gfx_cmdbuf* cmdBufRes = RZResourceManager::Get().getCommandBufferResource(cmdBuf);
            RZResourceManager::Get().destroyCommandBuffer(cmdBuf);
            rz_handle handle = cmdBufRes->resource.desc.cmdbufDesc.pool->resource.handle;
            RZResourceManager::Get().destroyCommandPool(handle);
        }

        RAZIX_API rz_gfx_texture_handle CreateTextureFromFile(const RZString& filePath, bool floatingPoint)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            rz_gfx_texture_handle textureHandle = {};

            // Check if the file exists
            if (filePath.empty()) {
                RAZIX_CORE_ERROR("[Texture] File path is empty!");
                return textureHandle;
            }

            RZString physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, physicalPath)) {
                RAZIX_CORE_ERROR("[Texture] File not found: {0}", filePath);
                return textureHandle;
            }
            // Create texture descriptor
            rz_gfx_texture_desc textureDesc = {};
            if (!floatingPoint) {
                uint32_t bpp           = 0;
                textureDesc.pPixelData = LoadImageData(physicalPath.c_str(), &textureDesc.width, &textureDesc.height, &bpp);
            }
            textureDesc.mipLevels = 1;
            // FIXME: Disable mips temporarily until rzRHI_GenerateMips is fixed for internal usage, as dx12 needs explicit shader for generating mips
            //rzRHI_GetMipLevelCount(textureDesc.width, textureDesc.height);
            textureDesc.depth         = 1;
            textureDesc.arraySize     = 1;
            textureDesc.textureType   = RZ_GFX_TEXTURE_TYPE_2D;
            textureDesc.format        = floatingPoint ? RZ_GFX_FORMAT_R32G32B32A32_FLOAT : RZ_GFX_FORMAT_R8G8B8A8_UNORM;
            textureDesc.resourceHints = RZ_GFX_RESOURCE_VIEW_FLAG_SRV;
            // Load the texture from file
            textureHandle = RZResourceManager::Get().createTexture(GetFileName(filePath).c_str(), textureDesc);

            if (!rz_handle_is_valid(&textureHandle)) {
                RAZIX_CORE_ERROR("[Texture] Failed to create texture from file: {0}", filePath);
                return textureHandle;
            }

            if (textureDesc.pPixelData)
                free(textureDesc.pPixelData);

            return textureHandle;
        }
    }    // namespace Gfx
}    // namespace Razix
