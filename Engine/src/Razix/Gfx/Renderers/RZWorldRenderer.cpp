// clang-format off
#include "rzxpch.h"
#include <Core/Log/RZLog.h>
#include <Core/RZHandle.h>
// clang-format on
#include "RZWorldRenderer.h"

#define ENABLE_CODE_DRIVEN_FG_PASSES 0

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

//#include "Razix/Gfx/Lighting/RZImageBasedLightingProbesManager.h"

#include "Razix/Gfx/Passes/GlobalData.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Math/Grid.h"
#include "Razix/Math/ImportanceSampling.h"

//#include "Razix/Scene/Components/RZComponents.h"

//#include "Razix/Scene/RZScene.h"

#include "Razix/Tools/Runtime/RZEngineRuntimeTools.h"

#include "Razix/Core/Utils/RZColorUtilities.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>
#elif defined(RAZIX_PLATFORM_MACOS)
    #define GLFW_EXPOSE_NATIVE_COCOA
    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>
#elif defined(RAZIX_PLATFORM_LINUX)
    #define GLFW_EXPOSE_NATIVE_WAYLAND
    #define GLFW_EXPOSE_NATIVE_X11
    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>
#endif

namespace Razix {
    namespace Gfx {
        // Static Definitions
        rz_gfx_descriptor_heap_handle RZWorldRenderer::m_RenderTargetHeap      = {};
        rz_gfx_descriptor_heap_handle RZWorldRenderer::m_DepthRenderTargetHeap = {};
        rz_gfx_descriptor_heap_handle RZWorldRenderer::m_ResourceHeap          = {};
        rz_gfx_descriptor_heap_handle RZWorldRenderer::m_SamplerHeap           = {};
        SamplersPool                  RZWorldRenderer::m_SamplersPool          = {};
        SamplersViewPool              RZWorldRenderer::m_SamplersViewPool      = {};

        //-------------------------------------------------------------------------------------------

        static void ExportFrameGraphVisFile(const RZFrameGraph& framegraph)
        {
            auto        now   = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm     local_tm;
#if defined(_MSC_VER)    // For MSVC (Windows)
            localtime_s(&local_tm, &now_c);
#else    // For GCC/Clang (Linux/macOS)
            localtime_r(&now_c, &local_tm);
#endif
            // Format timestamp manually
            char timestamp_buffer[32];
            rz_snprintf(timestamp_buffer, sizeof(timestamp_buffer), "%Y_%m_%d_%H_%M", &local_tm);
            RZString timestamp(timestamp_buffer);

            RZString outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);

            // Construct the filename with the timestamp
            RZString filename = outPath + "/fg_debug_draw_test_" + timestamp + ".dot";

            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", filename);
            RZFileSystem::WriteTextFile(filename, filename.c_str());
        }

        //-------------------------------------------------------------------------------------------

        static void printGLFWRequiredExtensions(void)
        {
#ifdef RAZIX_RENDER_API_VULKAN
            // First we are sending in the list of desired extensions by GLFW to interface with the WPI
            u32   glfwExtensionsCount = 0;
            cstr* glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
            RAZIX_CORE_TRACE("[Vulkan] GLFW loaded extensions count : {0}", glfwExtensionsCount);

            // This is just for information and Querying purpose
    #ifdef RAZIX_DEBUG
            RAZIX_CORE_TRACE("GLFW Requested Extensions are : \n");
            for (u32 i = 0; i < glfwExtensionsCount; i++) {
                RAZIX_CORE_TRACE("\t");
                int j = 0;
                while (*(glfwExtensions[i] + j) != 0) {
                    std::cout << *(glfwExtensions[i] + j);
                    j++;
                }
                std::cout << std::endl;
            }
    #endif
#endif
        }

        //-------------------------------------------------------------------------------------------

        void RZWorldRenderer::create(RZWindow* window, u32 width, u32 height)
        {
            m_Window = window;
            memset(&m_RenderSync, 0, sizeof(RenderSyncPrimitives));
            m_FrameCount = 0;

            printGLFWRequiredExtensions();
            // Create the swapchain
            GLFWwindow*   glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
            rz_render_api api        = rzGfxCtx_GetRenderAPI();

#ifdef RAZIX_PLATFORM_WINDOWS
            if (api == RZ_RENDER_API_D3D12) {
                HWND hwnd = glfwGetWin32Window(glfwWindow);
                if (hwnd == NULL) {
                    RAZIX_CORE_ERROR("Failed to get Win32 window handle from GLFW!");
                    return;
                }
                rzRHI_CreateSwapchain(&m_Swapchain, &hwnd, width, height);

            } else if (api == RZ_RENDER_API_VULKAN) {
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                VkResult     result  = glfwCreateWindowSurface(g_GfxCtx.vk.instance, glfwWindow, nullptr, &surface);
                if (result != VK_SUCCESS) {
                    RAZIX_CORE_ERROR("Failed to create Vulkan surface! GLFW Error: {0}", result);
                    return;
                }
                rzRHI_CreateSwapchain(&m_Swapchain, &surface, width, height);
            }

#elif defined(RAZIX_PLATFORM_MACOS) || defined(RAZIX_PLATFORM_LINUX)
            if (api == RZ_RENDER_API_VULKAN) {
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                VkResult     result  = glfwCreateWindowSurface(g_GfxCtx.vk.instance, glfwWindow, nullptr, &surface);
                if (result != VK_SUCCESS) {
                    RAZIX_CORE_ERROR("Failed to create Vulkan surface! GLFW Error: {0}", result);
                    return;
                }
                rzRHI_CreateSwapchain(&m_Swapchain, &surface, width, height);

            } else {
                RAZIX_CORE_ERROR("API {0} not supported on this platform! Only Vulkan is available.", static_cast<int>(api));
                RAZIX_CORE_ASSERT(false, "Only Vulkan is supported on this platform!");
            }

#else
    #error "Unsupported platform! Add support for your target platform."
#endif
            // create present sync primitives, rendering done is per swapchain image
            // in Vulkan without VK_KHR_maintenance1 this is better than using a fence for presentation images
            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++)
                rzRHI_CreateSyncobj(&m_RenderSync.presentSync.renderingDone[i], RZ_GFX_SYNCOBJ_TYPE_GPU);

