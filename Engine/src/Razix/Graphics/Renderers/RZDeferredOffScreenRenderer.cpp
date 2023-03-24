// clang-format off
#include "rzxpch.h"
// clang-format on
#if 0
#include "Precompiled.h"
#include "DeferredOffScreenRenderer.h"
#include "Scene/Scene.h"
#include "Core/Application.h"
#include "Core/Engine.h"
#include "Scene/Component/TextureMatrixComponent.h"
#include "Maths/Maths.h"
#include "Maths/Transform.h"

#include "RenderGraph.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Mesh.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Graphics/GBuffer.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Framebuffer.h"
#include "Graphics/API/Texture.h"
#include "Graphics/API/UniformBuffer.h"
#include "Graphics/API/Renderer.h"
#include "Graphics/API/CommandBuffer.h"
#include "Graphics/API/Swapchain.h"
#include "Graphics/API/RenderPass.h"
#include "Graphics/API/Pipeline.h"
#include "Graphics/API/GraphicsContext.h"

#include <imgui/imgui.h>

#define MAX_LIGHTS 32
#define MAX_SHADOWMAPS 16
#define MAX_BONES 100

namespace Razix
{
    namespace Graphics
    {
        enum VSSystemUniformIndices : int32_t
        {
            VSSystemUniformIndex_ProjectionViewMatrix = 0,
            VSSystemUniformIndex_Size
        };

        enum PSSystemUniformIndices : int32_t
        {
            PSSystemUniformIndex_Lights = 0,
            PSSystemUniformIndex_Size
        };

        RZDeferredOffScreenRenderer::RZDeferredOffScreenRenderer(u32 width, u32 height)
        {
            m_ScreenRenderer = false;

            RZDeferredOffScreenRenderer::SetScreenBufferSize(width, height);
            RZDeferredOffScreenRenderer::Init();
        }

        RZDeferredOffScreenRenderer::~RZDeferredOffScreenRenderer()
        {
            delete m_UniformBuffer;
            delete m_AnimUniformBuffer;
            delete m_DefaultMaterial;

            delete[] m_VSSystemUniformBuffer;

            m_Framebuffers.clear();
        }

        void RZDeferredOffScreenRenderer::Init()
        {
            RAZIX_PROFILE_FUNCTION();
            m_Shader = Application::Get().GetShaderLibrary()->GetResource("//CoreShaders/DeferredColour.shader");
            m_AnimatedShader = Application::Get().GetShaderLibrary()->GetResource("//CoreShaders/DeferredColourAnim.shader");

            m_DefaultMaterial = new Material(m_Shader);

            Graphics::MaterialProperties properties;
            properties.albedoColour = Maths::Vector4(1.0f);
            properties.roughnessColour = Maths::Vector4(0.5f);
            properties.metallicColour = Maths::Vector4(0.5f);
            properties.usingAlbedoMap = 0.0f;
            properties.usingRoughnessMap = 0.0f;
            properties.usingNormalMap = 0.0f;
            properties.usingMetallicMap = 0.0f;
            m_DefaultMaterial->SetMaterialProperites(properties);

            auto shader = Application::Get().GetShaderLibrary()->GetResource("//CoreShaders/DeferredColour.shader");
            m_DefaultMaterial->SetShader(shader);

            const sz minUboAlignment = sz(Graphics::Renderer::GetCapabilities().UniformBufferOffsetAlignment);

            m_UniformBuffer = nullptr;
            m_AnimUniformBuffer = nullptr;

            m_CommandQueue.reserve(1000);

            //
            // Vertex shader System uniforms
            //
            m_VSSystemUniformBufferSize = sizeof(Maths::Matrix4);
            m_VSSystemUniformBuffer = new u8[m_VSSystemUniformBufferSize];
            memset(m_VSSystemUniformBuffer, 0, m_VSSystemUniformBufferSize);
            m_VSSystemUniformBufferOffsets.resize(VSSystemUniformIndex_Size);

            //Animated Vertex shader uniform
            m_VSSystemUniformBufferAnimSize = sizeof(Maths::Matrix4) * MAX_BONES;
            m_VSSystemUniformBufferAnim = new u8[m_VSSystemUniformBufferAnimSize];
            memset(m_VSSystemUniformBufferAnim, 0, m_VSSystemUniformBufferAnimSize);

            // Per Scene System Uniforms
            m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionViewMatrix] = 0;

