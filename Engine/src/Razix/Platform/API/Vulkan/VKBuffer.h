#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        /* Vulkan buffer to hold and transfer data */
        class VKBuffer : public RZRoot
        {
        public:
            /**
             * Creates a buffer with the usage type and of required size
             * 
             * @param usage What will the buffer used for? Storage, transfer staging or something else
             * @param size The size of the buffer
             * @param data The data with which the buffer will be filled with
             */
            VKBuffer(VkBufferUsageFlags usage, uint32_t size, const void* data NAME_TAG);
            /* Creates an empty buffer to be mapped with data later */
            VKBuffer();
            ~VKBuffer() {}

            /**
             * Initializes the buffer with the given size, usage and data
             * 
             * @param data The data with which the buffer will be filled with
             */
            // TODO: Refactor this signature to take the size and usage
            void init(const void* data NAME_TAG);

            /* Destroy the buffer and it's memory */
            void destroy();

            /* Maps the buffer to an external location on the HOST to copy data to it */
            void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            /* Unmaps the memory to which the buffer was mapped to */
            void unMap();
            /* Clears out the buffer allocation */
            void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            /* Sets the buffer with new given data */
            void setData(uint32_t size, const void* data);
            /* Resizes the buffer */
            void resize(uint32_t size, const void* data NAME_TAG);

            /* Gets the HOST mapped buffer */
            inline void* getMappedRegion() { return m_Mapped; }
            /* Gets the reference to the Vulkan buffer object */
            inline const VkBuffer& getBuffer() const { return m_Buffer; }
            /* Tells how the buffer is being used currently */
            inline const VkBufferUsageFlags& getUsage() const { return m_UsageFlags; }
            /* Sets the usage of the buffer */
            inline void setUsage(VkBufferUsageFlags flags) { m_UsageFlags = flags; }
            /* Gets the size of the buffer */
            inline VkDeviceSize getSize() const { return m_BufferSize; }
            /* Sets the size of the buffer */
            inline void setSize(uint32_t size) { m_BufferSize = (VkDeviceSize) size; }

        protected:
            VkBuffer               m_Buffer;              /* handle to the Vulkan buffer          */
            VkDeviceMemory         m_BufferMemory;        /* Handle to the buffer memory          */
            VkDeviceSize           m_BufferSize;          /* The size of the buffer               */
            VkDescriptorBufferInfo m_DesciptorBufferInfo; /* The buffer description info          */
            VkBufferUsageFlags     m_UsageFlags;          /* Buffer usage description             */
            void*                  m_Mapped = nullptr;    /* The HOST mapped region of the buffer */
        };

    }    // namespace Graphics
}    // namespace Razix
#endif