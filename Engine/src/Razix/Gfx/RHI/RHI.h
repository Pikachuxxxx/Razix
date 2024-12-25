#pragma once

#include <tracy/Tracy.hpp>

#include "RZSTL/ring_buffer.h"

#include "Razix/Core/Profiling/RZProfiling.h"

#include "Razix/Gfx/RHI/API/RZAPIHandles.h"
#include "Razix/Gfx/RHI/API/RZBarriers.h"
#include "Razix/Gfx/RHI/API/RZBindingInfoAccessViews.h"
#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Gfx {

        class RZDrawCommandBuffer;
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
            k60  = 60,
            k120 = 120
        };

        /**
         * Rendering info is used by the GPU know the RTs, DRTs and viewport info etc.
         */
        struct RenderingInfo
        {
            Resolution                                                          resolution       = Resolution::kCustom; /* Resolution preset at which the scene will be rendered at         */
            glm::uvec2                                                          extent           = {0, 0};              /* Viewport extents (used only when Resolution is set to custom)    */
            std::vector<std::pair<RZTextureHandle, RenderTargetAttachmentInfo>> colorAttachments = {};                  /* List of attachments, texture and it's attachment info            */
            std::pair<RZTextureHandle, RenderTargetAttachmentInfo>              depthAttachment  = {};                  /* The depth attachment and it's info                               */
            int                                                                 layerCount       = 1;                   /* Total layers to render onto, needed for gl_Layer to work         */
            bool                                                                resize           = false;               /* Whether or not to enable resizing                                */
        };

        /* Command Queue is a collection of command buffers that will be submitted for execution at once */
        typedef std::vector<RZDrawCommandBufferHandle> CommandQueue;

        /* The Razix RHI (Render Hardware Interface) provides a interface and a set of common methods that abstracts over other APIs rendering implementation
         * The Renderers creates from the provided IRZRenderer interface of Razix uses this to perform command recording/submission sets binding
         * and other operations that doesn't require the underlying API, since renderers do not actually need that we use this high-level abstraction
         * over the supported APIs to make things look simple and easier to interact with
         */
        class RAZIX_API RHI
        {
        public:
            RHI()          = default;
            virtual ~RHI() = default;

            // Lifecycle Management
            static void Create(u32 width, u32 height);
            static void Destroy();

            static RHI& Get();
            static const RHI* GetPointer();

            static void Init();
            static void OnResize(u32 width, u32 height);

            // Command Recording & Submission
            static void
                        AcquireImage(RZSemaphore* signalSemaphore);
            static void Begin(RZDrawCommandBufferHandle cmdBuffer);
            static void Submit(RZDrawCommandBufferHandle cmdBuffer);
            static void Present(RZSemaphore* waitSemaphore);

            // Binding
            static void BindPipeline(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer);
            static void BindDescriptorSet(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZDescriptorSetHandle descriptorSet, u32 setIdx);
            static void BindUserDescriptorSets(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSetHandle>& descriptorSets, u32 startSetIdx = 0);
            static void BindPushConstant(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant);
            static void EnableBindlessTextures(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer);

            // Draws & Dispatches
            static void Draw(RZDrawCommandBufferHandle cmdBuffer, u32 count, DataType dataType = DataType::UNSIGNED_INT);
            static void DrawIndexed(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0);
            // TODO: Use AsyncCommandBufferHandle
            static void Dispatch(RZDrawCommandBufferHandle cmdBuffer, u32 groupX, u32 groupY, u32 groupZ);

            // Render Pass
            static void BeginRendering(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo);
            static void EndRendering(RZDrawCommandBufferHandle cmdBuffer);

            // Memory Barriers
            static void InsertImageMemoryBarrier(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo);
            static void InsertBufferMemoryBarrier(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo);
            // TODO:  static void InsertPipelineBarrier(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo);

            // Resource Management
            static void CopyTextureResource(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle);

            // Pipeline
            static void SetDepthBias(RZDrawCommandBufferHandle cmdBuffer);
            static void SetViewport(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height);
            static void SetScissorRect(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height);

            // Misc
            static RZSwapchain*              GetSwapchain();
            static RZDrawCommandBufferHandle GetCurrentCommandBuffer();

            inline RZDescriptorSetHandle       getFrameDataSet() const { return m_FrameDataSet; }
            inline void                        setFrameDataSet(RZDescriptorSetHandle set) { m_FrameDataSet = set; }
            inline const RZDescriptorSetHandle getSceneLightsDataSet() const { return m_SceneLightsDataSet; }
            inline void                        setSceneLightsDataSet(RZDescriptorSetHandle set) { m_SceneLightsDataSet = set; }
            inline const u32&                  getWidth() { return m_Width; }
            inline const u32&                  getHeight() { return m_Height; }

            virtual void OnImGui() = 0;

        protected:
            virtual void         InitAPIImpl()                                                                                                                                                                     = 0;
            virtual void         AcquireImageAPIImpl(RZSemaphore* signalSemaphore)                                                                                                                                 = 0;
            virtual void         BeginAPIImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                                 = 0;
            virtual void         SubmitImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                                   = 0;
            virtual void         PresentAPIImpl(RZSemaphore* waitSemaphore)                                                                                                                                        = 0;
            virtual void         BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)                                                                                                  = 0;
            virtual void         BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZDescriptorSetHandle descriptorSet, u32 setIdx)                                          = 0;
            virtual void         BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSetHandle>& descriptorSets, u32 startSetIdx)           = 0;
            virtual void         BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant)                                                             = 0;
            virtual void         DrawAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT)                                                                          = 0;
            virtual void         DrawIndexedAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0)               = 0;
            virtual void         DispatchAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 groupX, u32 groupY, u32 groupZ)                                                                                          = 0;
            virtual void         DestroyAPIImpl()                                                                                                                                                                  = 0;
            virtual void         OnResizeAPIImpl(u32 width, u32 height)                                                                                                                                            = 0;
            virtual void         SetDepthBiasImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                             = 0;
            virtual void         SetScissorRectImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)                                                                              = 0;
            virtual void         SetViewportImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)                                                                                 = 0;
            virtual void         EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)                                                                                        = 0;
            virtual void         BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptor>& descriptors)                                             = 0;
            virtual void         BeginRenderingImpl(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo)                                                                                       = 0;
            virtual void         EndRenderingImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                             = 0;
            virtual void         InsertImageMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)        = 0;
            virtual void         InsertBufferMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) = 0;
            virtual void         CopyTextureResourceImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)                                                        = 0;
            virtual RZSwapchain* GetSwapchainImpl()                                                                                                                                                                = 0;

        protected:
            static RHI* s_APIInstance;

            std::string                            m_RendererTitle; /* The name of the renderer API that is being used */
            u32                                    m_Width      = 0;
            u32                                    m_Height     = 0;
            u32                                    m_PrevWidth  = 0;
            u32                                    m_PrevHeight = 0;
            CommandQueue                           m_GraphicsCommandQueue; /* The queue of recorded commands that needs execution */
            RZDrawCommandBufferHandle              m_CurrentCommandBuffer;
            std::vector<RZDrawCommandBufferHandle> m_DrawCommandBuffers;
            std::vector<RZCommandPoolHandle>       m_GraphicsCommandPool;
            std::vector<RZCommandPoolHandle>       m_ComputeCommandPool;
            RZDescriptorSetHandle                  m_FrameDataSet       = {};
            RZDescriptorSetHandle                  m_SceneLightsDataSet = {};
        };
    }    // namespace Gfx
}    // namespace Razix
