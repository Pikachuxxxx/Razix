#pragma once

#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"

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
            VKUniformBuffer(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKUniformBuffer() {}

            void Init(const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;
            void Bind() override {}
            void SetData(u32 size, const void* data) override;
            void Destroy() override;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif