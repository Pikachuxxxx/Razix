// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKBuffer.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "VKDevice.h"

#include "VKUtilities.h"

#include "vulkan/vulkan_core.h"

namespace Razix {
    namespace Graphics {

        VKBuffer::VKBuffer(BufferUsage usage, u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
            : m_Usage(usage), m_BufferSize(size)
        {
            // Based on Usage set some vulkan usage flags

            switch (m_Usage) {
                case BufferUsage::Static:
                    break;
                case BufferUsage::Dynamic:
                    break;
                case BufferUsage::PersistentStream:
                    break;
                case BufferUsage::Staging:
                    break;
                case BufferUsage::IndirectDrawArgs:
                    break;
            }

            init(data RZ_DEBUG_E_ARG_NAME);
        }

        VKBuffer::VKBuffer()
            : m_UsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT), m_BufferSize(0)
        {
        }

        void VKBuffer::destroy()
        {
            if (m_Buffer != VK_NULL_HANDLE) {
#ifndef RAZIX_USE_VMA
                if (m_BufferMemory) {
                    vkFreeMemory(VKDevice::Get().getDevice(), m_BufferMemory, nullptr);
                }
                vkDestroyBuffer(VKDevice::Get().getDevice(), m_Buffer, nullptr);
#else
                vmaDestroyBuffer(VKDevice::Get().getVMA(), m_Buffer, m_VMAAllocation);
#endif
                m_Mapped = nullptr;
                //free(m_Mapped);
            }
        }

        void VKBuffer::map(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            VkResult res;
            // Map the memory to the mapped buffer
#ifndef RAZIX_USE_VMA
            res = vkMapMemory(VKDevice::Get().getDevice(), m_BufferMemory, offset, size, 0, &m_Mapped);
            RAZIX_CORE_ASSERT((res == VK_SUCCESS), "[Vulkan] Failed to map buffer!");
#else
            res = vmaMapMemory(VKDevice::Get().getVMA(), m_VMAAllocation, &m_Mapped);
#endif
            RAZIX_CORE_ASSERT((res == VK_SUCCESS), "[VMA] Failed to map buffer!");
        }

        void VKBuffer::unMap()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            if (m_Mapped) {
#ifndef RAZIX_USE_VMA
                vkUnmapMemory(VKDevice::Get().getDevice(), m_BufferMemory);
#else
                vmaUnmapMemory(VKDevice::Get().getVMA(), m_VMAAllocation);
#endif
                //free(m_Mapped);
                //delete m_Mapped;
                m_Mapped = nullptr;
            }
        }

        void VKBuffer::flush(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

#ifndef RAZIX_USE_VMA
            // Flush only if the buffer exists
            if (m_Buffer) {
                VkMappedMemoryRange mappedRange = {};
                mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
                mappedRange.memory              = m_BufferMemory;
                mappedRange.offset              = offset;
                mappedRange.size                = size;
                vkFlushMappedMemoryRanges(VKDevice::Get().getDevice(), 1, &mappedRange);
            }
#else
            vmaFlushAllocation(VKDevice::Get().getVMA(), m_VMAAllocation, offset, size);
#endif
        }

        void VKBuffer::setData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            map(size, 0);
            memcpy(m_Mapped, data, size);
            unMap();
        }

        void VKBuffer::resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            //destroy();
            m_BufferSize = size;
            destroy();

            // TODO: Find a better solution for this
            init(data RZ_DEBUG_E_ARG_NAME);
        }

        void VKBuffer::init(const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size               = m_BufferSize;
            bufferInfo.usage              = m_UsageFlags;
            bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

#ifndef RAZIX_USE_VMA
            // Create the buffer
            VK_CHECK_RESULT(vkCreateBuffer(VKDevice::Get().getDevice(), &bufferInfo, nullptr, &m_Buffer));

            // Get the memory requirements and allocate actual memory for the buffer and Bind it!
            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(VKDevice::Get().getDevice(), m_Buffer, &memRequirements);

            // Allocate the buffer
            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize       = memRequirements.size;
            allocInfo.memoryTypeIndex      = VKDevice::Get().getPhysicalDevice().get()->getMemoryTypeIndex(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            VK_CHECK_RESULT(vkAllocateMemory(VKDevice::Get().getDevice(), &allocInfo, nullptr, &m_BufferMemory));

            // Bind the buffer to it's memory
            vkBindBufferMemory(VKDevice::Get().getDevice(), m_Buffer, m_BufferMemory, 0);

            VK_TAG_OBJECT(bufferName + std::string("[Memory]"), VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t) m_BufferMemory);
#else
            VmaAllocationInfo allocationInfo{};

            VmaAllocationCreateInfo vmaallocInfo = {};

            //VmaMemoryUsage usage{};
            //usage = VMA_MEMORY_USAGE_AUTO;
            //if (m_UsageFlags ==)

            vmaallocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            vmaallocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            //allocate the buffer
            VK_CHECK_RESULT(vmaCreateBuffer(VKDevice::Get().getVMA(), &bufferInfo, &vmaallocInfo, &m_Buffer, &m_VMAAllocation, &allocationInfo));

    #ifdef RAZIX_DEBUG
            vmaSetAllocationName(VKDevice::Get().getVMA(), m_VMAAllocation RZ_DEBUG_E_ARG_NAME.c_str());
    #endif
            // TODO: Get allocated memory stats from which pool etc. and attach that to RZMemoryManager
            //Memory::RZMemAllocInfo memAllocInfo{};
#endif

            //! Set the Data
            if (data != nullptr)
                setData((u32) m_BufferSize, data);

            VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_BUFFER, (uint64_t) m_Buffer);
        }
    }    // namespace Graphics
}    // namespace Razix