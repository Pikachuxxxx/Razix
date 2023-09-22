#pragma once

#include "Razix/Graphics/Lighting/RZLight.h"

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/TileData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Maths/RZGrid.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        class RZDeferredLightingPass : public IRZPass
        {
        public:
            RZDeferredLightingPass()  = default;
            ~RZDeferredLightingPass() = default;

            void addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

            void setGrid(const Maths::RZGrid& grid) { m_Grid = grid; }

        private:
            Maths::RZGrid                 m_Grid;
            RZUniformBufferHandle         m_TileDataUBO;
            TileData                      m_TileData;
            RZUniformBufferHandle         m_LightDataUBO;
            RZMesh*                       m_ScreenQuadMesh;
            std::vector<RZDescriptorSet*> m_DescriptorSets;
            RZPipelineHandle              m_Pipeline;
            RZCommandBuffer*              m_CmdBuffers[RAZIX_MAX_SWAP_IMAGES_COUNT];
        };
    }    // namespace Graphics
}    // namespace Razix
