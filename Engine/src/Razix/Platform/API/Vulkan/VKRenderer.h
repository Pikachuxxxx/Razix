#pragma once

#include "Razix/Graphics/API/RZAPIRenderer.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

namespace Razix {
    namespace Graphics {
        
        class VKRenderer : public RZAPIRenderer
        {
        public:
            VKRenderer(uint32_t width, uint32_t height);
            ~VKRenderer();

        protected:
            void InitAPIImpl() override;
            void BeginAPIImpl() override;
            void PresentAPIImple(RZCommandBuffer* cmdBuffer) override;
            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) override;
            void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT) override;
        
        private:
            VKContext* m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
        };
    }
}

