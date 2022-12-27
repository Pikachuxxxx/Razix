#pragma once

#include "Razix/Core/RZDebugConfig.h"
#include "Razix/Core/RZSmartPointers.h"

#include "Razix/Graphics/API/RZTexture.h"

namespace Razix {
    namespace Graphics {

        /**
         * NOTE:- The problem is though VK_EXT_dynamic_rendering eliminated render passes and framebuffers it still needs some before hand info while creating pipeline
         * What this means is that the Pipeline cannot be the sole property of the material, cause in a FrameGraphPass it might have multiple R/W attachments
         * and a multiple Materials will have different pipelines, since this is pre-baked info
         * one needs to re-adjust the materials uses in a FrameGraph pass during Frame Graph compilation phase
         */

        // Forward decelerations to reduce include files complexity
        class RZRenderPass;
        class RZShader;
        class RZCommandBuffer;

        /* Culling mode describes which face of the polygon will be culled */
        enum class CullMode
        {
            BACK,    // default
            FRONT,
            FRONTANDBACK,
            NONE
        };

        /* Polygon mode describes how the geometry will be drawn, not the primitive used to draw */
        enum class PolygonMode
        {
            FILL, /* The geometry will be filled with pixels                                                  */
            LINE, /* Only the outline of the geometry primitives will be drawn based on the line width set    */
            POINT /* Only the vertices will be drawn with square shaped points based on point size            */
        };

        /* Draw type describes what primitive will be used to draw the vertex data */
        enum class DrawType
        {
            POINT,
            TRIANGLE,
            LINES
        };

        /* Information necessary to create the pipeline */
        struct PipelineInfo
        {
            RZShader*                      shader;
            std::vector<RZTexture::Format> colorAttachmentFormats;
            RZTexture::Format              depthFormat = RZTexture::Format::NONE;
            CullMode                       cullMode    = CullMode::BACK;
            PolygonMode                    polygonMode = PolygonMode::FILL;
            DrawType                       drawType    = DrawType::TRIANGLE;

            bool transparencyEnabled = true;
            bool depthBiasEnabled    = false;
        };

        /* Pipeline binds all the resources together that are necessary to render geometry such as shaders, buffers, uniforms, descriptors and pipeline info */
        class RAZIX_API RZPipeline : public RZRoot
        {
        public:
            RZPipeline() = default;
            virtual ~RZPipeline() {}

            static RZPipeline* Create(const PipelineInfo& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);

            virtual void Bind(RZCommandBuffer* commandBuffer) = 0;
            virtual void Destroy()                            = 0;

        protected:
        };
    }    // namespace Graphics
}    // namespace Razix