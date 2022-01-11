#pragma once

#include "Razix/Graphics/API/RZUniformBuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {

        /* Vulkan API Implementation of the RZUniformBufferm, it is also a VKBuffer by behavior */
        class VKUniformBuffer : public RZUniformBuffer, public VKBuffer
        {
        public:
            /**
             * Creates a Uniform buffer using VKBuffer using the right flags for usage
             * 
             * @param size The size of the buffer
             * @param data The data with which the buffer is filled with
             */
            VKUniformBuffer(uint32_t size, const void* data);
            ~VKUniformBuffer() {}

            void Init(const void* data) override;
            void SetData(uint32_t size, const void* data) override;
            void Destroy() override;
        };
    }
}
#endif