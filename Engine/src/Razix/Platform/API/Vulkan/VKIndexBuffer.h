#pragma once

#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Gfx {
        class VKIndexBuffer final : public RZIndexBuffer, public VKBuffer
        {
        public:
            VKIndexBuffer(const RZBufferDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKIndexBuffer() {}

            RAZIX_CLEANUP_RESOURCE

            void Bind(RZDrawCommandBufferHandle cmdBuffer = {}) override;
            void Unbind() override {}

            void Resize(u32 size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;

            void  Map(u32 size = 0, u32 offset = 0) override;
            void  UnMap() override;
            void  Flush() override;
            void  Invalidate() override;
            void* GetMappedBuffer() override;

        private:
            bool m_IsBufferMapped = false;
            
            void destroy();
        };
    }    // namespace Graphics
}    // namespace Razix