            for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                // create present sync primitives
                rzRHI_CreateSyncobj(&m_RenderSync.presentSync.imageAcquired[i], RZ_GFX_SYNCOBJ_TYPE_GPU);

                // create frame sync primitives
                rzRHI_CreateSyncobj(&m_RenderSync.frameSync.inflightSyncobj[i], g_GraphicsFeatures.support.TimelineSemaphores ? RZ_GFX_SYNCOBJ_TYPE_TIMELINE : RZ_GFX_SYNCOBJ_TYPE_CPU);

                rz_gfx_cmdpool_desc cmdPoolDesc = {};
                cmdPoolDesc.poolType            = RZ_GFX_CMDPOOL_TYPE_GRAPHICS;
                RZString commandPoolName        = "InFlightCommandPool_" + rz_to_string(i);
                m_InFlightCmdPool[i]            = RZResourceManager::Get().createCommandPool(commandPoolName.c_str(), cmdPoolDesc);

                rz_gfx_cmdbuf_desc desc        = {0};
                desc.pool                      = RZResourceManager::Get().getCommandPoolResource(m_InFlightCmdPool[i]);
                RZString inFlightCmdBufName    = "InFlightDrawCommandBuffer_" + rz_to_string(i);
                m_InFlightDrawCmdBufHandles[i] = RZResourceManager::Get().createCommandBuffer(inFlightCmdBufName.c_str(), desc);
                m_InFlightDrawCmdBufPtrs[i]    = RZResourceManager::Get().getCommandBufferResource(m_InFlightDrawCmdBufHandles[i]);
            }

            // Create generic resource and sampler heaps
            rz_gfx_descriptor_heap_desc resourceHeapDesc = {};
            resourceHeapDesc.heapType                    = RZ_GFX_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            resourceHeapDesc.descriptorCount             = 65536;
            resourceHeapDesc.flags                       = RZ_GFX_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE | RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST;
            m_ResourceHeap                               = RZResourceManager::Get().createDescriptorHeap("ResourceHeap", resourceHeapDesc);

            rz_gfx_descriptor_heap_desc samplerHeapDesc = {};
            samplerHeapDesc.heapType                    = RZ_GFX_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            samplerHeapDesc.descriptorCount             = 64;
            samplerHeapDesc.flags                       = RZ_GFX_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE | RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST;
            m_SamplerHeap                               = RZResourceManager::Get().createDescriptorHeap("SamplerHeap", samplerHeapDesc);

            rz_gfx_descriptor_heap_desc renderTargetHeapDesc = {};
            renderTargetHeapDesc.heapType                    = RZ_GFX_DESCRIPTOR_HEAP_TYPE_RTV;
            renderTargetHeapDesc.descriptorCount             = RAZIX_MAX_RENDER_TARGETS * 1024;
            renderTargetHeapDesc.flags                       = RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER;
            m_RenderTargetHeap                               = RZResourceManager::Get().createDescriptorHeap("RenderTargetHeap", renderTargetHeapDesc);

            rz_gfx_descriptor_heap_desc depthRenderTargetHeapDesc = {};
            depthRenderTargetHeapDesc.heapType                    = RZ_GFX_DESCRIPTOR_HEAP_TYPE_DSV;
            depthRenderTargetHeapDesc.descriptorCount             = 1024;    // 1024 Depth Stencil Views
            depthRenderTargetHeapDesc.flags                       = RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER;
            m_DepthRenderTargetHeap                               = RZResourceManager::Get().createDescriptorHeap("DepthRenderTargetHeap", depthRenderTargetHeapDesc);

            // HIGH PRIORITY!
            // TODO: use them as static samplers instead of building a table!
            {
                // Create some global basic samplers
                rz_gfx_sampler_desc linearSamplerDesc = {};
                linearSamplerDesc.minFilter           = RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR;
                linearSamplerDesc.magFilter           = RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR;
                linearSamplerDesc.mipFilter           = RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR;
                linearSamplerDesc.addressModeU        = RZ_GFX_TEXTURE_ADDRESS_MODE_CLAMP;
                linearSamplerDesc.addressModeV        = RZ_GFX_TEXTURE_ADDRESS_MODE_CLAMP;
                linearSamplerDesc.addressModeW        = RZ_GFX_TEXTURE_ADDRESS_MODE_CLAMP;
                linearSamplerDesc.maxAnisotropy       = 1;
                linearSamplerDesc.compareOp           = RZ_GFX_COMPARE_OP_TYPE_NEVER;
                linearSamplerDesc.minLod              = 0.0f;
                linearSamplerDesc.maxLod              = 1.0f;
                linearSamplerDesc.mipLODBias          = 0.0f;
                m_SamplersPool.linearSampler          = RZResourceManager::Get().createSampler("LinearSampler", linearSamplerDesc);

                // Create a descriptor table for all the samplers
                rz_gfx_descriptor samplerDescriptor = {};
                rz_snprintf(samplerDescriptor.pName, RAZIX_MAX_RESOURCE_NAME_CHAR, "LinearSamplerDescriptor");
                samplerDescriptor.type                                 = RZ_GFX_DESCRIPTOR_TYPE_SAMPLER;
                samplerDescriptor.location.binding                     = 0;
                samplerDescriptor.location.space                       = 0;
                rz_gfx_descriptor            samplerTableDescriptors[] = {samplerDescriptor};
                rz_gfx_descriptor_table_desc samplerTableDesc          = {};
                samplerTableDesc.tableIndex                            = 0;    // Note: Table 0 is reserved for Samplers
                samplerTableDesc.pHeap                                 = RZResourceManager::Get().getDescriptorHeapResource(m_SamplerHeap);
                samplerTableDesc.descriptorCount                       = 1;
                samplerTableDesc.pDescriptors                          = samplerTableDescriptors;    // Only one sampler for now
                m_GlobalSamplerTable                                   = RZResourceManager::Get().createDescriptorTable("GlobalSamplerTable", samplerTableDesc);

                rz_gfx_resource_view_desc samplerViewDesc             = {};
                samplerViewDesc.descriptorType                        = RZ_GFX_DESCRIPTOR_TYPE_SAMPLER;
                samplerViewDesc.samplerViewDesc.pSampler              = RZResourceManager::Get().getSamplerResource(m_SamplersPool.linearSampler);
                m_SamplersViewPool.linearSampler                      = RZResourceManager::Get().createResourceView("LinearSamplerView", samplerViewDesc);
                rz_gfx_resource_view           resourceViews[]        = {*RZResourceManager::Get().getResourceViewResource(m_SamplersViewPool.linearSampler)};
                rz_gfx_descriptor_table_update samplerTableUpdateDesc = {};
                samplerTableUpdateDesc.pTable                         = RZResourceManager::Get().getDescriptorTableResource(m_GlobalSamplerTable);
                samplerTableUpdateDesc.pResourceViews                 = resourceViews;
                samplerTableUpdateDesc.resViewCount                   = 1;
                rzRHI_UpdateDescriptorTable(samplerTableUpdateDesc);
            }
        }

        void RZWorldRenderer::destroy()
        {
            m_FrameCount = 0;

            if (m_LastSwapchainReadback.data) {
                // Note: This is allocated using malloc in the RHI readback function
                free(m_LastSwapchainReadback.data);
                m_LastSwapchainReadback.data = NULL;
            }

            // Wait for rendering to be done before halting
            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.inflightSyncobj[m_RenderSync.frameSync.inFlightSyncIdx]);

            m_FrameGraphBuildingInProgress = true;

            // Destroy Frame Graph Transient Resources
            m_FrameGraph.destroy();

            RZResourceManager::Get().destroyResourceView(m_FrameDataCBVHandle);
            RZResourceManager::Get().destroyDescriptorTable(m_FrameDataTable);

            RZResourceManager::Get().destroyResourceView(m_SceneLightsDataCBVHandle);
            RZResourceManager::Get().destroyDescriptorTable(m_SceneLightsDataTable);

            RZResourceManager::Get().destroyResourceView(m_SamplersViewPool.linearSampler);
            RZResourceManager::Get().destroySampler(m_SamplersPool.linearSampler);
            RZResourceManager::Get().destroyDescriptorTable(m_GlobalSamplerTable);

            RZResourceManager::Get().destroyDescriptorHeap(m_RenderTargetHeap);
            RZResourceManager::Get().destroyDescriptorHeap(m_DepthRenderTargetHeap);
            RZResourceManager::Get().destroyDescriptorHeap(m_SamplerHeap);
            RZResourceManager::Get().destroyDescriptorHeap(m_ResourceHeap);

            for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                RZResourceManager::Get().destroyCommandPool(m_InFlightCmdPool[i]);
                RZResourceManager::Get().destroyCommandBuffer(m_InFlightDrawCmdBufHandles[i]);
            }

            for (u32 i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++)
                rzRHI_DestroySyncobj(&m_RenderSync.presentSync.renderingDone[i]);

            for (u32 i = 0; i < RAZIX_MAX_FRAMES_IN_FLIGHT; i++) {
                rzRHI_DestroySyncobj(&m_RenderSync.presentSync.imageAcquired[i]);
                rzRHI_DestroySyncobj(&m_RenderSync.frameSync.inflightSyncobj[i]);
            }

            rzRHI_DestroySwapchain(&m_Swapchain);
        }

        /**
         * NOTE:
         * 1. In Razix we use CCW winding order for front facing triangles => Also, back facing faces are pointed towards the camera
         */

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            memset(&m_LastSwapchainReadback, 0, sizeof(rz_gfx_texture_readback));
            m_FrameGraphBuildingInProgress = true;

            //-----------------------------------------------------------------------------------
            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            m_BRDFfLUTTextureHandle             = CreateTextureFromFile("//RazixContent/Textures/Texture.Builtin.BrdfLUT.png");
            rz_gfx_texture_desc brdfTextureDesc = RZResourceManager::Get().getTextureResource(m_BRDFfLUTTextureHandle)->resource.pCold->desc.textureDesc;
            BRDFData&           brdfData        = m_FrameGraph.getBlackboard().add<BRDFData>();
            brdfData.lut                        = m_FrameGraph.import <RZFrameGraphTexture>("BRDFLut", CAST_TO_FG_TEX_DESC brdfTextureDesc, {m_BRDFfLUTTextureHandle});

            //-----------------------------------------------------------------------------------
            // Frame common data upload pass
            m_FrameGraph.getBlackboard().add<FrameData>() = m_FrameGraph.addCallbackPass<FrameData>(
                "Pass.Builtin.Code.FrameDataUpload",
                [&](FrameData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Core);

                    rz_gfx_buffer_desc framedataBufferDesc = {};
                    framedataBufferDesc.type               = RZ_GFX_BUFFER_TYPE_CONSTANT;
                    framedataBufferDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM;
                    framedataBufferDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_CBV;
                    framedataBufferDesc.sizeInBytes        = sizeof(GPUFrameData);
                    data.frameData                         = builder.create<RZFrameGraphBuffer>("FrameData", CAST_TO_FG_BUF_DESC framedataBufferDesc);
                    data.frameData                         = builder.write(data.frameData);
                },
                [=](const FrameData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    rz_gfx_cmdbuf_handle cmdBufHandle = m_InFlightDrawCmdBufHandles[m_RenderSync.frameSync.inFlightSyncIdx];

                    RAZIX_TIME_STAMP_BEGIN("Upload FrameData");
                    RAZIX_MARK_BEGIN(cmdBufHandle, "Upload FrameData", float4(0.8f, 0.2f, 0.15f, 1.0f));

                    GPUFrameData gpuData{};
                    gpuData.time += gpuData.deltaTime;
                    gpuData.deltaTime      = RZEngine::Get().GetStatistics().DeltaTime;
                    gpuData.resolution     = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    gpuData.renderFeatures = settings.renderFeatures;

                    // TODO: Support other types of frame jittering (Stratified etc.)
                    m_Jitter = m_TAAJitterHaltonSamples[(m_FrameCount % NUM_HALTON_SAMPLES_TAA_JITTER)];
                    // Based on scene sampling pattern set the apt jitter
                    if (RZEngine::Get().getWorldSettings().samplingPattern == Halton)
                        gpuData.jitterTAA = m_Jitter;

                    gpuData.previousJitterTAA = m_PreviousJitter;

                    // clang-format off
                    float4x4 jitterMatrix = float4x4(
                        1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        gpuData.jitterTAA.x, gpuData.jitterTAA.y, 0.0, 1.0    // translation
                    );
                    // clang-format on

                    // TODO: Pass jitter as separate, just to upload to GPU for other usage
                    //auto jitteredProjMatrix = sceneCam.getProjection() * jitterMatrix;

                    const auto& sceneCam              = scene->getSceneCamera();
                    gpuData.camera.projection         = sceneCam.getProjection();
                    gpuData.camera.inversedProjection = inverse(gpuData.camera.projection);
                    gpuData.camera.view               = sceneCam.getViewMatrix();
                    gpuData.camera.inversedView       = inverse(gpuData.camera.view);
                    gpuData.camera.prevViewProj       = m_PreviousViewProj;
                    gpuData.camera.fov                = sceneCam.getPerspectiveVerticalFOV();
                    gpuData.camera.nearPlane          = sceneCam.getPerspectiveNearClip();
                    gpuData.camera.farPlane           = sceneCam.getPerspectiveFarClip();

                    // update and upload the UBO
                    auto                 frameDataBufferHandle = resources.get<RZFrameGraphBuffer>(data.frameData).getRHIHandle();
                    rz_gfx_buffer_update bufferUpdate          = {};
                    bufferUpdate.pBuffer                       = RZResourceManager::Get().getBufferResource(frameDataBufferHandle);
                    bufferUpdate.sizeInBytes                   = sizeof(GPUFrameData);
                    bufferUpdate.offset                        = 0;
                    bufferUpdate.pData                         = &gpuData;
                    rzRHI_UpdateConstantBuffer(bufferUpdate);

                    // Since upload is done update the variables to store the previous data
                    {
                        m_PreviousJitter   = m_Jitter;
                        m_PreviousViewProj = gpuData.camera.projection * gpuData.camera.view;
                    }

                    // Create only once on start up when it's invalid, lazy alloc due to resource availability from FG
                    if (!rz_handle_is_valid(&m_FrameDataTable)) {
                        rz_gfx_descriptor descriptor = {};
                        rz_snprintf(descriptor.pName, RAZIX_MAX_RESOURCE_NAME_CHAR, "Descriptor.FrameData");
                        descriptor.type                        = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
                        descriptor.sizeInBytes                 = sizeof(GPUFrameData);
                        descriptor.location.binding            = 0;
                        descriptor.location.space              = 0;
                        rz_gfx_descriptor_table_desc tableDesc = {};
                        tableDesc.tableIndex                   = 0;
                        tableDesc.pHeap                        = RZResourceManager::Get().getDescriptorHeapResource(m_ResourceHeap);
                        tableDesc.descriptorCount              = 1;
                        tableDesc.pDescriptors                 = {&descriptor};
                        m_FrameDataTable                       = RZResourceManager::Get().createDescriptorTable("DescriptorTable.FrameData_0", tableDesc);

                        rz_gfx_resource_view_desc frameDataViewDesc = {};
                        frameDataViewDesc.descriptorType            = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
                        frameDataViewDesc.bufferViewDesc.pBuffer    = RZResourceManager::Get().getBufferResource(frameDataBufferHandle);
                        frameDataViewDesc.bufferViewDesc.size       = sizeof(GPUFrameData);
                        frameDataViewDesc.bufferViewDesc.offset     = 0;
                        frameDataViewDesc.bufferViewDesc.stride     = 0;
                        m_FrameDataCBVHandle                        = RZResourceManager::Get().createResourceView("ResView.FrameDataCBV", frameDataViewDesc);
                        rz_gfx_descriptor_table_update tableUpdate  = {};
                        tableUpdate.pTable                          = RZResourceManager::Get().getDescriptorTableResource(m_FrameDataTable);
                        tableUpdate.resViewCount                    = 1;
                        tableUpdate.pResourceViews                  = {RZResourceManager::Get().getResourceViewResource(m_FrameDataCBVHandle)};
                        rzRHI_UpdateDescriptorTable(tableUpdate);
                    }
                    RAZIX_MARK_END(cmdBufHandle);
                    RAZIX_TIME_STAMP_END();
                });

            //-----------------------------------------------------------------------------------
            // Scene Lights Data upload pass
            m_FrameGraph.getBlackboard().add<SceneLightsData>() = m_FrameGraph.addCallbackPass<SceneLightsData>(
                "Pass.Builtin.Code.SceneLightsDataUpload",
                [&](SceneLightsData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Core);

                    rz_gfx_buffer_desc lightdataBufferDesc = {};
                    lightdataBufferDesc.type               = RZ_GFX_BUFFER_TYPE_CONSTANT;
                    lightdataBufferDesc.usage              = RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM;
                    lightdataBufferDesc.resourceHints      = RZ_GFX_RESOURCE_VIEW_FLAG_CBV;
                    lightdataBufferDesc.sizeInBytes        = sizeof(GPULightsData);

                    data.lightsDataBuffer = builder.create<RZFrameGraphBuffer>("SceneLightsData", CAST_TO_FG_BUF_DESC lightdataBufferDesc);
                    data.lightsDataBuffer = builder.write(data.lightsDataBuffer);
                },
                [=](const SceneLightsData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    rz_gfx_cmdbuf_handle cmdBufHandle = m_InFlightDrawCmdBufHandles[m_RenderSync.frameSync.inFlightSyncIdx];

                    RAZIX_TIME_STAMP_BEGIN("Upload SceneLights");
                    RAZIX_MARK_BEGIN(cmdBufHandle, "Upload SceneLights", float4(0.2f, 0.2f, 0.75f, 1.0f));

                    GPULightsData gpuLightsData{};

                    //// Upload the lights data after updating some stuff such as position etc.
                    //auto group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
                    //for (auto entity: group) {
                    //    const auto& [lightComponent, transformComponent] = group.get<LightComponent, TransformComponent>(entity);

                    //    lightComponent.light.getLightData().position = transformComponent.Translation;
                    //    lightComponent.light.setDirection(lightComponent.light.getLightData().position);
                    //    gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                    //    gpuLightsData.numLights++;
                    //}

                    // update and upload the UBO
                    auto                 lightsDataBufferHandle = resources.get<RZFrameGraphBuffer>(data.lightsDataBuffer).getRHIHandle();
                    rz_gfx_buffer_update bufferUpdate           = {};
                    bufferUpdate.pBuffer                        = RZResourceManager::Get().getBufferResource(lightsDataBufferHandle);
                    bufferUpdate.sizeInBytes                    = sizeof(GPULightsData);
                    bufferUpdate.offset                         = 0;
                    bufferUpdate.pData                          = &gpuLightsData;
                    rzRHI_UpdateConstantBuffer(bufferUpdate);

                    // Create descriptor table and resource view for the scene lights data buffer (lazy alloc)
                    if (!rz_handle_is_valid(&m_SceneLightsDataTable)) {
                        rz_gfx_descriptor descriptor = {};
                        rz_snprintf(descriptor.pName, RAZIX_MAX_RESOURCE_NAME_CHAR, "Descriptor.SceneLightsData");
                        descriptor.type                        = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
                        descriptor.sizeInBytes                 = sizeof(GPULightsData);
                        descriptor.location.binding            = 0;
                        descriptor.location.space              = 0;
                        rz_gfx_descriptor_table_desc tableDesc = {};
                        tableDesc.tableIndex                   = 0;
                        tableDesc.pHeap                        = RZResourceManager::Get().getDescriptorHeapResource(m_ResourceHeap);
                        tableDesc.descriptorCount              = 1;
                        tableDesc.pDescriptors                 = {&descriptor};
                        m_SceneLightsDataTable                 = RZResourceManager::Get().createDescriptorTable("DescriptorTable.SceneLightsData_0", tableDesc);

                        rz_gfx_resource_view_desc lightsDataViewDesc = {};
                        lightsDataViewDesc.descriptorType            = RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
                        lightsDataViewDesc.bufferViewDesc.pBuffer    = RZResourceManager::Get().getBufferResource(lightsDataBufferHandle);
                        lightsDataViewDesc.bufferViewDesc.size       = sizeof(GPULightsData);
                        lightsDataViewDesc.bufferViewDesc.offset     = 0;
                        lightsDataViewDesc.bufferViewDesc.stride     = 0;
                        m_SceneLightsDataCBVHandle                   = RZResourceManager::Get().createResourceView("ResView.SceneLightsDataCBV", lightsDataViewDesc);
                        rz_gfx_descriptor_table_update tableUpdate   = {};
                        tableUpdate.pTable                           = RZResourceManager::Get().getDescriptorTableResource(m_SceneLightsDataTable);
                        tableUpdate.resViewCount                     = 1;
                        tableUpdate.pResourceViews                   = {RZResourceManager::Get().getResourceViewResource(m_SceneLightsDataCBVHandle)};
                        rzRHI_UpdateDescriptorTable(tableUpdate);
                    }

                    RAZIX_MARK_END(cmdBufHandle);
                    RAZIX_TIME_STAMP_END();
                });

