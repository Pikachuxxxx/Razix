#pragma once

#include "Razix/Gfx/RHI/API/RZFramebuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        class VKFramebuffer : public RZFramebuffer
        {
        public:
            VKFramebuffer(const FramebufferInfo& frameBufInfo RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKFramebuffer();

            void Destroy() override;

            /* Gets the handle to the Vulkan framebuffer object */
            inline const VkFramebuffer& getVkFramebuffer() const { return m_Framebuffer; }

        private:
            VkFramebuffer m_Framebuffer;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif