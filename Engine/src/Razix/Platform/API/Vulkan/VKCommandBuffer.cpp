// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKCommandBuffer.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

namespace Razix {
    namespace Graphics {

        VKCommandBuffer::VKCommandBuffer()
            : m_CommandBuffer(VK_NULL_HANDLE), m_CommandPool(VK_NULL_HANDLE)

        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_State = CommandBufferState::Idle;
        }

        VKCommandBuffer::VKCommandBuffer(VkCommandBuffer vulkanHandle)
            : m_CommandBuffer(vulkanHandle)
        {
        }

        VKCommandBuffer::~VKCommandBuffer()
        {
            //Reset();
        }

        void VKCommandBuffer::Init(RZ_DEBUG_NAME_TAG_S_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            VkCommandBufferAllocateInfo cmdBufferCI = {};

            m_CommandPool = VKDevice::Get().getCommandPool()->getVKPool();

            cmdBufferCI.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferCI.commandBufferCount = 1;
            cmdBufferCI.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufferCI.commandPool        = m_CommandPool;

            VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().getDevice(), &cmdBufferCI, &m_CommandBuffer));

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t) m_CommandBuffer)
        }

        //void VKCommandBuffer::Init(VkCommandPool cmdPool /*= VK_NULL_HANDLE*/)
        //{
        //    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);
        //
        //    VkCommandBufferAllocateInfo cmdBufferCI = {};
        //
        //    m_CommandPool = cmdPool;
        //
        //    cmdBufferCI.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        //    cmdBufferCI.commandBufferCount = 1;
        //    cmdBufferCI.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        //    cmdBufferCI.commandPool        = m_CommandPool;
        //
        //    VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().getDevice(), &cmdBufferCI, &m_CommandBuffer));
        //
        //    VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t) m_CommandBuffer)
        //}

        void VKCommandBuffer::BeginRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            m_State = CommandBufferState::Recording;
            VkCommandBufferBeginInfo beginCI{};
            beginCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginCI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginCI));
        }

        void VKCommandBuffer::EndRecording()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            RAZIX_ASSERT(m_State == CommandBufferState::Recording, "CommandBuffer ended before started recording");
            VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));
            m_State = CommandBufferState::Ended;
        }

        void VKCommandBuffer::Execute()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            RAZIX_ASSERT(m_State == CommandBufferState::Ended, "CommandBuffer executed before ended recording");
            // TODO: Attach to the synchronization primitives
            VkSubmitInfo submitInfo         = {};
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
            m_State = CommandBufferState::Submitted;
        }

        void VKCommandBuffer::Reset()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            vkFreeCommandBuffers(VKDevice::Get().getDevice(), m_CommandPool, 1, &m_CommandBuffer);
        }

        /*
        void VKCommandBuffer::Draw(u32 verticexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
        {
            vkCmdDraw(m_CommandBuffer, verticexCount, instanceCount, firstInstance, firstInstance);
        } 

        void VKCommandBuffer::DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, int32_t vertexOffset, u32 firstInstance)
        {
            vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstInstance, vertexOffset, firstInstance);
        }
        */
    }    // namespace Graphics
}    // namespace Razix