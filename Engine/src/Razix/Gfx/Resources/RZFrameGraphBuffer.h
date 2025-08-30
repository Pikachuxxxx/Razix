#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        struct RAZIX_API RZFrameGraphBuffer
        {
            typedef rz_gfx_buffer_desc Desc;

            void create(const std::string& name, const Desc& desc, u32 id, const void* transientAllocator);
            void destroy(u32 id, const void* transientAllocator);

            void preRead(const Desc& desc, uint32_t flags);
            void preWrite(const Desc& desc, uint32_t flags);

            static std::string toString(const Desc& desc);

            inline rz_gfx_buffer_handle getRHIHandle() const { return m_BufferHandle; }
            //inline Gfx::BufferBarrierType getLastReadBarrier() const { return m_LastReadBarrier; }
            //inline Gfx::BufferBarrierType getLastWriteBarrier() const { return m_LastWriteBarrier; }

            // public for initializer list support
            rz_gfx_buffer_handle m_BufferHandle;

        private:
            //Gfx::BufferBarrierType     m_LastReadBarrier;
            //Gfx::BufferBarrierType     m_LastWriteBarrier;
        };
    }    // namespace Gfx
}    // namespace Razix