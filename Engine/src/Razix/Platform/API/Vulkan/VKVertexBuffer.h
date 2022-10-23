#pragma once

#include "Razix/Graphics/API/RZVertexBuffer.h"
#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"

namespace Razix {
    namespace Graphics {

        class VKVertexBuffer : public RZVertexBuffer, public VKBuffer
        {
        public:
            VKVertexBuffer(uint32_t size, const void* data, BufferUsage usage RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKVertexBuffer();

            void Bind(RZCommandBuffer* cmdBuffer) override;
            void Unbind() override;

            void SetData(uint32_t size, const void* data) override;
            void Resize(uint32_t size, const void* data RZ_DEBUG_NAME_TAG_E_ARG) override;
            void AddBufferLayout(RZVertexBufferLayout& layout) override;

            void Destroy() override;

            virtual void  Map(uint32_t size = 0, uint32_t offset = 0) override;
            virtual void  UnMap() override;
            virtual void* GetMappedBuffer() override;
            virtual void  Flush() override;

        private:
            bool m_IsBufferMapped = false;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif