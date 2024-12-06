#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/GIData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Maths/RZGrid.h"

namespace Razix {
    namespace Graphics {

        class RZGIPass : public IRZPass
        {
        public:
            RZGIPass() = default;
            ~RZGIPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

            void setGrid(const Maths::RZGrid& grid) { m_Grid = grid; }

        private:
            Maths::RZGrid                 m_Grid;
            RZPipelineHandle              m_RIPipeline;
            RZPipelineHandle              m_RSMPipeline;
            RZPipelineHandle              m_RPropagationPipeline;
            Graphics::RZDescriptorSet*    m_MVPDescriptorSet;
            Graphics::RZDescriptorSet*    m_RIDescriptorSet;
            std::vector<RZDrawCommandBuffer*> m_RSMCmdBuffers;
            std::vector<RZDrawCommandBuffer*> m_RadianceInjectionCmdBuffers;
            std::vector<RZDrawCommandBuffer*> m_RadiancePropagationCmdBuffers;
            RadianceInjectionUBOData      radianceInjectionData;
            RZUniformBufferHandle         m_RadianceInjectionUBO;
            RadiancePropagationUBOData    radiancePropagationData;
            RZUniformBufferHandle         m_RadiancePropagationUBO;

            struct PropagationGPUResources
            {
                Graphics::RZDescriptorSet* PropagationDescriptorSet = nullptr;
            } m_PropagationGPUResources[kDefaultNumPropagations];

        private:
            ReflectiveShadowMapData     addRSMPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, const glm::mat4& lightViewProj, glm::vec3 lightIntensity);
            LightPropagationVolumesData addRadianceInjectionPass(FrameGraph::RZFrameGraph& framegraph, const ReflectiveShadowMapData& RSM, const Maths::RZGrid& grid);
            LightPropagationVolumesData addRadiancePropagationPass(FrameGraph::RZFrameGraph& framegraph, const LightPropagationVolumesData& LPV, const Maths::RZGrid& grid, u32 propagationIdx);
        };
    }    // namespace Graphics
}    // namespace Razix
