#pragma once

#include <tracy/Tracy.hpp>

#include "Razix/Core/RZProfiling.h"

#include "Razix/Graphics/RHI/API/RZAPIHandles.h"
#include "Razix/Graphics/RHI/API/RZBarriers.h"
#include "Razix/Graphics/RHI/API/RZBindingInfoAccessViews.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        class RZCommandBuffer;
        class RZPipeline;
        class RZDescriptorSet;
        class RZSwapchain;
        class RZDescriptorSet;
        class RZSemaphore;
        struct RZPushConstant;

        enum class DataType
        {
            FLOAT,
            UNSIGNED_INT,
            UNSIGNED_BYTE
        };

        // TODO: Move this to RZApplication and cache it per application
        /* Rendering Resolution of the render targets */
        enum class Resolution : u32
        {
            k1080p = 0,  /* native 1920x1080 full HD resolution                                 */
            k1440p,      /* native 2K resolution 2560x1440 rendering                            */
            k4KUpscaled, /* Upscaled using FSR/DLSS                                             */
            k4KNative,   /* native 3840x2160 rendering                                          */
            kWindow,     /* Selects the resolution dynamically based on the presentation window */
            kCustom,     /* Custom resolution for rendering                                     */
            COUNT
        };

        static const char* ResolutionNames[] =
            {
                "k1080p",
                "k1440p",
                "k4KUpscaled",
                "k4KNative",
                "kWindow",
                "kCustom"};

        RAZIX_ENUM_NAMES_ASSERT(ResolutionNames, Resolution);

        static std::unordered_map<Resolution, glm::uvec2> ResolutionToExtentsMap = {
            {Resolution::k1080p, glm::uvec2(1920, 1080)},
            {Resolution::k1440p, glm::uvec2(2560, 1440)},
            {Resolution::k4KUpscaled, glm::uvec2(3840, 2160)},
            {Resolution::k4KNative, glm::uvec2(3840, 2160)}};

        /* Final target FPS */
        enum class TargetFPS
        {
            k60,
            k120
        };

        /**
         * Rendering info is used by the GPU know the RTs, DRTs and viewport info etc.
         */
        struct RenderingInfo
        {
            Resolution                                              resolution       = Resolution::kCustom; /* Resolution preset at which the scene will be rendered at         */
            glm::uvec2                                              extent           = {0, 0};              /* Viewport extents (used only when Resolution is set to custom)    */
            std::vector<std::pair<RZTextureHandle, AttachmentInfo>> colorAttachments = {};                  /* List of attachments, texture and it's attachment info            */
            std::pair<RZTextureHandle, AttachmentInfo>              depthAttachment  = {};                  /* The depth attachment and it's info                               */
            int                                                     layerCount       = 1;                   /* Total layers to render onto, needed for gl_Layer to work         */
            bool                                                    resize           = false;               /* Whether or not to enable resizing                                */
        };

        /* Command Queue is a collection of command buffers that will be submitted for execution at once */
        typedef std::vector<RZCommandBuffer*> CommandQueue;

        /* The Razix RHI (Render Hardware Interface) provides a interface and a set of common methods that abstracts over other APIs rendering implementation
         * The Renderers creates from the provided IRZRenderer interface of Razix uses this to perform command recording/submission sets binding
         * and other operations that doesn't require the underlying API, since renderers do not actually need that we use this high-level abstraction
         * over the supported APIs to make things look simple and easier to interact with
         */
        class RAZIX_API RHI : public RZRoot
        {
        public:
            RHI()          = default;
            virtual ~RHI() = default;

            /**
             * Create the RHI instance with the window width and height
             */
            static void Create(u32 width, u32 height);
            /* Destroy RHI and release all it's resources */
            static void Release();

            /* Gets the singleton RHI handle instance */
            static RHI& Get()
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                return *s_APIInstance;
            }
            /* Gets RHI underlying pointer */
            static const RHI* GetPointer() { return s_APIInstance; }

            /* Initializes the API renderer with the resources it needs */
            RAZIX_FORCE_INLINE static void Init()
            {
                RAZIX_PROFILE_GPU_SCOPE("Init Rendering");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->InitAPIImpl();
            }
            /* Acquires a image from the back buffer to render onto */
            RAZIX_FORCE_INLINE static void AcquireImage(RZSemaphore* signalSemaphore)
            {
                RAZIX_PROFILE_GPU_SCOPE("Acquire swap image");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->AcquireImageAPIImpl(signalSemaphore);
            }
            /* Begins recording of the command buffer */
            RAZIX_FORCE_INLINE static void Begin(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Begin Rendering");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BeginAPIImpl(cmdBuffer);
            }
            /* Submits the recorded command buffers onto the RenderQueue to be submitted to the GPU for execution */
            RAZIX_FORCE_INLINE static void Submit(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Submit To Render Queue");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->SubmitImpl(cmdBuffer);
            }
            /* Executes the the lists of command buffers stacked up in the Command Queue  */
            RAZIX_FORCE_INLINE static void SubmitWork(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
            {
                RAZIX_PROFILE_GPU_SCOPE("SubmitWork");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->SubmitWorkImpl(waitSemaphores, signalSemaphores);
            }
            /* Presents the rendered images to the presentation queue for the user to view */
            RAZIX_FORCE_INLINE static void Present(RZSemaphore* waitSemaphore)
            {
                RAZIX_PROFILE_GPU_SCOPE("Present Swapchain");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->PresentAPIImpl(waitSemaphore);
            }
            /* Bind the Pipeline State Object to the command buffer */
            RAZIX_FORCE_INLINE static void BindPipeline(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Pipeline");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BindPipelineImpl(pipeline, cmdBuffer);
            }
            /* Binds a descriptor set to the given PSO at the specified set index */
            RAZIX_FORCE_INLINE static void BindDescriptorSet(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Set at Idx");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BindDescriptorSetAPImpl(pipeline, cmdBuffer, descriptorSet, setIdx);
            }
            /* Binds vector of descriptor sets to the given PSO from a starting set index */
            RAZIX_FORCE_INLINE static void BindUserDescriptorSets(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx = 0)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BindUserDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets, startSetIdx);
            }
            /* Binds array of descriptor sets to the given PSO from a starting set index */
            RAZIX_FORCE_INLINE static void BindUserDescriptorSets(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx = 0)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BindUserDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets, totalSets, startSetIdx);
            }
            /* Binds a push/root constant to the PSO */
            RAZIX_FORCE_INLINE static void BindPushConstant(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Pushconstants");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BindPushConstantsAPIImpl(pipeline, cmdBuffer, pushConstant);
            }
            /* Issue a Draw call to the GPU with vertex count and vertex data type */
            RAZIX_FORCE_INLINE static void Draw(RZCommandBuffer* cmdBuffer, u32 count, DataType dataType = DataType::UNSIGNED_INT)
            {
                RAZIX_PROFILE_GPU_SCOPE("Draw");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->DrawAPIImpl(cmdBuffer, count, dataType);
            }
            /* Issues a Indexed Draw call to the GPU with index count, instance info and vertex offset */
            RAZIX_FORCE_INLINE static void DrawIndexed(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0)
            {
                RAZIX_PROFILE_GPU_SCOPE("Draw Indexed");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->DrawIndexedAPIImpl(cmdBuffer, indexCount);
            }
            /* Resize callback function for RHI */
            RAZIX_FORCE_INLINE static void OnResize(u32 width, u32 height)
            {
                RAZIX_PROFILE_GPU_SCOPE("OnResize");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->OnResizeAPIImpl(width, height);
            }
            /* Starts a rendering pass to record commands */
            RAZIX_FORCE_INLINE static void BeginRendering(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
            {
                RAZIX_PROFILE_GPU_SCOPE("Begin Rendering");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BeginRenderingImpl(cmdBuffer, renderingInfo);
            }
            /* Ends a rendering pass with recorded commands */
            RAZIX_FORCE_INLINE static void EndRendering(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("End Rendering");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->EndRenderingImpl(cmdBuffer);
            }
            /* Enables bindless textures for the current PSO by binding the global sets */
            RAZIX_FORCE_INLINE static void EnableBindlessTextures(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Enable Bindless Textures");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->EnableBindlessTexturesImpl(pipeline, cmdBuffer);
            }
            /* Binds on-the-fly descriptors to the PSO on-demand basis */
            RAZIX_FORCE_INLINE static void BindPushDescriptors(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Push Descriptors");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->BindPushDescriptorsImpl(pipeline, cmdBuffer, descriptors);
            }
            /* Inserts a image memory barriers for a texture using pipeline barriers and memory barrier info */
            RAZIX_FORCE_INLINE static void InsertImageMemoryBarrier(RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
            {
                RAZIX_PROFILE_GPU_SCOPE("Insert Image Memory Barrier");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                s_APIInstance->InsertImageMemoryBarrierImpl(cmdBuffer, texture, pipelineBarrierInfo, imgBarrierInfo);
            }
            /* Inserts a image memory barriers for a buffer using pipeline barriers and memory barrier info */
            RAZIX_FORCE_INLINE static void InsertBufferMemoryBarrier(RZCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
            {
                RAZIX_PROFILE_GPU_SCOPE("Insert Buffer Memory Barrier");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                s_APIInstance->InsertBufferMemoryBarrierImpl(cmdBuffer, buffer, pipelineBarrierInfo, bufBarrierInfo);
            }
            /* Inserts a image memory barriers for a texture using pipeline barriers*/
            RAZIX_FORCE_INLINE static void InsertPipelineBarrier(RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo)
            {
                RAZIX_UNIMPLEMENTED_METHOD

                RAZIX_PROFILE_GPU_SCOPE("Insert Pipeline Barrier");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                //s_APIInstance->InsertPipelineBarrierImpl(cmdBuffer, texture, pipelineBarrierInfo);
            }
            /* Sets Hardware depth bias value */
            RAZIX_FORCE_INLINE static void SetDepthBias(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Set Depth Bias");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                return s_APIInstance->SetDepthBiasImpl(cmdBuffer);
            }
            /* Sets viewport rendering extents */
            RAZIX_FORCE_INLINE static void SetViewport(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
            {
                RAZIX_PROFILE_GPU_SCOPE("SetViewport");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                return s_APIInstance->SetViewportImpl(cmdBuffer, x, y, width, height);
            }
            /* Sets scissor rect rendering extents */
            RAZIX_FORCE_INLINE static void SetScissorRect(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
            {
                RAZIX_PROFILE_GPU_SCOPE("SetScissorRect");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                return s_APIInstance->SetScissorRectImpl(cmdBuffer, x, y, width, height);
            }
            /* Gets the current swapchain from the back buffer */
            RAZIX_FORCE_INLINE static RZSwapchain* GetSwapchain()
            {
                RAZIX_PROFILE_GPU_SCOPE("GetSwapchain");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                return s_APIInstance->GetSwapchainImpl();
            }
            /* Gets the current command buffer onto which we record the commands */
            RAZIX_FORCE_INLINE static RZCommandBuffer* GetCurrentCommandBuffer()
            {
                RAZIX_PROFILE_GPU_SCOPE("GetCurrentCommandBuffer");
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                return s_APIInstance->m_CurrentCommandBuffer;
            }

            /* OnImGui UI rendering for RHI */
            RAZIX_PURE_VIRTUAL(void, OnImGui)

            /* Gets the frame data descriptor set */
            RAZIX_FORCE_INLINE const RZDescriptorSet* getFrameDataSet() const { return m_FrameDataSet; }
            /* Set the frame data descriptor set */
            RAZIX_FORCE_INLINE void setFrameDataSet(RZDescriptorSet* set) { m_FrameDataSet = set; }
            /* Gets the scene light data descriptor set */
            RAZIX_FORCE_INLINE const RZDescriptorSet* getSceneLightsDataSet() const { return m_SceneLightsDataSet; }
            /* Set the scene light data descriptor set */
            RAZIX_FORCE_INLINE void setSceneLightsDataSet(RZDescriptorSet* set) { m_SceneLightsDataSet = set; }
            /* Gets the width of the current RHI rendering extents */
            RAZIX_FORCE_INLINE const u32& getWidth() { return m_Width; }
            /* Gets the height of the current RHI rendering extents */
            RAZIX_FORCE_INLINE const u32& getHeight() { return m_Height; }
            /* Set the command buffer to record command onto */
            RAZIX_DEPRECATED("SetCmdBuffer is no longer used, RHI will use a RingBufferAllocator to allocate command buffers from as needed per each frame graph render pass.")
            RAZIX_FORCE_INLINE static void SetCmdBuffer(RZCommandBuffer* cmdBuf) { s_APIInstance->m_CurrentCommandBuffer = cmdBuf; }

        protected:
            RAZIX_PURE_VIRTUAL(void, InitAPIImpl)
            RAZIX_PURE_VIRTUAL(void, AcquireImageAPIImpl, RZSemaphore* signalSemaphore)
            RAZIX_PURE_VIRTUAL(void, BeginAPIImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_PURE_VIRTUAL(void, SubmitImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_PURE_VIRTUAL(void, SubmitWorkImpl, std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
            RAZIX_PURE_VIRTUAL(void, PresentAPIImpl, RZSemaphore* waitSemaphore)
            RAZIX_PURE_VIRTUAL(void, BindPipelineImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
            RAZIX_PURE_VIRTUAL(void, BindDescriptorSetAPImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
            RAZIX_PURE_VIRTUAL(void, BindUserDescriptorSetsAPImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
            RAZIX_PURE_VIRTUAL(void, BindUserDescriptorSetsAPImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
            RAZIX_PURE_VIRTUAL(void, BindPushConstantsAPIImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
            RAZIX_PURE_VIRTUAL(void, DrawAPIImpl, RZCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT)
            RAZIX_PURE_VIRTUAL(void, DrawIndexedAPIImpl, RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0)
            RAZIX_PURE_VIRTUAL(void, DestroyAPIImpl)
            RAZIX_PURE_VIRTUAL(void, OnResizeAPIImpl, u32 width, u32 height)
            RAZIX_PURE_VIRTUAL(void, SetDepthBiasImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_PURE_VIRTUAL(void, SetScissorRectImpl, RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
            RAZIX_PURE_VIRTUAL(void, SetViewportImpl, RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
            RAZIX_PURE_VIRTUAL(void, EnableBindlessTexturesImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
            RAZIX_PURE_VIRTUAL(void, BindPushDescriptorsImpl, RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
            RAZIX_PURE_VIRTUAL(void, BeginRenderingImpl, RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
            RAZIX_PURE_VIRTUAL(void, EndRenderingImpl, RZCommandBuffer* cmdBuffer)
            RAZIX_PURE_VIRTUAL(void, InsertImageMemoryBarrierImpl, RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
            RAZIX_PURE_VIRTUAL(void, InsertBufferMemoryBarrierImpl, RZCommandBuffer* cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
            RAZIX_PURE_VIRTUAL(RZSwapchain*, GetSwapchainImpl);

        protected:
            static RHI* s_APIInstance;

            std::string                   m_RendererTitle; /* The name of the renderer API that is being used */
            u32                           m_Width      = 0;
            u32                           m_Height     = 0;
            u32                           m_PrevWidth  = 0;
            u32                           m_PrevHeight = 0;
            CommandQueue                  m_CommandQueue; /* The queue of recorded commands that needs execution */
            RZCommandBuffer*              m_CurrentCommandBuffer = nullptr;
            std::vector<RZCommandBuffer*> m_DrawCommandBuffers;
            RZDescriptorSet*              m_FrameDataSet       = nullptr;
            RZDescriptorSet*              m_SceneLightsDataSet = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix
