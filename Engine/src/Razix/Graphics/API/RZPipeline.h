#pragma once

#include "Razix/Core/RZSmartPointers.h"

namespace Razix {
    namespace Graphics {

        // Forward decelerations to reduce include files complexity
        class RZRenderPass;
        class RZShader;
        class RZCommandBuffer;

        /* Culling mode describes which face of the polygon will be culled */
        enum class CullMode
        {
            FRONT,
            BACK,
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
            RZRenderPass* renderpass;
            RZShader*     shader;

            CullMode    cullMode    = CullMode::BACK;
            PolygonMode polygonMode = PolygonMode::FILL;
            DrawType    drawType    = DrawType::TRIANGLE;

            bool transparencyEnabled = true;
            bool depthBiasEnabled    = false;
        };

        /* Pipeline binds all the resources together that are necessary to render geometry such as shaders, buffers, uniforms, descriptors and pipeline info */
        class RAZIX_API RZPipeline
        {
        public:
            RZPipeline () = default;
            virtual ~RZPipeline () {}

            static RZPipeline* Create (const PipelineInfo& pipelineInfo);

            virtual void Bind (RZCommandBuffer* commandBuffer) = 0;
            virtual void Destroy ()                            = 0;

        protected:
        };
    }    // namespace Graphics
}    // namespace Razix