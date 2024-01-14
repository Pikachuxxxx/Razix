#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vma/vk_mem_alloc.h>
    #include <vulkan/vulkan.h>

    #include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        /* Vulkan buffer to hold and transfer data */
        class VKBuffer
        {
        public:
            /**
             * Creates a buffer with the usage type and of required size
             * 
             * @param usage What will the buffer used for? Storage, transfer staging or something else, high level usage
             * @param size The size of the buffer
             * @param data The data with which the buffer will be filled with
             */
            VKBuffer(BufferUsage usage, u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG);
            /* Creates an empty buffer to be mapped with data later */
            VKBuffer();
            ~VKBuffer() {}

            /**
             * Initializes the buffer with the given size, usage and data
             * 
             * @param data The data with which the buffer will be filled with
             */
            // TODO: Refactor this signature to take the size and usage
            void init(const void* data RZ_DEBUG_NAME_TAG_E_ARG);

            /* Destroy the buffer and it's memory */
            void destroy();

            /* Maps the buffer to an external location on the HOST to copy data to it */
            void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            /* Unmaps the memory to which the buffer was mapped to */
            void unMap();
            /* Clears out the buffer allocation */
            void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            /* Sets the buffer with new given data */
            void setData(u32 size, const void* data);
            /* Resizes the buffer */
            void resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG);

            /* Gets the HOST mapped buffer */
            inline void* getMappedRegion() { return m_Mapped; }
            /* Gets the reference to the Vulkan buffer object */
            inline const VkBuffer& getBuffer() const { return m_Buffer; }
            /* Tells how the buffer is being used currently */
            inline const VkBufferUsageFlags& getUsageFlags() const { return m_UsageFlags; }
            /* Sets the usage of the buffer */
            inline void setUsageFlags(VkBufferUsageFlags flags) { m_UsageFlags = flags; }
            /* Sets the size of the buffer */
            inline void setSize(u32 size) { m_BufferSize = (VkDeviceSize) size; }
            /* Gets the size of the buffer */
            inline VkDeviceSize getSize() const { return m_BufferSize; }
            /* Gets the buffer high-level usage type */
            inline const BufferUsage& getUsage() const { return m_Usage; }
    #ifdef RAZIX_USE_VMA
            /* Gets the VMA allocation for the buffer */
            inline VmaAllocation getVMAAllocation() const { return m_VMAAllocation; }
    #endif

        protected:
    #ifndef RAZIX_USE_VMA
            VkDeviceMemory m_BufferMemory`; /* Handle to the buffer memory                               */
    #else
            VmaAllocation     m_VMAAllocation; /* Holds the VMA allocation state info                       */
            VmaAllocationInfo m_AllocInfo;     /* Holds the alloc info and persistently mapped CPU pointer  */
    #endif                                         // RAZIX_USE_VMA
            void*              m_Mapped = nullptr; /* The HOST mapped region of the buffer, non-persistent      */
            VkBuffer           m_Buffer;           /* handle to the Vulkan GPU buffer handle                    */
            VkDeviceSize       m_BufferSize;       /* The size of the buffer                                    */
            VkBufferUsageFlags m_UsageFlags;       /* Buffer usage description, vulkan level                    */
            BufferUsage        m_Usage;            /* Buffer usage description                                  */
        };
    }    // namespace Graphics
}    // namespace Razix
#endif