#pragma once

#include "Razix/Gfx/Passes/IRZPass.h"

namespace Razix {
    namespace Gfx {

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
            float2 direction;
        };

        class RZGaussianBlurPass : public IRZPass
        {
        public:
            RZGaussianBlurPass() {}
            ~RZGaussianBlurPass() {}

            void addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings) override;
            void destroy() override;

            RAZIX_INLINE RZFrameGraphResource getOutputTexture() { return m_OutputTexture; }

            RAZIX_INLINE void setTwoPassFilter(bool value) { m_IsTwoPass = value; }
            RAZIX_INLINE void setBlurRadius(f32 value) { m_BlurRadius = value; }
            RAZIX_INLINE void setFilterTap(GaussianTap value) { m_FilterTap = value; }
            RAZIX_INLINE void setDirection(GaussianDirection value) { m_Direction = value; }
            RAZIX_INLINE void setInputTexture(RZFrameGraphResource inputTexture) { m_InputTexture = inputTexture; }

            // TODO: Utility method to set direction
        private:
            RZPipelineHandle                 m_Pipeline      = {};
            bool                             m_IsTwoPass     = true;
            f32                              m_BlurRadius    = 1.0f;
            GaussianTap                      m_FilterTap     = GaussianTap::Five;
            GaussianDirection                m_Direction     = GaussianDirection::Horizontal;
            RZFrameGraphResource m_InputTexture  = {-1};
            RZFrameGraphResource m_OutputTexture = {-1};

        private:
            RZFrameGraphResource addBlurPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings, RZFrameGraphResource inputTexture, f32 blurRadius, GaussianTap filterTap, GaussianDirection direction);
        };
    }    // namespace Gfx
}    // namespace Razix
