#include "rzxpch.h"
#include "VKCommandBuffer.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKCommandBuffer::VKCommandBuffer()
            : m_CommandBuffer(VK_NULL_HANDLE), m_CommandPool(VK_NULL_HANDLE)
        { }

        VKCommandBuffer::~VKCommandBuffer()
        {
            Reset();
        }

        void VKCommandBuffer::Init()
        {
            VkCommandBufferAllocateInfo cmdBufferCI = {};

            m_CommandPool = VKDevice::Get().getCommandPool()->getVKPool();

            cmdBufferCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferCI.commandBufferCount = 1;
            cmdBufferCI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufferCI.commandPool = m_CommandPool;

            VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().getDevice(), &cmdBufferCI, &m_CommandBuffer));
        }

        void VKCommandBuffer::BeginRecording()
        {
            VkCommandBufferBeginInfo beginCI{};
            beginCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginCI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginCI));
        }

        void VKCommandBuffer::EndRecording()
        {
            VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));
        }

        void VKCommandBuffer::Execute()
        {
            VkSubmitInfo submitInfo = {};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext                = VK_NULL_HANDLE;
            submitInfo.waitSemaphoreCount   = 0;
            submitInfo.pWaitSemaphores      = nullptr;
            submitInfo.pWaitDstStageMask    = nullptr;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &m_CommandBuffer;
            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores    = nullptr;

            VK_CHECK_RESULT(vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
        }

        void VKCommandBuffer::Reset()
        {
            vkFreeCommandBuffers(VKDevice::Get().getDevice(), m_CommandPool, 1, &m_CommandBuffer);
        }

        void VKCommandBuffer::UpdateViewport(uint32_t width, uint32_t height)
        {
            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(width);
            viewport.height = static_cast<float>(height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor = {};
            scissor.offset = { 0, 0 };
            scissor.extent = { width, height };

            vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
        }
    }
}