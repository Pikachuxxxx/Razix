#pragma once

#include "Razix/Core/RZSmartPointers.h"
#include "Razix/Graphics/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        class RZCommandBuffer;
        class RZPipeline;
        class RZDescriptorSet;
        class RZSwapchain;
        class RZPushConstant;

        enum class DataType
        {
            FLOAT,
            UNSIGNED_INT,
            UNSIGNED_BYTE
        };

        /* The Razix API Renderer provides a interface and a set of common methods that abstracts over other APIs rendering implementation
         * The Renderers creates from the provided IRZRenderer interface of razix uses this to perform command recording/submission sets binding
         * and other operations that doesn't require the underlying API, since renderers do not actually need that we use this high-level abstraction
         * over the supported APIs to make things look simple and easier to interact with
         */
        class RAZIX_API RZAPIRenderer
        {
        public:
            RZAPIRenderer() = default;
            virtual ~RZAPIRenderer() = default;

            static void Create(uint32_t width, uint32_t height);
            static void Release();

            RAZIX_FORCE_INLINE static void Init() { s_APIInstance->InitAPIImpl(); }
            RAZIX_FORCE_INLINE static void Begin() { s_APIInstance->BeginAPIImpl(); }
            RAZIX_FORCE_INLINE static void Present(RZCommandBuffer* cmdBuffer) { s_APIInstance->PresentAPIImple(cmdBuffer); }
            RAZIX_FORCE_INLINE static void BindDescriptorSets(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets, uint32_t setIndex) { s_APIInstance->BindDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets); }
            RAZIX_FORCE_INLINE static void BindPushConstants(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer) { s_APIInstance->BindPushConstantsAPIImpl(pipeline, cmdBuffer); }
            RAZIX_FORCE_INLINE static void Draw(RZCommandBuffer* cmdBuffer, uint32_t count, DataType dataType = DataType::UNSIGNED_INT) { s_APIInstance->DrawAPIImpl(cmdBuffer, count, dataType); }
            RAZIX_FORCE_INLINE static void DrawIndexed(RZCommandBuffer* cmdBuffer, uint32_t count, uint32_t start = 0) { s_APIInstance->DrawIndexedAPIImpl(cmdBuffer, count, start); }

            RAZIX_FORCE_INLINE static void OnResize(uint32_t width, uint32_t height) { s_APIInstance->OnResizeAPIImpl(width, height); }

            RAZIX_FORCE_INLINE static RZSwapchain* getSwapchain() { return s_APIInstance->GetSwapchainImpl(); }

            RAZIX_FORCE_INLINE static void SetDepthBias(RZCommandBuffer* cmdBuffer) { return s_APIInstance->SetDepthBiasImpl(cmdBuffer); }

        protected:
            virtual void InitAPIImpl() = 0;
            virtual void BeginAPIImpl() = 0;
            virtual void PresentAPIImple(RZCommandBuffer* cmdBuffer) = 0;
            virtual void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) = 0;
            virtual void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer) = 0;
            virtual void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT) = 0;
            virtual void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, uint32_t start = 0) = 0;
            virtual void DestroyAPIImpl() = 0;

            virtual void OnResizeAPIImpl(uint32_t width, uint32_t height) = 0;

            virtual RZSwapchain* GetSwapchainImpl() = 0;

            virtual void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) = 0;
        protected:
            static RZAPIRenderer*   s_APIInstance;

            std::string             m_RendererTitle;    /* The name of the renderer API that is being used */
            uint32_t                m_Width;
            uint32_t                m_Height;
        };
    }
}
