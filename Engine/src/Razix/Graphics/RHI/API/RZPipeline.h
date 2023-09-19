#pragma once

#include "Razix/Graphics/Resources/IRZResource.h"

namespace Razix {
    namespace Graphics {

        // Forward decelerations to reduce include files complexity
        struct RZPipelineDesc;
        class RZCommandBuffer;

        /**
         * NOTE:- The problem is though VK_EXT_dynamic_rendering eliminated render passes and framebuffers it still needs some before hand info while creating pipeline
         * What this means is that the Pipeline cannot be the sole property of the material, cause in a FrameGraphPass it might have multiple R/W attachments
         * and a multiple Materials will have different pipelines, since this is pre-baked info
         * one needs to re-adjust the materials uses in a FrameGraph pass during Frame Graph compilation phase
         */

        // TODO: Add presets to select blendings like Additive, Subtractive etc as in PhotoShop

        /* Pipeline binds all the resources together that are necessary to render geometry such as shaders, buffers, uniforms, descriptors and pipeline info */
        class RAZIX_API RZPipeline : public IRZResource<RZPipeline>
        {
        public:
            RZPipeline() = default;
            virtual ~RZPipeline() {}

            virtual void Bind(RZCommandBuffer* commandBuffer) = 0;

            RZPipelineDesc getDesc() { return m_Desc; }

        protected:
            RZPipelineDesc m_Desc;

        private:
            static void Create(void* where, const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Graphics
}    // namespace Razix