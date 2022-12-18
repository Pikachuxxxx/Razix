#pragma once

#include "Razix/Graphics/API/RZRenderContext.h"

namespace Razix {
    namespace Graphics {

        // Forward Declarations
        class RZScene;

        /**
         * Razix World Renderer handles rendering everything in the scene, it build and compiles and frame graph and is responsible for execution
         */
        class RZWorldRenderer
        {
        public:
            RZWorldRenderer()  = default;
            ~RZWorldRenderer() = default;

            /**
             * Builds the frame graph using all the passes
             * 
             * Note: exports the svg & dot file into the Game/Exports/FrameGraph folder
             */
            void buildFrameGraph(RZScene* scene);
            /* draws the frame by executing the frame graph */
            void drawFrame(RZScene* scene);

        private:
            
            // List of all passes and data in the frame graph
        };
    }    // namespace Graphics
}    // namespace Razix
