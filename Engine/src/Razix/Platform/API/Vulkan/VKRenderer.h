#pragma once

#include "Razix/Graphics/API/RZAPIRenderer.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

namespace Razix {
    namespace Graphics {
        
        class VKRenderer : public RZAPIRenderer
        {
        public:
            VKRenderer(uint32_t width, uint32_t height);
            ~VKRenderer();

            static VKRenderer* GetVKRenderer() { return static_cast<VKRenderer*>(s_APIInstance); }

            inline const VkDescriptorPool& getDescriptorPool() const { return m_DescriptorPool; }

        protected:
            void InitAPIImpl() override;
            void BeginAPIImpl() override;
            void PresentAPIImple(RZCommandBuffer* cmdBuffer) override;
            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) override;
            void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, uint32_t start = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(uint32_t width, uint32_t height) override;

            RZSwapchain* GetSwapchainImpl() override;


            void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer) override;


            void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) override;


            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets) override;

        private:
            VKContext* m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
            VkDescriptorSet m_DescriptorSetPool[16];
            VkDescriptorPool m_DescriptorPool;

        };
    }
}

