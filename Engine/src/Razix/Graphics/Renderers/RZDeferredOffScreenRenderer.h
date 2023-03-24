#if 0
#pragma once

namespace Razix {

    namespace Graphics {
        class RZPipeline;
        class RZDescriptorSet;
        class RZGBuffer;
        class RZTexture2D;
        class RZTextureDepth;
        class RZTextureDepthArray;
        class RZSkyboxRenderer;
        class RZShader;
        class RZShadowRenderer;
        class RZFramebuffer;
        class RZMaterial;

        class RAZIX_EXPORT RZDeferredOffScreenRenderer : public IRZRenderer
        {
        public:
            RZDeferredOffScreenRenderer(uint32_t width, uint32_t height);
            ~RZDeferredOffScreenRenderer() override;

            void RenderScene() override;

            void Init() override;
            void Begin() override;
            void BeginScene(RZScene* scene) override;
            void Submit(const RenderCommand& command) override;
            void SubmitMesh(RZMesh* mesh, RZMaterial* material, const glm::mat4& transform, const glm::mat4& textureMatrix) override;
            void EndScene() override;
            void End() override;
            void Present() override;
            void OnResize(uint32_t width, uint32_t height) override;
            void PresentToScreen() override {}

            void CreatePipeline();
            void CreateBuffer();
            void CreateFramebuffer();

            void OnImGui() override;

            bool HadRendered() const { return m_HasRendered; }

        private:
            void SetSystemUniforms(Shader* shader);

            Material* m_DefaultMaterial;

            UniformBuffer* m_UniformBuffer;

            rzstl::Ref<Shader>                    m_AnimatedShader = nullptr;
            rzstl::Ref<Razix::Graphics::Pipeline> m_AnimatedPipeline;
            UniformBuffer*                 m_AnimUniformBuffer;
            uint8_t*                       m_VSSystemUniformBufferAnim     = nullptr;
            uint32_t                       m_VSSystemUniformBufferAnimSize = 0;

            struct UniformBufferModel
            {
                Maths::Matrix4* model;
            };

            UniformBufferModel m_UBODataDynamic;
            bool               m_HasRendered = false;
        };
    }    // namespace Graphics
}    // namespace Razix
#endif
