#pragma once

namespace Razix {
    namespace Graphics {

        class RZCommandBuffer;
        class RZPipeline;
        class RZDescriptorSet;

        enum class DataType
        {
            FLOAT,
            UNSIGNED_INT,
            UNSIGNED_BYTE
        };

        /* The Razix API Renderer provides a interface and a set of common methods that abstracts over other APIs rendering implementation
         * The Renderers creates from the provided IRZRenderer interface of razix uses this to perform command recording/submission sets binding
         * and other operations that doesn't require the underlying API, since renderers do not actually need that we use this high-level abstraction
         * over the supported APIs to make things look simple and easier to interact with
         */
        class RAZIX_API RZAPIRenderer
        {
        public:
            RZAPIRenderer() = default;
            virtual ~RZAPIRenderer() = default;

            static void Create(uint32_t width, uint32_t height);

            inline static void Init() { s_APIInstance->InitAPIImpl(); }
            inline static void Begin() { s_APIInstance->BeginAPIImpl(); }
            inline static void Present(RZCommandBuffer* cmdBuffer) { s_APIInstance->PresentAPIImple(cmdBuffer); }
            inline static void BindDescriptorSets(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) { s_APIInstance->BindDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets); }
            inline static void Draw(RZCommandBuffer* cmdBuffer, uint32_t count, DataType dataType = DataType::UNSIGNED_INT) { s_APIInstance->DrawAPIImpl(cmdBuffer, count, dataType); }

        protected:
            virtual void InitAPIImpl() = 0;
            virtual void BeginAPIImpl() = 0;
            virtual void PresentAPIImple(RZCommandBuffer* cmdBuffer) = 0;
            virtual void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) = 0;
            virtual void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT) = 0;

        protected:
            static RZAPIRenderer*   s_APIInstance;

            std::string             m_RendererTitle;    /* The name of the renderer API that is being used */
            uint32_t                m_Width;
            uint32_t                m_Height;
        };
    }
}
