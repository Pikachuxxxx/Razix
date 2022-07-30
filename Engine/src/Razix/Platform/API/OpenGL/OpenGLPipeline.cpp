// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLPipeline.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

namespace Razix {
    namespace Graphics {

        OpenGLPipeline::OpenGLPipeline(const PipelineInfo& pipelineInfo)
            : m_Shader(pipelineInfo.shader), m_PipelineInfo(pipelineInfo)
        {
        }

        void OpenGLPipeline::Bind(RZCommandBuffer* commandBuffer)
        {
            // Set cull mode, polygon mode, depth and transparency settings
            // TODO: Configure depth and blend test functions

            //glCullFace(OpenGLUtilities::CullModeToGLCullModeEnum(m_PipelineInfo.cullMode));

            //glPolygonMode(OpenGLUtilities::CullModeToGLCullModeEnum(m_PipelineInfo.cullMode), OpenGLUtilities::PolygonModeToGLPolygonModeEnum(m_PipelineInfo.polygonMode));

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);

            //if (m_PipelineInfo.depthBiasEnabled)
            //    glEnable(GL_DEPTH_TEST);
            //else
            //    glDisable(GL_DEPTH_TEST);
            //
            //if (m_PipelineInfo.transparencyEnabled)
            //    glEnable(GL_BLEND);
            //else
            //    glDisable(GL_BLEND);
        }

        void OpenGLPipeline::Destroy()
        {
        }

    }    // namespace Graphics
}    // namespace Razix