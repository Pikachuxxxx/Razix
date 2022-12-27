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

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

            void setGrid(const Maths::RZGrid& grid) { m_Grid = grid; }

        private:
            Maths::RZGrid                    m_Grid;
            RZPipeline*                      m_RSMPipeline;
            RZUniformBuffer*                 m_ModelViewProjectionSystemUBO = nullptr;
            ModelViewProjectionSystemUBOData m_ModelViewProjSystemUBOData;
            Graphics::RZDescriptorSet*       m_MVPDescriptorSet;
            Graphics::RZDescriptorSet*       m_RIDescriptorSet;
            std::vector<RZCommandBuffer*>    m_RSMCmdBuffers;
            std::vector<RZCommandBuffer*>    m_RadianceInjectionCmdBuffers;
            RZUniformBuffer*                 m_RadianceInjectionUBO = nullptr;
            RZPipeline*                      m_RIPipeline;
            RadianceInjectionUBOData         radianceInjectionData;

        private:
            ReflectiveShadowMapData     addRSMPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, const glm::mat4& lightViewProj, glm::vec3 lightIntensity);
            LightPropagationVolumesData addRadianceInjectionPass(FrameGraph::RZFrameGraph& framegraph, const ReflectiveShadowMapData& RSM, const Maths::RZGrid& grid);
        };
    }    // namespace Graphics
}    // namespace Razix
