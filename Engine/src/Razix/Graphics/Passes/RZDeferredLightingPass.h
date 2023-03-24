#pragma once

#include "Razix/Graphics/Lighting/RZLight.h"

#include "Razix/Graphics/Passes/IRZPass.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/SceneColorData.h"
#include "Razix/Graphics/Passes/Data/TileData.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Maths/RZGrid.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;

        struct GPULightData
        {
            u32  numLights;
            u32  _padding;
            LightData data;
        };

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
            RZUniformBuffer*              m_TileDataUBO;
            TileData                      m_TileData;
            GPULightData                  m_GPULightData;
            RZUniformBuffer*              m_LightDataUBO;
            RZMesh*                       m_ScreenQuadMesh;
            std::vector<RZDescriptorSet*> m_DescriptorSets;
        };
    }    // namespace Graphics
}    // namespace Razix
