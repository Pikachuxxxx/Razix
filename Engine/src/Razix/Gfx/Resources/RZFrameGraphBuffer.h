#pragma once

namespace Razix {
    namespace Gfx {
        struct RZFrameGraphBuffer
        {
            typedef RZBufferDesc Desc;

            RZFrameGraphBuffer() = default;
            RZFrameGraphBuffer(Gfx::RZUniformBufferHandle handle)
                : m_BufferHandle(handle) {}

            void create(const Desc& desc, void* transientAllocator);
            void destroy(const Desc& desc, void* transientAllocator);

            void preRead(const Desc& desc, uint32_t flags);
            void preWrite(const Desc& desc, uint32_t flags);

            static std::string toString(const Desc& desc);

            Gfx::RZUniformBufferHandle getHandle() const { return m_BufferHandle; }

        private:
            Gfx::RZUniformBufferHandle m_BufferHandle;
            Gfx::BufferBarrierType     m_LastReadBarrier;
            Gfx::BufferBarrierType     m_LastWriteBarrier;
        };
    }    // namespace Gfx
}    // namespace Razix