#if 0
            //-----------------------------------------------------------------------------------
            // Load the Skybox and Global Light Probes
            // FIXME: This is hard coded make this a user land material
            // Or this is the default fallback but should be user configurable
            m_GlobalLightProbes.skybox   = RZImageBasedLightingProbesManager::convertEquirectangularToCubemap("//RazixContent/Textures/HDR/teufelsberg_inner_4k.hdr");
            m_GlobalLightProbes.diffuse  = RZImageBasedLightingProbesManager::generateIrradianceMap(m_GlobalLightProbes.skybox);
            m_GlobalLightProbes.specular = RZImageBasedLightingProbesManager::generatePreFilteredMap(m_GlobalLightProbes.skybox);
            // Import this into the Frame Graph
            auto& globalLightProbeData = m_FrameGraph.getBlackboard().add<GlobalLightProbeData>();

            auto SkyboxDesc   = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.skybox)->getDescription();
            auto DiffuseDesc  = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.diffuse)->getDescription();
            auto SpecularDesc = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.specular)->getDescription();

            SkyboxDesc.name   = "EnvironmentMap";
            DiffuseDesc.name  = "IrradianceMap";
            SpecularDesc.name = "PreFilteredMap";

            globalLightProbeData.environmentMap         = m_FrameGraph.import <RZFrameGraphTexture>("EnvironmentMap", CAST_TO_FG_TEX_DESC SkyboxDesc, {globalLightProbe.skybox});
            globalLightProbeData.diffuseIrradianceMap   = m_FrameGraph.import <RZFrameGraphTexture>("IrradianceMap", CAST_TO_FG_TEX_DESC DiffuseDesc, {globalLightProbe.diffuse});
            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <RZFrameGraphTexture>("PreFilteredMap", CAST_TO_FG_TEX_DESC SpecularDesc, {globalLightProbe.specular});

            //-----------------------------------------------------------------------------------
            // Misc Variables
            // Jitter samples for TAA
            for (int i = 0; i < NUM_HALTON_SAMPLES_TAA_JITTER; ++i) {
                // Generate jitter using Halton sequence with bases 2 and 3 for X and Y respectively
                m_TAAJitterHaltonSamples[i].x = 2.0f * (f32) (Math::ImportanceSampling::HaltonSequenceSample(i + 1, 2) - 1.0f);    // Centering the jitter around (0,0)
                m_TAAJitterHaltonSamples[i].y = 2.0f * (f32) (Math::ImportanceSampling::HaltonSequenceSample(i + 1, 3) - 1.0f);    // Centering the jitter around (0,0)
                m_TAAJitterHaltonSamples[i].x /= RZApplication::Get().getWindow()->getWidth();
                m_TAAJitterHaltonSamples[i].y /= RZApplication::Get().getWindow()->getHeight();
            }

            //-----------------------------------------------------------------------------------

            auto& frameDataBlock = m_FrameGraph.getBlackboard().get<FrameData>();

            //-----------------------------------------------------------------------------------

    #if ENABLE_DATA_DRIVEN_FG_PASSES
            //-------------------------------
            // Data Driven Frame Graph
            //-------------------------------

            if (!getFrameGraphFilePath().empty())
                RAZIX_ASSERT(m_FrameGraph.parse(getFrameGraphFilePath()), "[Frame Graph] Failed to parse graph!");
    #endif

            //-------------------------------
            // Simple Shadow map Pass
            //-------------------------------
            m_ShadowPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // GBuffer Pass
            //-------------------------------
            m_GBufferPass.addPass(m_FrameGraph, scene, &settings);
            auto& gBufferData = m_FrameGraph.getBlackboard().get<GBufferData>();

            //-------------------------------
            // PBR Deferred Pass
            //-------------------------------
            m_PBRDeferredPass.addPass(m_FrameGraph, scene, &settings);
            auto& sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Skybox Pass
            //-------------------------------
            m_SkyboxPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Tonemap Pass
            //-------------------------------
            //m_TonemapPass.addPass(m_FrameGraph, scene, &settings);

            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
            m_FrameGraph.getBlackboard().add<DebugPassData>() = m_FrameGraph.addCallbackPass<DebugPassData>(
                "Pass.Builtin.Code.DebugDraw",
                [&](DebugPassData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::Debug);

                    RZDebugRendererProxy::Get().Init();

                    builder.read(frameDataBlock.frameData);

                    sceneData.SceneHDR = builder.write(sceneData.SceneHDR);
                    data.DebugRT       = sceneData.SceneHDR;

                    gBufferData.GBufferDepth = builder.write(gBufferData.GBufferDepth);
                    data.DebugDRT            = gBufferData.GBufferDepth;
                },
                [=](const DebugPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("DebugDraw Pass");

                    // Origin point
                    //RZDebugRendererProxy::DrawPoint(float3(0.0f), 0.25f);

                    // X, Y, Z lines
                    RZDebugRendererProxy::DrawLine(float3(-100.0f, 0.0f, 0.0f), float3(100.0f, 0.0f, 0.0f), float4(1.0f, 0.0f, 0.0f, 1.0f));
                    RZDebugRendererProxy::DrawLine(float3(0.0f, -100.0f, 0.0f), float3(0.0f, 100.0f, 0.0f), float4(0.0f, 1.0f, 0.0f, 1.0f));
                    RZDebugRendererProxy::DrawLine(float3(0.0f, 0.0f, -100.0f), float3(0.0f, 0.0f, 100.0f), float4(0.0f, 0.0f, 1.0f, 1.0f));

                    // Grid
                    RZDebugRendererProxy::DrawGrid(125, float4(0.75f));

                    // Draw all lights in the scene
                    auto lights = scene->GetComponentsOfType<LightComponent>();
                    // Draw predefined light matrix
                    // Use the first directional light and currently only one Dir Light casts shadows, multiple just won't do anything in the scene not even light contribution
                    RZLight dir_light;
                    for (auto& light: lights) {
                        if (light.light.getType() == LightType::DIRECTIONAL) {
                            dir_light = light.light;
                            break;
                        }
                    }

                    float4x4 lightView  = lookAt(dir_light.getPosition(), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
                    float    near_plane = -50.0f, far_plane = 50.0f;
                    float4x4 lightProjection = ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
                    lightProjection[1][1] *= -1;
                    auto lightViewProj = lightProjection * lightView;
                    RZDebugRendererProxy::DrawFrustum(lightViewProj, float4(0.863f, 0.28f, 0.21f, 1.0f));

                    // Draw all camera frustums
                    auto cameras = scene->GetComponentsOfType<CameraComponent>();
                    for (auto& camComponents: cameras) {
                        RZDebugRendererProxy::DrawFrustum(camComponents.Camera.getFrustum(), float4(0.2f, 0.85f, 0.1f, 1.0f));
                    }

                    // Draw AABBs for all the Meshes in the Scene
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        float4x4 transform = mesh_trans.GetGlobalTransform();

                        if (mrc.Mesh && mrc.enableBoundingBoxes)
                            RZDebugRendererProxy::DrawAABB(mrc.Mesh->getBoundingBox().transform(transform), float4(0.0f, 1.0f, 0.0f, 1.0f));
                    }

                    RZDebugRendererProxy::Get().Begin(scene);

                    auto rt = resources.get<RZFrameGraphTexture>(data.DebugRT).getHandle();
                    auto dt = resources.get<RZFrameGraphTexture>(data.DebugDRT).getHandle();

                    RenderingInfo info    = {};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RHI::BeginRendering(cmdBuffer, info);

                    RZDebugRendererProxy::Get().Draw(cmdBuffer);

                    RHI::EndRendering(cmdBuffer);

                    RZDebugRendererProxy::Get().End();
                    RAZIX_TIME_STAMP_END();
                });

            //-------------------------------
            // ImGui Pass
            //-------------------------------

            m_FrameGraph.getBlackboard().add<ImGuiPassData>() = m_FrameGraph.addCallbackPass<ImGuiPassData>(
                "Pass.Builtin.Code.ImGui",
                [&](ImGuiPassData& data, RZPassResourceBuilder& builder) {
                    builder
                        .setAsStandAlonePass()
                        .setDepartment(Department::UI);

                    sceneData.SceneHDR = builder.write(sceneData.SceneHDR);
                    data.ImGuiRT       = sceneData.SceneHDR;

                    gBufferData.GBufferDepth = builder.write(gBufferData.GBufferDepth);
                    data.ImGuiDRT            = gBufferData.GBufferDepth;

                    RZImGuiRendererProxy::Get().Init();
                },
                [=](const ImGuiPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_TIME_STAMP_BEGIN("ImGui Pass");

                    RZImGuiRendererProxy::Get().Begin(scene);

                    auto rt = resources.get<RZFrameGraphTexture>(data.ImGuiRT).getHandle();
                    auto dt = resources.get<RZFrameGraphTexture>(data.ImGuiDRT).getHandle();

                    RenderingInfo info    = {};
                    info.resolution       = Resolution::kWindow;
                    info.colorAttachments = {{rt, {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {dt, {false, ClearColorPresets::DepthOneToZero}};

                    RHI::BeginRendering(Gfx::RHI::GetCurrentCommandBuffer(), info);

                    if (settings.renderFeatures & RendererFeature_ImGui)
                        RZImGuiRendererProxy::Get().Draw(Gfx::RHI::GetCurrentCommandBuffer());

                    RZImGuiRendererProxy::Get().End();
                    RAZIX_TIME_STAMP_END();
                });

            sceneData = m_FrameGraph.getBlackboard().get<SceneData>();

            //-------------------------------
            // Composition Pass
            //-------------------------------
            m_FrameGraph.getBlackboard().setFinalOutputName("SceneHDR");
            m_CompositePass.addPass(m_FrameGraph, scene, &settings);

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph....");
            m_FrameGraph.compile();

    #ifndef RAZIX_GOLD_MASTER
            // Dump the Frame Graph for visualization
            // NOTE: Careful this won't write to the Engine directory this is inside bin and build artifact
            // FIXME: Find a way to map VFS to OG Engine path pre-copy or idk just umm...be careful I guess
            ExportFrameGraphVisFile(m_FrameGraph);
    #endif

#endif
            m_FrameGraphBuildingInProgress = false;
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            m_FrameCount++;

            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (m_IsFGFilePathDirty) {
                destroy();
                RZFrameGraph::ResetFirstFrame();
                buildFrameGraph(settings, RZSceneManager::Get().getCurrentSceneMutablePtr());
                m_IsFGFilePathDirty = false;
            }

            if (m_FrameGraphBuildingInProgress)
                return;

            // Main Frame Graph World Rendering Loop
            {
                u32 inFlightSyncobjIdx = m_RenderSync.frameSync.inFlightSyncIdx;

                // If no timeline semaphores, we need to use fences for CPU-GPU sync per in-flight frame
                // So we have to wait on the CPU for the GPU to finish the frame before reusing the command buffers and other resources
                // and also before acquiring the next swapchain image to render onto
                rz_gfx_syncobj* frameSyncobj = &m_RenderSync.frameSync.inflightSyncobj[inFlightSyncobjIdx];

                // In DirectX 12, the swapchain back buffer index currBackBufferIdx directly maps to the index of the image
                // that is being presented/rendered to. This is because DXGI explicitly exposes the current back buffer index
                // via IDXGISwapChain::GetCurrentBackBufferIndex(), and the driver guarantees image acquisition in strict
                // presentation order (FIFO-like). As a result, synchronization objects like fences, command allocators, or
                // timestamp slots are usually tracked and reused per back buffer index. But we can also track it per in-flight frame.
                //
                // In contrast, Vulkan allows more flexibility: the acquired image index from vkAcquireNextImageKHR may return
                // any image in the swapchain (not necessarily in FIFO order). This requires applications to track resource
                // usage and in-flight sync per acquired image, using a round-robin or per-image tracking model. Since Vulkan
                // makes no guarantee about reuse pattern, it's incorrect to assume back buffer N will always follow N-1, hence
                // a more generalized ring buffer or semaphore timeline sync tracking model is required. Hence, we track
                // rendering done semaphores per swapchain image, and in-flight command buffers/fences per in-flight frame.

                // waits on fence/semaphore signaled by last submit for this frame index for CPU/GPU sync
                rzRHI_WaitOnPrevCmds(frameSyncobj);

                // Acquire Image to render onto
                rzRHI_AcquireImage(&m_Swapchain, &m_RenderSync.presentSync.imageAcquired[inFlightSyncobjIdx]);

                // Begin Recording  onto the command buffer, select one as per the in-flight frame idx
                const rz_gfx_cmdbuf_handle cmdBuffer = m_InFlightDrawCmdBufHandles[inFlightSyncobjIdx];
                rzRHI_BeginCmdBuf(cmdBuffer);

                // Begin Frame Marker
                RAZIX_MARK_BEGIN(cmdBuffer, "Frame # " + rz_to_string(m_FrameCount) + " [back buffer # " + rz_to_string(inFlightSyncobjIdx) + " ]", float4(1.0f, 0.0f, 1.0f, 1.0f));

                // Insert barrier to transition the swapchain image (PRESENT) to RENDER_TARGET
                rzRHI_InsertSwapchainImageBarrier(cmdBuffer, &m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx], RZ_GFX_RESOURCE_STATE_PRESENT, RZ_GFX_RESOURCE_STATE_RENDER_TARGET);

                // Execute the Frame Graph passes --> Records commands onto the current command buffer
                m_FrameGraph.execute();

                // Insert barrier to transition the swapchain image (RENDER_TARGET) to PRESENT
                rzRHI_InsertSwapchainImageBarrier(cmdBuffer, &m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx], RZ_GFX_RESOURCE_STATE_RENDER_TARGET, RZ_GFX_RESOURCE_STATE_PRESENT);

                // End Frame Marker
                RAZIX_MARK_END(cmdBuffer);

                // End command buffer recording
                rzRHI_EndCmdBuf(cmdBuffer);

                // Submit the render queue before presenting next
                rz_gfx_submit_desc submitDesc = {};
                submitDesc.pCmdBufs           = m_InFlightDrawCmdBufPtrs[inFlightSyncobjIdx];
                submitDesc.cmdCount           = 1;
                submitDesc.pWaitSyncobjs      = &m_RenderSync.presentSync.imageAcquired[inFlightSyncobjIdx];    // per-FIF, wait until image acquired
                submitDesc.waitSyncobjCount   = 1;
                submitDesc.pSignalSyncobjs    = &m_RenderSync.presentSync.renderingDone[m_Swapchain.currBackBufferIdx];    // per-image, signal when rendering done to present for this swapchain image
                submitDesc.signalSyncobjCount = 1;
                submitDesc.pFrameSyncobj      = frameSyncobj;
                rzRHI_SubmitCmdBuf(submitDesc);

                // swapchain capture is done before presentation
                if (m_ReadSwapchainThisFrame) {
                    m_ReadSwapchainThisFrame = false;

                    // Wait for rendering to be done before capturing
                    rzRHI_FlushGPUWork(frameSyncobj);
                    rzRHI_InsertSwapchainTextureReadback(&m_Swapchain.backbuffers[m_Swapchain.currBackBufferIdx], &m_LastSwapchainReadback);
                }

                // Present the image to presentation engine as soon as rendering to COLOR_ATTACHMENT is done
                rz_gfx_present_desc presentDesc = {};
                presentDesc.pSwapchain          = &m_Swapchain;
                presentDesc.pWaitSyncobjs       = &m_RenderSync.presentSync.renderingDone[m_Swapchain.currBackBufferIdx];
                presentDesc.waitSyncobjCount    = 1;
                presentDesc.pFrameSyncobj       = frameSyncobj;
                rzRHI_Present(presentDesc);

                // Advance the frame index, we are using a ring buffer so wrap around after max frames in flight
                m_RenderSync.frameSync.inFlightSyncIdx = (m_RenderSync.frameSync.inFlightSyncIdx + 1) % RAZIX_MAX_FRAMES_IN_FLIGHT;
            }
        }

        void RZWorldRenderer::OnUpdate(RZTimestep dt)
        {
        }

        void RZWorldRenderer::OnImGui()
        {
#ifndef RAZIX_GOLD_MASTER

            // TESTING IMGUI RUNTIME TOOLS CONFIG!
            // This will be owned by RZEngine
            //            static Tools::ToolsDrawConfig drawConfig = {};

            //Tools::OnImGuiDrawEngineTools(drawConfig);
#endif
        }

        void RZWorldRenderer::OnResize(u32 width, u32 height)
        {
            // make sure no work is being done on GPU before resizing resources
            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.inflightSyncobj[m_RenderSync.frameSync.inFlightSyncIdx]);

            rzRHI_ResizeSwapchain(&m_Swapchain, width, height);
            m_FrameGraph.resize(width, height);

            // wait for the GPU to be done with complete resize before resuming rendering
            m_RenderSync.frameSync.inFlightSyncIdx = 0;    // Optional: reset idx after resize
            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.inflightSyncobj[m_RenderSync.frameSync.inFlightSyncIdx]);
        }

        void RZWorldRenderer::flushGPUWork()
        {
            rzRHI_FlushGPUWork(&m_RenderSync.frameSync.inflightSyncobj[m_RenderSync.frameSync.inFlightSyncIdx]);
        }
    }    // namespace Gfx
}    // namespace Razix
