#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Events/RZEvent.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"

#include "Razix/Scene/RZScene.h"

#include <string>

namespace Razix {

    namespace Graphics {

        /**
         * Provides the Interface for Implementing renderers in Razix engine
         */
        class RAZIX_API IRZRenderer
        {
        public:
            IRZRenderer() = default;
            virtual ~IRZRenderer() {}

            // Renderer Flow - same order as the methods declared

            /**
             * Initializes the renderer and the necessary resources for it
             */
            virtual void InitRenderer() = 0;
            /**
             * Begins the Renderer, sets up Engine/Render system variables/data and prepares the GPU and bindable resources
             * along with render passes, pipelines, descriptors, root signatures and everything the GPU needs to render
             */
            virtual void BeginRenderer() = 0;
            /**
             * Begins rendering the scene by reading the scene and extracting the necessary information 
             */
            virtual void BeginScene(const RZScene* scene) = 0;
            /**
             * Submits the draw calls to the GPU by binding the necessary resources + context information + pipeline
             */
            virtual void Submit(RZCommandBuffer& cmdBuf) = 0;
            /**
             * Ends rendering the scene
             */
            virtual void EndScene(const RZScene* scene) = 0;
            /**
             * Ends the renderer for the frame
             */
            virtual void EndRenderer() = 0;
            /**
             * Presents the Renderer output onto the screen
             */
            virtual void Present() = 0;
            /**
             * Handles resizing the renderer
             */
            virtual void Resize(uint32_t width, uint32_t height) = 0;

            /**
             * Events passed to the Renderer, Rendering Events can be passes this way 
             * This may include important info from Engine/GPU/any other critical place
             * 
             * @param event TThe event received from all the above mentions systems/places
             */
            virtual void OnEvent(RZEvent& event) = 0;
            /* ImGui render settings/Info of the renderer for debug builds */
            virtual void OnImGui() {}
        };
    }    // namespace Graphics
}    // namespace Razix