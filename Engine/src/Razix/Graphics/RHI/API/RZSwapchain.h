#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZLog.h"

#include "Razix/Core/OS/RZWindow.h"

namespace Razix {
    namespace Graphics {

        /* Forward decelerations */
        class RZTexture;       /* The texture2D to which the swap images are stored as */
        class RZCommandBuffer; /* The command buffer that will be submitted for execution */

        /* The swapchain that consists of multiple render targets and framebuffer attachments to render to the surface */
        class RAZIX_API RZSwapchain : public RZRoot
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
            static RZSwapchain* Create(u32 width, u32 height);

            /* Initializes the swapchain and it's resources */
            virtual void Init(u32 width, u32 height) = 0;
            /* Destroys the swapchain and it's resources */
            virtual void Destroy() = 0;
            /* Flips the swapchain images for presentation, typically used while doing d32/triple buffered rendering */
            virtual void Flip() = 0;
            /* Recreates the swapchain on window resize or for offline rendering */
            virtual void OnResize(u32 width, u32 height) = 0;

            /* Gets the swapchain image at the given index */
            virtual RZTextureHandle GetImage(u32 index) = 0;
            /* Gets the current image that is ready for rendering */
            virtual RZTextureHandle GetCurrentImage() = 0;
            /* Gets the count of total number of images in the swapchain */
            virtual sz GetSwapchainImageCount() = 0;
            /* Gets the index of the current image that is ready for rendering */
            virtual inline u32 getCurrentImageIndex() = 0;

            /* Gets the width of the swapchain */
            inline u32 getWidth() { return m_Width; }
            /* Gets the height of the swapchain */
            inline u32 getHeight() { return m_Height; }

            virtual void* GetAPIHandle() = 0;

        protected:
            u32 m_Width;                 /* The width of the swapchain extent */
            u32 m_Height;                /* The height of the swapchain extent */
            u32 m_CurrentImageIndex = 0; /* The current image that is ready for rendering  */
        };

    }    // namespace Graphics
}    // namespace Razix