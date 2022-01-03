#include "rzxpch.h"
#include "VKPipeline.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"

namespace Razix {
    namespace Graphics {

        VKPipeline::VKPipeline(const PipelineInfo& pipelineInfo)
        {
            m_Shader = pipelineInfo.shader;
            //m_PipelineLayout = m_Shader.As<VKShader>()


        }

        VKPipeline::~VKPipeline()
        {
            vkDestroyPipeline(VKDevice::Get().getDevice(), m_Pipeline, nullptr);
        }

        void VKPipeline::Bind(RZCommandBuffer* commandBuffer)
        {

        }
    }
}