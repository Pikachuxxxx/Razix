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

        VkResult CreateDebugObjName (VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
        {
            auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetInstanceProcAddr (VKContext::Get ()->getInstance (), "vkSetDebugUtilsObjectNameEXT");
            if (func != nullptr)
                return func (device, pNameInfo);
            else
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        VKBuffer::VKBuffer (VkBufferUsageFlags usage, uint32_t size, const void* data, const std::string& bufferName)
            : m_UsageFlags (usage), m_BufferSize (size)
        {
            init (data, bufferName);
        }

        VKBuffer::VKBuffer ()
            : m_UsageFlags (VK_BUFFER_USAGE_TRANSFER_SRC_BIT), m_BufferSize (0) {}

        void VKBuffer::destroy ()
        {
            if (m_Buffer != VK_NULL_HANDLE) {
                vkDestroyBuffer (VKDevice::Get ().getDevice (), m_Buffer, nullptr);

                if (m_BufferMemory) {
                    vkFreeMemory (VKDevice::Get ().getDevice (), m_BufferMemory, nullptr);
                }
            }
        }

        void VKBuffer::map (VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC (RZ_PROFILE_COLOR_CORE);

            // Map the memory to the mapped buffer
            VkResult res = vkMapMemory (VKDevice::Get ().getDevice (), m_BufferMemory, offset, size, 0, &m_Mapped);
            RAZIX_CORE_ASSERT ((res == VK_SUCCESS), "[Vulkan] Failed to map buffer!");
        }

        void VKBuffer::unMap ()
        {
            RAZIX_PROFILE_FUNCTIONC (RZ_PROFILE_COLOR_CORE);

            if (m_Mapped) {
                vkUnmapMemory (VKDevice::Get ().getDevice (), m_BufferMemory);
                m_Mapped = nullptr;
            }
        }

        void VKBuffer::flush (VkDeviceSize size /*= VK_WHOLE_SIZE*/, VkDeviceSize offset /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC (RZ_PROFILE_COLOR_CORE);

            // Flush only if the buffer exists
            if (m_Buffer) {
                VkMappedMemoryRange mappedRange = {};
                mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
                mappedRange.memory              = m_BufferMemory;
                mappedRange.offset              = offset;
                mappedRange.size                = size;
                vkFlushMappedMemoryRanges (VKDevice::Get ().getDevice (), 1, &mappedRange);
            }
        }

        void VKBuffer::setData (uint32_t size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC (RZ_PROFILE_COLOR_CORE);

            map (size, 0);
            memcpy (m_Mapped, data, size);
            unMap ();

            //flush();
        }

        void VKBuffer::resize (uint32_t size, const void* data)
        {
            RAZIX_PROFILE_FUNCTIONC (RZ_PROFILE_COLOR_CORE);

            //destroy();
            m_BufferSize = size;
            init (data, m_BufferName);
        }

        void VKBuffer::init (const void* data, const std::string& bufferName)
        {
            RAZIX_PROFILE_FUNCTIONC (RZ_PROFILE_COLOR_CORE);

            m_BufferName                  = bufferName;
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size               = m_BufferSize;
            bufferInfo.usage              = m_UsageFlags;
            bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

            // Create the buffer
            VK_CHECK_RESULT (vkCreateBuffer (VKDevice::Get ().getDevice (), &bufferInfo, nullptr, &m_Buffer));

            // Get the memory requirements and allocate actual memory for the buffer and Bind it!
            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements (VKDevice::Get ().getDevice (), m_Buffer, &memRequirements);

            // Allocate the buffer
            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize       = memRequirements.size;
            allocInfo.memoryTypeIndex      = VKDevice::Get ().getPhysicalDevice ().get ()->getMemoryTypeIndex (memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            VK_CHECK_RESULT (vkAllocateMemory (VKDevice::Get ().getDevice (), &allocInfo, nullptr, &m_BufferMemory));

            //! Set the Data
            if (data != nullptr)
                setData ((uint32_t) m_BufferSize, data);

            // Bind the buffer to it's memory
            vkBindBufferMemory (VKDevice::Get ().getDevice (), m_Buffer, m_BufferMemory, 0);
            // Set name for the Buffer
            VkDebugUtilsObjectNameInfoEXT bufferObjNameInfo{};
            bufferObjNameInfo.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            bufferObjNameInfo.pObjectName  = bufferName.c_str ();
            bufferObjNameInfo.objectType   = VK_OBJECT_TYPE_BUFFER;
            bufferObjNameInfo.objectHandle = (uint64_t) m_Buffer;

            CreateDebugObjName (VKDevice::Get ().getDevice (), &bufferObjNameInfo);
        }
    }    // namespace Graphics
}    // namespace Razix