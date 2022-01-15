#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZLog.h"

#include "Razix/Core/OS/RZWindow.h"

namespace Razix {
    namespace Graphics {

        /* Triple buffering is enabled by default */
        #define RAZIX_ENABLE_TRIPLE_BUFFERING
        /* The total number of images that the swapchan can render/present to, by default we use triple buffering, defaults to double buffering if disabled */
#ifdef RAZIX_ENABLE_TRIPLE_BUFFERING
        /* Frames in FLight defines the number of frames that will be rendered to while another frame is being presented (used for triple buffering)*/
        #define RAZIX_MAX_FRAMES_IN_FLIGHT 2
        #define RAZIX_MAX_SWAP_IMAGES_COUNT 3
#elif 
        #define RAZIX_MAX_SWAP_IMAGES_COUNT 2
#endif
        /* Forward decelerations */
        class RZTexture;            /* The texture2D to which the swap images are stored as */
        class RZCommandBuffer;      /* The command buffer that will be submitted for execution */  

        /* The swapchain that consists of multiple render targets and framebuffer attachments to render to the surface */
        class RAZIX_API RZSwapchain
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
            virtual void Init(uint32_t width, uint32_t height) = 0;
            /* Destroys the swapchain and it's resources */
            virtual void Destroy() = 0;
            /* Flips the swapchain images for presentation, typically used while doing double/triple buffered rendering */
            virtual void Flip() = 0;
            /* Recreates the swapchain on window resize or for offline rendering */
            virtual void OnResize(uint32_t width, uint32_t height) = 0;

            /* Gets the swapchain image at the given index */
            virtual RZTexture* GetImage(uint32_t index) = 0;
            /* Gets the current image that is ready for rendering */
            virtual RZTexture* GetCurrentImage() = 0;
            /* Gets the count of total number of images in the swapchain */
            virtual size_t GetSwapchainImageCount() = 0;
            /* Gets the current command buffer being executed for rendering onto the swapchain */
            virtual RZCommandBuffer* getCurrentCommandBuffer() = 0;

            /* Gets the width of the swapchain */
            inline uint32_t getWidth() { return m_Width; }
            /* Gets the height of the swapchain */
            inline uint32_t getHeight() { return m_Height; }
            /* Gets the index of the current image that is ready for rendering */
            virtual inline uint32_t getCurrentImageIndex() = 0;
        protected:
            uint32_t m_Width;                   /* The width of the swapchain extent */
            uint32_t m_Height;                  /* The height of the swapchain extent */
            uint32_t m_CurrentImageIndex = 0;   /* The current image that is ready for rendering  */
        };

    }
}