#pragma once

#include "Razix/Graphics/API/RZFramebuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        class VKFramebuffer : public RZFramebuffer
        {
        public:
            VKFramebuffer(const FramebufferInfo& frameBufInfo);
            ~VKFramebuffer();

            /* Gets the handle to the Vulkan framebuffer object */
            inline const VkFramebuffer& getVkFramebuffer() const { return m_Framebuffer; }
        private:
            VkFramebuffer m_Framebuffer;
        };
    }
}
#endif