#pragma once

#include "Razix/Core/RZRoot.h"

#include "Razix/Core/RZCore.h"

#include "Razix/Events/RZEvent.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"
#include "Razix/Graphics/Cameras/Camera3D.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Scene/RZSceneCamera.h"

#include <string>

namespace Razix {

    namespace Graphics {

        class RZFramebuffer;
        class RZShader;
        class RZRenderPass;

#define MAX_SWAPCHAIN_BUFFERS 3

        // TODO: Add the ViewProjection (+ maybe Light) as system UBOs and perform static Initialization for all the Renderers
        struct ViewProjectionSystemUBOData
        {
            alignas(16) glm::mat4 view       = glm::mat4(1.0f);
            alignas(16) glm::mat4 projection = glm::mat4(1.0f);
            //alignas(16) glm::mat4 _padding1 = glm::mat4(1.0f);
            //alignas(16) glm::mat4 _padding2 = glm::mat4(1.0f);
        };

        /**
         * Provides the Interface for Implementing renderers in Razix engine
         */
        class RAZIX_API IRZRenderer : public RZRoot
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
             * uUsed to init disposable resources like render passes, fbos, etc.
             */
            virtual void InitDisposableResources() = 0;
            /**
             * Begins the Renderer, sets up Engine/Render system variables/data and prepares the GPU and bindable resources
             * along with render passes, pipelines, descriptors, root signatures and everything the GPU needs to render
             */
            virtual void Begin() = 0;
            /**
             * Begins rendering the scene by reading the scene and extracting the necessary information 
             */
            virtual void BeginScene(RZScene* scene) = 0;
            /**
             * Submits the draw calls to the GPU by binding the necessary resources + context information + pipeline
             */
            virtual void Submit(RZCommandBuffer* cmdBuf) = 0;
            /**
             * Ends rendering the scene
             */
            virtual void EndScene(RZScene* scene) = 0;
            /**
             * Ends the renderer for the frame
             */
            virtual void End() = 0;
            /**
             * Presents the Renderer output onto the screen
             */
            virtual void Present() = 0;
            /**
             * Handles resizing the renderer
             */
            virtual void Resize(uint32_t width, uint32_t height) = 0;
            /**
             * Destroy all the renderer resources
             */
            virtual void Destroy() = 0;

            // TODO: Other Utility Methods :: Yes to be designed after implementing Materials and Scene Culling
            //virtual void SubmitMesh();
            //virtual void SubmitCulledMesh();

            /**
             * Events passed to the Renderer, Rendering Events can be passes this way 
             * This may include important info from Engine/GPU/any other critical place
             * 
             * @param event TThe event received from all the above mentions systems/places
             */
            virtual void OnEvent(RZEvent& event) = 0;
            /* ImGui render settings/Info of the renderer for debug builds */
            virtual void OnImGui() {}

            void setRenderPriorityIndex(uint32_t idx) { m_PriorityIndex = idx; }

        protected:
            // TODO: Use a vector to hold many shaders to support multiple render passes + multi-layered materials in future???
            RZSceneCamera*                          m_Camera;
            RZCommandBuffer*                        m_MainCommandBuffers[MAX_SWAPCHAIN_BUFFERS];
            RZShader*                               m_OverrideGlobalRHIShader;
            RZScene*                                m_CurrentScene;
            RZUniformBuffer*                        m_ViewProjectionSystemUBO = nullptr;
            ViewProjectionSystemUBOData             m_ViewProjSystemUBOData;
            RZRenderPass*                           m_RenderPass;
            RZPipeline*                             m_Pipeline;
            uint32_t                                m_ScreenBufferWidth  = 0;
            uint32_t                                m_ScreenBufferHeight = 0;
            RZTexture*                              m_RenderTexture      = nullptr;
            RZTexture*                              m_DepthTexture       = nullptr;
            uint32_t                                m_PriorityIndex      = 0;
            bool                                    m_OffScreenRender    = false;
            uint8_t                                 _padding[3];
            std::vector<RZFramebuffer*>             m_Framebuffers;
            std::vector<Graphics::RZDescriptorSet*> m_DescriptorSets;
            std::string                             m_RendererName;
        };
    }    // namespace Graphics
}    // namespace Razix