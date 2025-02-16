#pragma once

#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"
#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"

namespace Razix {
    namespace Gfx {

        class VKVertexBuffer final : public RZVertexBuffer, public VKBuffer
        {
        public:
            VKVertexBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKVertexBuffer() {}

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            void Bind(RZDrawCommandBufferHandle cmdBuffer, uint32_t binding = 0) override;
            void Unbind() override {}

            void SetData(u32 size, const void* data) override;
            void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;

            void  Map(u32 size = 0, u32 offset = 0) override;
            void  UnMap() override;
            void  Flush() override;
            void  Invalidate() override;
            void* GetMappedBuffer() override;

            void Destroy();

        private:
            bool m_IsBufferMapped = false;
        };
    }    // namespace Gfx
}    // namespace Razix
#endif