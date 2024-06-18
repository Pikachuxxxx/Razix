#if 1
    #pragma once

    #include "Razix/Core/RZRoot.h"

    #include "Razix/Core/RZCore.h"

    #include "Razix/Events/RZEvent.h"

    #include "Razix/Graphics/Cameras/Camera3D.h"
    #include "Razix/Graphics/RHI/RHI.h"
    #include "Razix/Graphics/Renderers/RZSystemBinding.h"

    #include "Razix/Scene/RZScene.h"

    #include "Razix/Scene/RZSceneCamera.h"

    #include "Razix/Utilities/RZTimer.h"
    #include "Razix/Utilities/RZTimestep.h"

    #include <string>

namespace Razix {

    namespace Graphics {

        class RZFramebuffer;
        class RZShader;
        class RZRenderPass;
        class RZRenderTexture;

    #define MAX_SWAPCHAIN_BUFFERS 3

        /**
         * Provides the Interface for Implementing renderers in Razix engine
         */
        class RAZIX_MEM_ALIGN_16 RAZIX_API IRZRenderer : public RZRoot
        {
        public:
            IRZRenderer() = default;
            virtual ~IRZRenderer() {}

            // Renderer Flow - same order as the methods declared

            /**
             * Initializes the renderer and the necessary resources for it
             */
            virtual void Init() = 0;
            /**
             * Begins the Renderer, sets up Engine/Render system variables/data and prepares the GPU and bindable resources
             * along with render passes, pipelines, descriptors, root signatures and everything the GPU needs to render
             */
            virtual void Begin(RZScene* scene) = 0;
            /**
             * Draw by binding the necessary resources
             */
            virtual void Draw(RZDrawCommandBufferHandle cmdBuffer) = 0;
            /**
             * Ends the rendering for the frame
             */
            virtual void End() = 0;
            /**
             * Handles resizing the renderer
             */
            virtual void Resize(u32 width, u32 height) = 0;
            /**
             * Destroy all the renderer resources
             */
            virtual void Destroy() = 0;

        protected:
            RZSceneCamera*   m_Camera;
            RZScene*         m_CurrentScene;
            RZPipelineHandle m_Pipeline;
            u32              m_ScreenBufferWidth  = 0;
            u32              m_ScreenBufferHeight = 0;
            std::string      m_RendererName;
            RZTimestep       m_PassTimer;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif