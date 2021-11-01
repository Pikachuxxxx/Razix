#include "rzxpch.h"
#include "VKBuffer.h"

#include "VKDevice.h"

#include "VKUtilities.h"

namespace Razix {
    namespace Graphics {
    
    
        VKBuffer::VKBuffer(VkBufferUsageFlags usage, uint32_t size, const void* data)
            : m_UsageFlags(usage), m_Size(size)
        {
            init(data);
        }

        VKBuffer::VKBuffer()
            : m_UsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT), m_Size(0) { }

        VKBuffer::~VKBuffer()
        {
            if (m_Buffer) {
                vkDestroyBuffer(VKDevice::Get().getDevice(), m_Buffer, nullptr);

                if (m_Memory) {
                    vkFreeMemory(VKDevice::Get().getDevice(), m_Memory, nullptr);
                }
            }
        }

        void VKBuffer::map(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            // Map the memory to the mapped buffer
            VkResult res = vkMapMemory(VKDevice::Get().getDevice(), m_Memory, offset, size, 0, &m_Mapped);
            RAZIX_CORE_ASSERT((res == VK_SUCCESS), "[Vulkan] Failed to map buffer!");
        }

        void VKBuffer::unMap()
        {
            if (m_Mapped) {
                vkUnmapMemory(VKDevice::Get().getDevice(), m_Memory);
                m_Mapped = nullptr;
            }
        }

        void VKBuffer::flush(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.memory = m_Memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            vkFlushMappedMemoryRanges(VKDevice::Get().getDevice(), 1, &mappedRange);
        }

        void VKBuffer::setData(uint32_t size, const void* data)
        {
            map(size, 0);
            memcpy(m_Mapped, data, size);
            unMap();
        }

        void VKBuffer::init(const void* data)
        {
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = m_Size;
            bufferInfo.usage = m_UsageFlags;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            // Create the buffer
            VK_CHECK_RESULT(vkCreateBuffer(VKDevice::Get().getDevice(), &bufferInfo, nullptr, &m_Buffer));

            // Get the memory requirements and allocate actual memory for the buffer and Bind it!
            VK_CHECK_RESULT(vkCreateBuffer(VKDevice::Get().getDevice(), &bufferInfo, nullptr, &m_Buffer));

            // Get the memory requirements
            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(VKDevice::Get().getDevice(), m_Buffer, &memRequirements);

            // Allocate the buffer
            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = VKDevice::Get().getPhysicalDevice().get()->getMemoryTypeIndex(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            VK_CHECK_RESULT(vkAllocateMemory(VKDevice::Get().getDevice(), &allocInfo, nullptr, &m_Memory));

            // Bind the buffer to it's memory
            vkBindBufferMemory(VKDevice::Get().getDevice(), m_Buffer, m_Memory, 0);

            //! Set the Data
            if (data != nullptr)
                setData((uint32_t)m_Size, data);
        }
    }
}