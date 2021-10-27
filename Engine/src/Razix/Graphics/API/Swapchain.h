#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

#include "Razix/Core/OS/Window.h"

namespace Razix {
    namespace Graphics {

        class RZTexture;
        class RZCommandBuffer;

        class RZSwapchain
        {
        public:
            virtual ~RZSwapchain() = default;

            static RZSwapchain* Create(uint32_t width, uint32_t height);
            
            virtual void Init() = 0;
            virtual void Destroy() = 0;

            virtual RZTexture* GetImage(uint32_t index) = 0;
            virtual RZTexture* GetCurrentImage() = 0;
            virtual uint32_t GetCurrentImageIndex() = 0;
            virtual size_t GetSwapchainImageCount() = 0;
            virtual RZCommandBuffer* GetCurrentCommandBuffer() = 0;
        };

    }
}