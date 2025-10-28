#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Events/RZEvent.h"

#include "Razix/Gfx/Cameras/Camera3D.h"
#include "Razix/Gfx/RHI/RHI.h"
#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Scene/RZSceneCamera.h"

#include "Razix/Core/Utils/RZTimer.h"
#include "Razix/Core/Utils/RZTimestep.h"

#include <string>

namespace Razix {

    namespace Gfx {

        class RZFramebuffer;
        class RZShader;
        class RZRenderPass;
        class RZRenderTexture;

#define MAX_SWAPCHAIN_BUFFERS 3

        /**
         * Provides the Interface for Implementing renderer proxies for aiding Frame Graph rendering
         */
        class RAZIX_MEM_ALIGN_16 RAZIX_API IRZRendererProxy
        {
        public:
            IRZRendererProxy() = default;
            virtual ~IRZRendererProxy() {}

            // Renderer Flow - same order as the methods declared

            virtual void Init()                                    = 0;
            virtual void Begin(RZScene* scene)                     = 0;
            virtual void Draw(RZDrawCommandBufferHandle cmdBuffer) = 0;
            virtual void End()                                     = 0;
            virtual void Resize(u32 width, u32 height)             = 0;
            virtual void Destroy()                                 = 0;

        protected:
            RZSceneCamera*   m_Camera             = nullptr;
            RZScene*         m_CurrentScene       = nullptr;
            RZPipelineHandle m_Pipeline           = {};
            u32              m_ScreenBufferWidth  = 0;
            u32              m_ScreenBufferHeight = 0;
            std::string      m_RendererName       = "";
            RZTimestep       m_PassTimer          = {};
        };
    }    // namespace Gfx
}    // namespace Razix
