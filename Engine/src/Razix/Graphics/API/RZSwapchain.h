#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/RZLog.h"

#include "Razix/Core/OS/RZWindow.h"

namespace Razix {
    namespace Graphics {

        /* Forward decelerations */
        class RZTexture;            /* The texture2D to which the swap images are stored as */
        class RZCommandBuffer;      /* The command buffer that will be submitted for execution */  

        /* The swapchain that consists of multiple render targets and framebuffer attachments to render to the surface */
        class RZSwapchain
        {
        public:
            /* Virtual destructor RZSwapchain */
            virtual ~RZSwapchain() = default;

            /**
             * Creates a swapchain for the render pipeline
             * 
             * @param width The Width of the swapchain extent
             * @param height The Height of the swapchain extent
             */
            static RZSwapchain* Create(uint32_t width, uint32_t height);
            
            /* Initializes the swapchain and it's resources */
            virtual void Init() = 0;
            /* Destroys the swapchain and it's resources */
            virtual void Destroy() = 0;
            /* Flips the swapchain images for presentation, typically used while foing double/triple buffered rendering */
            virtual void Flip() = 0;

            /* Gets the swapchain image at the given index */
            virtual RZTexture* GetImage(uint32_t index) = 0;
            /* Gets the current image that is ready for rendering */
            virtual RZTexture* GetCurrentImage() = 0;
            /* Gets the count of total number of images in the swapchain */
            virtual size_t GetSwapchainImageCount() = 0;
            /* Gets the current command buffer being executed for rendering onto the swapchain */
            virtual RZCommandBuffer* GetCurrentCommandBuffer() = 0;

            /* Gets the width of the swapchain */
            inline uint32_t getWidth() { return m_Width; }
            /* Gets the height of the swapchain */
            inline uint32_t getHeight() { return m_Height; }
            /* Gets the index of the current image that is ready for rendering */
            inline uint32_t getCurrentImageIndex() { return m_CurrentImageIndex; }
        protected:
            uint32_t m_Width;                   /* The width of the swapchain extent */
            uint32_t m_Height;                  /* The height of the swapchain extent */
            uint32_t m_CurrentImageIndex = 0;   /* The current image that is ready for rendering  */
        };

    }
}