#include "rzxpch.h"
#include "VKRenderer.h"

#include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#include "Razix/Platform/API/Vulkan/VKPipeline.h"

namespace Razix {
    namespace Graphics {

        VKRenderer::VKRenderer(uint32_t width, uint32_t height)
        {
            m_RendererTitle = "Vulkan";
            m_Width = width;
            m_Height = height;

            // Create any extra descriptor pools here such as for ImGui and other needs
        }

        VKRenderer::~VKRenderer()
        {
            //m_Context->Release();
        }

        void VKRenderer::InitAPIImpl()
        {
            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = VKContext::Get();
        }

        void VKRenderer::BeginAPIImpl()
        {
            // Get the next image to present
            m_Context->getSwapchain()->acquireNextImage();
            // Begin recording to the command buffer
            m_Context->getSwapchain()->begin();
        }

        void VKRenderer::PresentAPIImple(RZCommandBuffer* cmdBuffer)
        {
            m_Context->getSwapchain()->end();
            m_Context->getSwapchain()->queueSubmit();
            m_Context->getSwapchain()->present();
        }

        void VKRenderer::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)
        {
            uint32_t numDynamicDescriptorSets = 0;
            uint32_t numDesciptorSets = 0;

            for (auto descriptorSet : descriptorSets) {
                if (descriptorSet) {
                    auto vkDescSet = static_cast<VKDescriptorSet*>(descriptorSet);


                }
            }

            vkCmdBindDescriptorSets(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pipeline)->getPipelineLayout(), 0, numDesciptorSets, )

        }

        void VKRenderer::DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }
}