            AttachmentInfo textureTypesOffScreen[5] = {
                { TextureType::COLOUR, Application::Get().GetRenderGraph()->GetGBuffer()->GetTextureFormat(SCREENTEX_COLOUR) },
                { TextureType::COLOUR, Application::Get().GetRenderGraph()->GetGBuffer()->GetTextureFormat(SCREENTEX_POSITION) },
                { TextureType::COLOUR, Application::Get().GetRenderGraph()->GetGBuffer()->GetTextureFormat(SCREENTEX_NORMALS) },
                { TextureType::COLOUR, Application::Get().GetRenderGraph()->GetGBuffer()->GetTextureFormat(SCREENTEX_PBR) },
                { TextureType::DEPTH, TextureFormat::DEPTH }
            };

            Graphics::RenderPassInfo renderpassCIOffScreen {};
            renderpassCIOffScreen.attachmentCount = 5;
            renderpassCIOffScreen.textureType = textureTypesOffScreen;

            m_RenderPass = Graphics::RenderPass::Get(renderpassCIOffScreen);

            Graphics::DescriptorInfo info {};
            info.layoutIndex = 0;
            info.shader = m_Shader.get();
            m_DescriptorSet.resize(1);
            m_DescriptorSet[0] = rzstl::Ref<Graphics::DescriptorSet>(Graphics::DescriptorSet::Create(info));

            CreatePipeline();
            CreateBuffer();
            CreateFramebuffer();

            m_DefaultMaterial->CreateDescriptorSet(1);

            m_ClearColour = Maths::Vector4(0.1f, 0.1f, 0.1f, 1.0f);
            m_CurrentDescriptorSets.resize(2);
        }

        void RZDeferredOffScreenRenderer::RenderScene()
        {
            RAZIX_PROFILE_FUNCTION();

            if(m_CommandQueue.empty())
            {
                m_HasRendered = false;
                // return;
            }

            m_HasRendered = true;

            Begin();
            SetSystemUniforms(m_Shader.get());
            Present();
            End();
        }

