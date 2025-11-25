#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        struct RAZIX_API RZFrameGraphBuffer
        {
            typedef rz_gfx_buffer_desc Desc;

            void create(const RZString& name, const Desc& desc, u32 id, const void* transientAllocator);
            void destroy(u32 id, const void* transientAllocator);

            void preRead(u32 descriptorType, u32 resViewOpFlags);
            void preWrite(u32 descriptorType, u32 resViewOpFlags);

            static RZString toString(const Desc& desc);

            inline rz_gfx_buffer_handle getRHIHandle() const { return m_BufferHandle; }

            rz_gfx_buffer_handle m_BufferHandle;
        };
    }    // namespace Gfx
}    // namespace Razix