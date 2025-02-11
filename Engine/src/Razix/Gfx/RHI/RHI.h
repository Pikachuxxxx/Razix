#pragma once

#include <tracy/Tracy.hpp>

#include "RZSTL/ring_buffer.h"

#include "Razix/Core/Profiling/RZProfiling.h"

#include "Razix/Gfx/GfxData.h"

namespace Razix {
    namespace Gfx {

        class RZDrawCommandBuffer;
        class RZPipeline;
        class RZDescriptorSet;
        class RZSwapchain;
        class RZDescriptorSet;
        class RZSemaphore;
        struct RZPushConstant;

        // TODO: Compile this into a C dynamic library that Razix.dll links with
        // TODO: Move Platform/API/Vulkan and DirectX12 to here and name it RHI_Backend and compile RHI as one C11 big library
        // https://github.com/Pikachuxxxx/Razix/issues/384

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

            static RHI&       Get();
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
            static void Draw(RZDrawCommandBufferHandle cmdBuffer, u32 count, DrawDataType dataType = DrawDataType::UNSIGNED_INT);
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
            static void           CopyTextureResource(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle);
            static TextureReadback InsertTextureReadback(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture);

            // Pipeline
            static void SetDepthBias(RZDrawCommandBufferHandle cmdBuffer);
            static void SetViewport(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height);
            static void SetScissorRect(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height);

            // Misc
            // FIXME: Don't expose this at all, write simple wrappers, we hardly need the current image index or the swap image RTV
            static RZSwapchain* GetSwapchain();
            /* Returns the current draw command buffer to record onto from a ring buffer */
            static RZDrawCommandBufferHandle GetCurrentCommandBuffer();

            // TODO: Move this to Diana or RZScene
            inline RZDescriptorSetHandle       getFrameDataSet() const { return m_FrameDataSet; }
            inline void                        setFrameDataSet(RZDescriptorSetHandle set) { m_FrameDataSet = set; }
            inline const RZDescriptorSetHandle getSceneLightsDataSet() const { return m_SceneLightsDataSet; }
            inline void                        setSceneLightsDataSet(RZDescriptorSetHandle set) { m_SceneLightsDataSet = set; }
            inline const u32&                  getWidth() { return m_Width; }
            inline const u32&                  getHeight() { return m_Height; }

            virtual void OnImGui() = 0;

        protected:
            virtual void           InitAPIImpl()                                                                                                                                                                     = 0;
            virtual void           AcquireImageAPIImpl(RZSemaphore* signalSemaphore)                                                                                                                                 = 0;
            virtual void           BeginAPIImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                                 = 0;
            virtual void           SubmitImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                                   = 0;
            virtual void           PresentAPIImpl(RZSemaphore* waitSemaphore)                                                                                                                                        = 0;
            virtual void           BindPipelineImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)                                                                                                  = 0;
            virtual void           BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZDescriptorSetHandle descriptorSet, u32 setIdx)                                          = 0;
            virtual void           BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptorSetHandle>& descriptorSets, u32 startSetIdx)           = 0;
            virtual void           BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, RZPushConstant pushConstant)                                                             = 0;
            virtual void           DrawAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 count, DrawDataType datayType = DrawDataType::UNSIGNED_INT)                                                                  = 0;
            virtual void           DrawIndexedAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0)               = 0;
            virtual void           DispatchAPIImpl(RZDrawCommandBufferHandle cmdBuffer, u32 groupX, u32 groupY, u32 groupZ)                                                                                          = 0;
            virtual void           DestroyAPIImpl()                                                                                                                                                                  = 0;
            virtual void           OnResizeAPIImpl(u32 width, u32 height)                                                                                                                                            = 0;
            virtual void           SetDepthBiasImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                             = 0;
            virtual void           SetScissorRectImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)                                                                              = 0;
            virtual void           SetViewportImpl(RZDrawCommandBufferHandle cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)                                                                                 = 0;
            virtual void           EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer)                                                                                        = 0;
            virtual void           BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZDrawCommandBufferHandle cmdBuffer, const std::vector<RZDescriptor>& descriptors)                                             = 0;
            virtual void           BeginRenderingImpl(RZDrawCommandBufferHandle cmdBuffer, const RenderingInfo& renderingInfo)                                                                                       = 0;
            virtual void           EndRenderingImpl(RZDrawCommandBufferHandle cmdBuffer)                                                                                                                             = 0;
            virtual void           InsertImageMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)        = 0;
            virtual void           InsertBufferMemoryBarrierImpl(RZDrawCommandBufferHandle cmdBuffer, RZUniformBufferHandle buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo) = 0;
            virtual void           CopyTextureResourceImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle dstTexture, RZTextureHandle srcTextureHandle)                                                        = 0;
            virtual TextureReadback InsertTextureReadbackImpl(RZDrawCommandBufferHandle cmdBuffer, RZTextureHandle texture)                                                                                           = 0;
            virtual RZSwapchain*   GetSwapchainImpl()                                                                                                                                                                = 0;

        protected:
            static RHI* s_APIInstance;

            std::string  m_RendererTitle; /* The name of the renderer API that is being used */
            u32          m_Width      = 0;
            u32          m_Height     = 0;
            u32          m_PrevWidth  = 0;
            u32          m_PrevHeight = 0;
            CommandQueue m_GraphicsCommandQueue; /* The queue of recorded commands that needs execution */
            // TODO: Move all these to Diana, these setups are internally customized by the high-level renderer, only util functions exist here no low-level logic must exist

            RZDrawCommandBufferHandle              m_CurrentCommandBuffer;
            std::vector<RZDrawCommandBufferHandle> m_DrawCommandBuffers;
            std::vector<RZCommandPoolHandle>       m_GraphicsCommandPool;
            std::vector<RZCommandPoolHandle>       m_ComputeCommandPool;
            RZDescriptorSetHandle                  m_FrameDataSet       = {};
            RZDescriptorSetHandle                  m_SceneLightsDataSet = {};
        };
    }    // namespace Gfx
}    // namespace Razix