        void RZDeferredOffScreenRenderer::Begin()
        {
            RAZIX_PROFILE_FUNCTION();
            m_RenderPass->BeginRenderpass(Renderer::GetSwapchain()->GetCurrentCommandBuffer(), Maths::Vector4(0.0f), m_Framebuffers.front().get(), Graphics::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void RZDeferredOffScreenRenderer::BeginScene(Scene* scene, Camera* overrideCamera, Maths::Transform* overrideCameraTransform)
        {
            RAZIX_PROFILE_FUNCTION();
            m_CommandQueue.clear();
            {
                RAZIX_PROFILE_SCOPE("Get Camera");

                m_Camera = overrideCamera;
                m_CameraTransform = overrideCameraTransform;

                auto view = m_CameraTransform->GetWorldMatrix().Inverse();

                if(!m_Camera)
                {
                    return;
                }

                RAZIX_ASSERT(m_Camera, "No Camera Set for Renderer");
                auto projView = m_Camera->GetProjectionMatrix() * view;
                memcpy(m_VSSystemUniformBuffer + m_VSSystemUniformBufferOffsets[VSSystemUniformIndex_ProjectionViewMatrix], &projView, sizeof(Maths::Matrix4));

                m_Frustum = m_Camera->GetFrustum(view);
            }

            {
                auto& registry = scene->GetRegistry();
                auto group = registry.group<Model>(entt::get<Maths::Transform>);

                for(auto entity : group)
                {
                    const auto& [model, trans] = group.get<Model, Maths::Transform>(entity);
                    const auto& meshes = model.GetMeshes();

                    for(auto& mesh : meshes)
                    {
                        if(mesh->GetActive())
                        {
                            auto& worldTransform = trans.GetWorldMatrix();
                            Maths::Intersection inside;
                            {
                                RAZIX_PROFILE_SCOPE("Frustum Check");

                                inside = m_Frustum.IsInsideFast(mesh->GetBoundingBox()->Transformed(worldTransform));
                            }

                            if(inside == Maths::Intersection::OUTSIDE)
                                continue;

                            auto textureMatrixTransform = registry.try_get<TextureMatrixComponent>(entity);
                            SubmitMesh(mesh.get(), mesh->GetMaterial().get(), worldTransform, textureMatrixTransform ? textureMatrixTransform->GetMatrix() : Maths::Matrix4());
                        }
                    }
                }
            }
        }

        void RZDeferredOffScreenRenderer::Submit(const RenderCommand& command)
        {
            RAZIX_PROFILE_FUNCTION();
            m_CommandQueue.push_back(command);
        }

        void RZDeferredOffScreenRenderer::SubmitMesh(Mesh* mesh, Material* material, const Maths::Matrix4& transform, const Maths::Matrix4& textureMatrix)
        {
            RAZIX_PROFILE_FUNCTION();
            RenderCommand command;
            command.mesh = mesh;
            command.material = material ? material : m_DefaultMaterial;
            command.transform = transform;
            command.textureMatrix = textureMatrix;
            Submit(command);
        }

        void RZDeferredOffScreenRenderer::EndScene()
        {
        }

        void RZDeferredOffScreenRenderer::End()
        {
            RAZIX_PROFILE_FUNCTION();
            m_RenderPass->EndRenderpass(Renderer::GetSwapchain()->GetCurrentCommandBuffer());
        }

        void RZDeferredOffScreenRenderer::SetSystemUniforms(Shader* shader)
        {
            RAZIX_PROFILE_FUNCTION();
            m_UniformBuffer->SetData(m_VSSystemUniformBufferSize, *&m_VSSystemUniformBuffer);
            //Move as per mesh
            m_AnimUniformBuffer->SetData(m_VSSystemUniformBufferAnimSize, *&m_VSSystemUniformBufferAnim);
        }

        void RZDeferredOffScreenRenderer::Present()
        {
            RAZIX_PROFILE_FUNCTION();

            for(u32 i = 0; i < static_cast<u32>(m_CommandQueue.size()); i++)
            {
                Engine::Get().Statistics().NumRenderedObjects++;

                auto command = m_CommandQueue[i];
                Mesh* mesh = command.mesh;

                if(!command.material || !command.material->GetShader())
                    continue;

                auto commandBuffer = Renderer::GetSwapchain()->GetCurrentCommandBuffer();

                Graphics::PipelineInfo pipelineCreateInfo {};
                pipelineCreateInfo.shader = command.material->GetShader();
                pipelineCreateInfo.renderpass = m_RenderPass;
                pipelineCreateInfo.polygonMode = Graphics::PolygonMode::FILL;
                pipelineCreateInfo.cullMode = command.material->GetFlag(Material::RenderFlags::TWOSIDED) ? Graphics::CullMode::NONE : Graphics::CullMode::BACK;
                pipelineCreateInfo.transparencyEnabled = command.material->GetFlag(Material::RenderFlags::ALPHABLEND);

                auto pipeline = Graphics::Pipeline::Get(pipelineCreateInfo);

                pipeline->Bind(commandBuffer);

                command.material->Bind();

                m_CurrentDescriptorSets[SCENE_DESCRIPTORSET_ID] = m_DescriptorSet[SCENE_DESCRIPTORSET_ID].get();
                m_CurrentDescriptorSets[MATERIAL_DESCRIPTORSET_ID] = command.material->GetDescriptorSet();

                auto trans = command.transform;
                auto& pushConstants = command.material->GetShader()->GetPushConstants()[0];
                pushConstants.SetValue("transform", (void*)&trans);

                command.material->GetShader()->BindPushConstants(commandBuffer, pipeline.get());

                mesh->GetVertexBuffer()->Bind(commandBuffer, pipeline.get());
                mesh->GetIndexBuffer()->Bind(commandBuffer);

                Renderer::BindDescriptorSets(pipeline.get(), commandBuffer, 0, m_CurrentDescriptorSets);
                Renderer::DrawIndexed(commandBuffer, DrawType::TRIANGLE, mesh->GetIndexBuffer()->GetCount());

                mesh->GetVertexBuffer()->Unbind();
                mesh->GetIndexBuffer()->Unbind();
            }
        }

        void RZDeferredOffScreenRenderer::CreatePipeline()
        {
            RAZIX_PROFILE_FUNCTION();

            Graphics::PipelineInfo pipelineCreateInfo {};
            pipelineCreateInfo.shader = m_Shader;
            pipelineCreateInfo.renderpass = m_RenderPass;
            pipelineCreateInfo.polygonMode = Graphics::PolygonMode::FILL;
            pipelineCreateInfo.cullMode = Graphics::CullMode::BACK;
            pipelineCreateInfo.transparencyEnabled = false;

            m_Pipeline = Graphics::Pipeline::Get(pipelineCreateInfo);

            Graphics::BufferLayout vertexBufferLayoutAnim;

            Graphics::PipelineInfo pipelineCreateInfoAnim {};
            pipelineCreateInfoAnim.shader = m_AnimatedShader;
            pipelineCreateInfoAnim.renderpass = m_RenderPass;
            pipelineCreateInfoAnim.polygonMode = Graphics::PolygonMode::FILL;
            pipelineCreateInfoAnim.cullMode = Graphics::CullMode::BACK;
            pipelineCreateInfoAnim.transparencyEnabled = false;

            m_AnimatedPipeline = Graphics::Pipeline::Get(pipelineCreateInfoAnim);
        }

        void RZDeferredOffScreenRenderer::CreateBuffer()
        {
            RAZIX_PROFILE_FUNCTION();
            if(m_UniformBuffer == nullptr)
            {
                m_UniformBuffer = Graphics::UniformBuffer::Create();

                u32 bufferSize = m_VSSystemUniformBufferSize;
                m_UniformBuffer->Init(bufferSize, nullptr);
            }

            if(m_AnimUniformBuffer == nullptr)
            {
                m_AnimUniformBuffer = Graphics::UniformBuffer::Create();

                u32 bufferSize = m_VSSystemUniformBufferAnimSize;
                m_AnimUniformBuffer->Init(bufferSize, nullptr);
            }

            std::vector<Graphics::Descriptor> bufferInfos;

            Graphics::Descriptor bufferInfo = {};
            bufferInfo.buffer = m_UniformBuffer;
            bufferInfo.offset = 0;
            bufferInfo.size = m_VSSystemUniformBufferSize;
            bufferInfo.type = Graphics::DescriptorType::UNIFORM_BUFFER;
            bufferInfo.binding = 0;
            bufferInfo.shaderType = ShaderType::VERTEX;
            bufferInfo.name = "UniformBufferObject";
            bufferInfos.push_back(bufferInfo);

            m_DescriptorSet[0]->Update(bufferInfos);

            Graphics::Descriptor bufferInfoAnim = {};
            bufferInfoAnim.buffer = m_AnimUniformBuffer;
            bufferInfoAnim.offset = 0;
            bufferInfoAnim.size = m_VSSystemUniformBufferAnimSize;
            bufferInfoAnim.type = Graphics::DescriptorType::UNIFORM_BUFFER;
            bufferInfoAnim.binding = 1;
            bufferInfoAnim.shaderType = ShaderType::VERTEX;
            bufferInfoAnim.name = "UniformBufferObjectAnim";
            bufferInfos.push_back(bufferInfoAnim);

            //m_AnimatedDescriptorSets->Update(bufferInfos);
        }

        void RZDeferredOffScreenRenderer::CreateFramebuffer()
        {
            RAZIX_PROFILE_FUNCTION();
            const u32 attachmentCount = 5;
            TextureType attachmentTypes[attachmentCount];
            attachmentTypes[0] = TextureType::COLOUR;
            attachmentTypes[1] = TextureType::COLOUR;
            attachmentTypes[2] = TextureType::COLOUR;
            attachmentTypes[3] = TextureType::COLOUR;
            attachmentTypes[4] = TextureType::DEPTH;

            FramebufferInfo bufferInfo {};
            bufferInfo.width = m_ScreenBufferWidth;
            bufferInfo.height = m_ScreenBufferHeight;
            bufferInfo.attachmentCount = attachmentCount;
            bufferInfo.renderPass = m_RenderPass.get();
            bufferInfo.attachmentTypes = attachmentTypes;

            Texture* attachments[attachmentCount];
            attachments[0] = Application::Get().GetRenderGraph()->GetGBuffer()->GetTexture(SCREENTEX_COLOUR);
            attachments[1] = Application::Get().GetRenderGraph()->GetGBuffer()->GetTexture(SCREENTEX_POSITION);
            attachments[2] = Application::Get().GetRenderGraph()->GetGBuffer()->GetTexture(SCREENTEX_NORMALS);
            attachments[3] = Application::Get().GetRenderGraph()->GetGBuffer()->GetTexture(SCREENTEX_PBR);
            attachments[4] = Application::Get().GetRenderGraph()->GetGBuffer()->GetDepthTexture();
            bufferInfo.attachments = attachments;

            m_Framebuffers.push_back(rzstl::Ref<Framebuffer>(Framebuffer::Get(bufferInfo)));
        }

        void RZDeferredOffScreenRenderer::OnResize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTION();
            m_Framebuffers.clear();

            RZDeferredOffScreenRenderer::SetScreenBufferSize(width, height);

            CreateFramebuffer();
        }

        void RZDeferredOffScreenRenderer::OnImGui()
        {
            ImGui::TextUnformatted("Deferred Offscreen Renderer");
        }
    }
}
#endif
