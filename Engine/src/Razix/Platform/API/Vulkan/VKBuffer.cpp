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

        VKBuffer::VKBuffer(BufferUsage usage, VkBufferUsageFlags usageFlags, u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
            : m_Usage(usage), m_UsageFlags(usageFlags), m_BufferSize(size)
        {
            // Based on Usage set some vulkan usage flags
            // [Source] : https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/usage_patterns.html#usage_patterns_advanced_data_uploading

            switch (m_Usage) {
                case BufferUsage::Static: {
                    /**
                     * Static buffers are for GPU resident only memory! 
                     * Used for static Constant Buffers, Storage buffers for GPU<->GPU only copy etc.
                     * 
                     * Data can be copied from GPU or via using a staging buffer during init stage
                     */
                    m_VMAAllocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
                    m_UsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                }
                case BufferUsage::Staging:
                    /**
                     * Staging buffers are updated at random intervals from CPU side
                     * it is mappable to the user on CPU
                     * Used for dynamic constant buffers/vertex buffers ex. Material Data mostly 
                     * We don't' need a persistent mapping, hence we will use vmaMapMemory functions in this case
                     */
                    m_VMAAllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                    m_UsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                    break;
                case BufferUsage::PersistentStream:
                    /**
                     * We map it on creation itself
                     */
                    m_VMAAllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
                    m_UsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                    break;
                case BufferUsage::IndirectDrawArgs:
                    /**
                     * Everything here is written by GPU (no initial data from user side is allowed!)
                     */
                    m_VMAAllocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
                    m_UsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                    break;
                case BufferUsage::ReadBack:
                    m_VMAAllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
                    break;
            }

            init(data RZ_DEBUG_E_ARG_NAME);
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
                unMap();
                vmaDestroyBuffer(VKDevice::Get().getVMA(), m_Buffer, m_VMAAllocation);
#endif
                m_Mapped = nullptr;
            }
        }

        void VKBuffer::map(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            VkResult res;
            // Map the memory to the mapped buffer
#ifndef RAZIX_USE_VMA
            if (!m_Mapped) {
                res = vkMapMemory(VKDevice::Get().getDevice(), m_BufferMemory, offset, size, 0, &m_Mapped);
                RAZIX_CORE_ASSERT((res == VK_SUCCESS), "[Vulkan] Failed to map buffer!");
            }
#else
            if (m_Usage == BufferUsage::Staging) {
                res = vmaMapMemory(VKDevice::Get().getVMA(), m_VMAAllocation, &m_Mapped);
                RAZIX_CORE_ASSERT((res == VK_SUCCESS), "[VMA] Failed to map buffer!");
            }
            // If it's persistent mapped we did at creation stage itself!
#endif
        }

        void VKBuffer::unMap()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

#ifndef RAZIX_USE_VMA
            if (m_Mapped) {
                vkUnmapMemory(VKDevice::Get().getDevice(), m_BufferMemory);
            }
#else
            if (m_Usage == BufferUsage::Staging)
                vmaUnmapMemory(VKDevice::Get().getVMA(), m_VMAAllocation);
#endif
            m_Mapped = nullptr;
        }

        void VKBuffer::flush(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

#ifndef RAZIX_USE_VMA
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory              = m_BufferMemory;
            mappedRange.offset              = offset;
            mappedRange.size                = size;
            vkFlushMappedMemoryRanges(VKDevice::Get().getDevice(), 1, &mappedRange);
#else
            vmaFlushAllocation(VKDevice::Get().getVMA(), m_VMAAllocation, offset, size);
#endif
        }

        void VKBuffer::invalidate(VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

#ifndef RAZIX_USE_VMA
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory              = m_BufferMemory;
            mappedRange.offset              = offset;
            mappedRange.size                = size;
            vkInvalidateMappedMemoryRanges(VKDevice::Get().getDevice(), 1, &mappedRange);
#else
            vmaInvalidateAllocation(VKDevice::Get().getVMA(), m_VMAAllocation, offset, size);
#endif
        }

        void VKBuffer::setData(u32 size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

#ifdef RAZIX_USE_VMA
            if (m_Usage == BufferUsage::Staging) {
                map(size, 0);
                memcpy(m_Mapped, data, size);
            } else if (m_Usage == BufferUsage::PersistentStream) {
                memcpy(m_AllocInfo.pMappedData, data, size);
            } else if (m_Usage == BufferUsage::Static) {
                /**
                * For anything else we copy using a staging buffer to copy to the GPU
                */
                VKBuffer m_TransferBuffer = VKBuffer(BufferUsage::Staging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging buffer to copy to Device only GPU buffer"));
                {
                    // 1.1 Copy from staging buffer to Image
                    VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                    VkBufferCopy region = {};
                    region.srcOffset    = 0;
                    region.dstOffset    = 0;
                    region.size         = size;

                    vkCmdCopyBuffer(commandBuffer, m_TransferBuffer.getBuffer(), m_Buffer, 1, &region);

                    VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
                }
                m_TransferBuffer.destroy();
            }
#else
            map(size, 0);
            memcpy(m_Mapped, data, size);
            unMap();
#endif
        }

        void VKBuffer::resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

            m_BufferSize = size;
            destroy();

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
            VmaAllocationCreateInfo vmaallocInfo = {};
            vmaallocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            vmaallocInfo.flags = m_VMAAllocFlags | VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
            vmaallocInfo.priority = 1.0f;
            //allocate the buffer
            VK_CHECK_RESULT(vmaCreateBuffer(VKDevice::Get().getVMA(), &bufferInfo, &vmaallocInfo, &m_Buffer, &m_VMAAllocation, &m_AllocInfo));

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

        void* VKBuffer::getMappedRegion()
        {
#ifdef RAZIX_USE_VMA
            //m_VMAAllocation->GetMappedData();
            if (m_Usage == BufferUsage::PersistentStream)
                return m_AllocInfo.pMappedData;
            else
                return m_Mapped;
#else
            return m_Mapped;
#endif
        }
    }    // namespace Graphics
}    // namespace Razix