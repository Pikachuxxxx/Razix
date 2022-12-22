#if 1
    #pragma once

    #include "Razix/Core/RZRoot.h"

    #include "Razix/Core/RZCore.h"

    #include "Razix/Events/RZEvent.h"

    #include "Razix/Graphics/API/RZRenderContext.h"
    #include "Razix/Graphics/Cameras/Camera3D.h"

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
         * Note:- Not an enum class because we need them as Int!!!! so the style guide is broke here on purpose
         * This for the render system 
         * for ex. for GI data, View proj data and Lighting data that will be defined by the rendering engine system data
         * we will have only set info for now as we implement if we need set info we'll see
         */
        enum BindingTable_System : uint32_t
        {
            BINDING_SET_SYSTEM_VIEW_PROJECTION   = 0,    // How to feed this to the Renderer? in terms of Include files order which is fucked up, should I make a header file to hold binding Material + Render System binding table infos
            BINDING_SET_SYSTEM_FORWARD_LIGHTING  = 1,
            BINDING_SET_SYSTEM_DEFERRED_LIGHTING = BINDING_SET_SYSTEM_FORWARD_LIGHTING
        };

        // TODO: Add the ViewProjection (+ maybe Light) as system UBOs and perform static Initialization for all the Renderers
        struct ViewProjectionSystemUBOData
        {
            alignas(16) glm::mat4 view       = glm::mat4(1.0f);
            alignas(16) glm::mat4 projection = glm::mat4(1.0f);
        };

        /**
         * Provides the Interface for Implementing renderers in Razix engine
         */
        class RAZIX_MEM_ALIGN RAZIX_API IRZRenderer : public RZRoot
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
            virtual void Draw(RZCommandBuffer* cmdBuffer) = 0;
            /**
             * Ends the rendering for the frame
             */
            virtual void End() = 0;
            /**
             * Handles resizing the renderer
             */
            virtual void Resize(uint32_t width, uint32_t height) = 0;
            /**
             * Destroy all the renderer resources
             */
            virtual void Destroy() = 0;

        protected:
            RZSceneCamera*                          m_Camera;
            RZCommandBuffer*                        m_MainCommandBuffers[MAX_SWAPCHAIN_BUFFERS];
            RZShader*                               m_OverrideGlobalRHIShader;
            RZScene*                                m_CurrentScene;
            RZUniformBuffer*                        m_ViewProjectionSystemUBO = nullptr;
            ViewProjectionSystemUBOData             m_ViewProjSystemUBOData;
            uint32_t                                m_ScreenBufferWidth  = 0;
            uint32_t                                m_ScreenBufferHeight = 0;
            RZPipeline*                             m_Pipeline;
            RZRenderTexture*                        m_RenderTexture = nullptr;
            RZTexture*                              m_DepthTexture  = nullptr;
            uint8_t                                 _padding[3];
            std::vector<Graphics::RZDescriptorSet*> m_DescriptorSets;
            std::string                             m_RendererName;
            RZTimestep                              m_PassTimer;
            RZTimer                                 m_RendererTimer;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif