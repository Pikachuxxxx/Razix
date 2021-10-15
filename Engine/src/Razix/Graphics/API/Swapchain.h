#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

#include "Razix/Core/OS/Window.h"

namespace Razix {
    namespace Graphics {

        class Texture;
        class CommandBuffer;

        class Swapchain
        {
        public:
            virtual ~Swapchain() = default;

            static Swapchain* Create(uint32_t width, uint32_t height);
            
            virtual void Init() = 0;
            virtual void Destroy() = 0;

            virtual Texture* GetImage(uint32_t index) = 0;
            virtual Texture* GetCurrentImage() = 0;
            virtual uint32_t GetCurrentImageIndex() = 0;
            virtual size_t GetSwapchainImageCount() = 0;
            virtual CommandBuffer* GetCurrentCommandBuffer() = 0;
        };

    }
}