#pragma once

#include "Razix/Graphics/Passes/IRZPass.h"

namespace Razix {
    namespace Graphics {

        enum class GaussianTap
        {
            Five     = 0,
            Nine     = 1,
            Thirteen = 2
        };

        enum class GaussianDirection
        {
            Horizontal,
            Vertical
        };

        struct GaussianGPUPCData
        {
            u32       filterTap;
            f32       blurRadius;
            glm::vec2 direction;
        };

        class RZGaussianBlurPass : public IRZPass
        {
        public:
            RZGaussianBlurPass() {}
            ~RZGaussianBlurPass() {}

            void addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings) override;
            void destroy() override;

            RAZIX_INLINE void setTwoPassFilter(bool value) { m_IsTwoPass = value; }
            RAZIX_INLINE void setBlurRadius(f32 value) { m_BlurRadius = value; }
            RAZIX_INLINE void setFilterTap(GaussianTap value) { m_FilterTap = value; }
            RAZIX_INLINE void setDirection(GaussianDirection value) { m_Direction = value; }

            // TODO: Utility method to set direction
        private:
            RZPipelineHandle  m_Pipeline;
            bool              m_IsTwoPass  = true;
            f32               m_BlurRadius = 1.0f;
            GaussianTap       m_FilterTap  = GaussianTap::Five;
            GaussianDirection m_Direction  = GaussianDirection::Horizontal;

        private:
            FrameGraph::RZFrameGraphResource addBlurPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings);
        };
    }    // namespace Graphics
}    // namespace Razix
