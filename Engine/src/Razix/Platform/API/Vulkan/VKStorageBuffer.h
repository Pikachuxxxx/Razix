#pragma once

#include "Razix/Gfx/RHI/API/RZStorageBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Gfx {
        class VKStorageBuffer : public RZStorageBuffer
        {
        public:
            VKStorageBuffer(u32 size, const std::string& name);
            ~VKStorageBuffer() {}

            void* GetData() override;
            void  SetData(u32 size, const void* data) override;
            void  Destroy() override;
        };
    }    // namespace Gfx
}    // namespace Razix
