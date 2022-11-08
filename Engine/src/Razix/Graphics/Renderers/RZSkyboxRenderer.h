#if 0
#pragma once

#include "IRenderer.h"

namespace Razix
{
    namespace Graphics
    {
        class TextureDepth;
        class Texture;
        class Shader;

        class RAZIX_API RZSkyboxRenderer : public IRZRenderer
        {
        public:
            RZSkyboxRenderer(uint32_t width, uint32_t height);
            ~RZSkyboxRenderer();

            void Init() override;
            void BeginScene(Scene* scene, Camera* overrideCamera) override;
            void OnResize(uint32_t width, uint32_t height) override;
            void CreateGraphicsPipeline();
            void SetCubeMap(Texture* cubeMap);
            void UpdateUniformBuffer();

            void Begin() override;
            void Submit(const RenderCommand& command) override {};
            void SubmitMesh(Mesh* mesh, Material* material, const Maths::Matrix4& transform, const Maths::Matrix4& textureMatrix) override {};
            void EndScene() override {};
            void End() override;
            void Present() override {};
            void RenderScene() override;
            void PresentToScreen() override { }

            void CreateFramebuffers();

            struct UniformBufferObject
            {
                Razix::Maths::Matrix4 invprojview;
            };

            void SetRenderTarget(Texture* texture, bool rebuildFramebuffer) override;

            void OnImGui() override;

        private:
            void SetSystemUniforms(Shader* shader) const;

            Razix::Graphics::UniformBuffer* m_UniformBuffer;

            uint32_t m_CurrentBufferID = 0;

            Mesh* m_Skybox;
            Texture* m_CubeMap;
        };
    }
}
#endif
