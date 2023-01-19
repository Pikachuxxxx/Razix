#pragma once

#include "Razix/Graphics/RHI/API/RZStorageBuffer.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"

namespace Razix {
    namespace Graphics {
        class VKStorageBuffer : public RZStorageBuffer, public VKBuffer
        {
        public:
            VKStorageBuffer(uint32_t size, const std::string& name);
            ~VKStorageBuffer() {}

            void* GetData() override;
            void  SetData(uint32_t size, const void* data) override;
            void  Destroy() override;
        };
    }    // namespace Graphics
}    // namespace Razix
