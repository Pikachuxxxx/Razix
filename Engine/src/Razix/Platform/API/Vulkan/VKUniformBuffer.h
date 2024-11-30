#pragma once

#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"

namespace Razix {
    namespace Gfx {

        /* Vulkan API Implementation of the RZUniformBufferm, it is also a VKBuffer by behavior */
        class VKUniformBuffer final : public RZUniformBuffer, public VKBuffer
        {
        public:
            /**
             * Creates a Uniform buffer using VKBuffer using the right flags for usage
             * 
             * @param size The size of the buffer
             * @param data The data with which the buffer is filled with
             */
            VKUniformBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKUniformBuffer() {}

            void Bind() override {}
            void SetData(u32 size, const void* data) override;
            void Flush() override;
            void Invalidate() override;

            void DestroyResource() override;
        };
    }    // namespace Gfx
}    // namespace Razix
#endif
