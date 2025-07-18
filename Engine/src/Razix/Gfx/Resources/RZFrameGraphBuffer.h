#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {
        struct RZFrameGraphBuffer
        {
            typedef rz_gfx_buffer_desc Desc;

            RZFrameGraphBuffer() = default;
            //RZFrameGraphBuffer(Gfx::RZUniformBufferHandle handle)
            //: m_BufferHandle(handle) {}

            void create(const Desc& desc, u32 id, const void* transientAllocator);
            void destroy(u32 id, const void* transientAllocator);

            void preRead(const Desc& desc, uint32_t flags);
            void preWrite(const Desc& desc, uint32_t flags);

            static std::string toString(const Desc& desc);

            //Gfx::RZUniformBufferHandle getHandle() const { return m_BufferHandle; }

        private:
            //Gfx::RZUniformBufferHandle m_BufferHandle;
            //Gfx::BufferBarrierType     m_LastReadBarrier;
            //Gfx::BufferBarrierType     m_LastWriteBarrier;
        };
    }    // namespace Gfx
}    // namespace Razix