#if 1

#include <Razix.h>

using namespace Razix;

class SceneManagerTestApp : public Razix::RZApplication
{
private:
    struct ViewProjectionUniformBuffer
    {
        alignas(16)glm::mat4 view = glm::mat4(1.0f);
        alignas(16)glm::mat4 projection = glm::mat4(1.0f);

    }viewProjUBOData;

    struct DirectionalLightUniformBuffer
    {
        alignas(16) glm::vec3 position;

        alignas(16) glm::vec3 ambient;
        alignas(16) glm::vec3 diffuse;
        alignas(16) glm::vec3 specular;

        float shininess;
        glm::vec3 viewPos;
    } directional_light_data;

public:
    SceneManagerTestApp() : RZApplication("/Sandbox/", "SceneManager Test App")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------
    }

    ~SceneManagerTestApp() {}

    void OnStart() override
    {

        //Get the current active scene
        Razix::RZEngine::Get().getSceneManager().loadScene(1);
        activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();

        if (!activeScene) {
            RAZIX_TRACE("Creatng new scene...");
            RZScene* modelLightScene = new RZScene("SceneManagerTest_Alt");
            Razix::RZEngine::Get().getSceneManager().enqueScene(modelLightScene);
            Razix::RZEngine::Get().getSceneManager().loadScene();
            activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
        }
       
        // Add entities to the scene programatically for the first time
        // Camera Entity
        auto& cameras = activeScene->GetComponentsOfType<CameraComponent>();
        if (!cameras.size()) {
            RZEntity& camera = activeScene->createEntity("Camera");
            camera.AddComponent<CameraComponent>();
            if (camera.HasComponent<CameraComponent>()) {
                CameraComponent& cc = camera.GetComponent<CameraComponent>();
                cc.Camera.setViewportSize(getWindow()->getWidth(), getWindow()->getHeight());
            }
        }

        width = getWindow()->getWidth();
        height = getWindow()->getHeight();

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            swapchain = Graphics::RZSwapchain::Create(getWindow()->getWidth(), getWindow()->getHeight());
        }
        else  if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {

            buildPipelineResources();
            buildCommandPipeline();

            Graphics::RZAPIRenderer::Init();

            // Add some model entities
            auto& modelEnitties = activeScene->GetComponentsOfType<Graphics::RZModel>();
            if (!modelEnitties.size()) {
                // Avocado
                auto& avocadoModelEntity = activeScene->createEntity("Avocado Angle Model");
                avocadoModelEntity.AddComponent<Graphics::RZModel>("//Meshes/Avocado.gltf");
                // Plane
                auto& planeEntity = activeScene->createEntity("Ground");
                planeEntity.AddComponent<MeshRendererComponent>(Graphics::MeshFactory::CreatePrimitive(Graphics::MeshPrimitive::Plane));
            }
        }
    }

    void OnUpdate(const RZTimestep& dt) override
    {
        if (Razix::RZInput::IsKeyPressed(KeyCode::Key::P)) {
            Razix::RZEngine::Get().getSceneManager().loadScene(1);
            activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
        }
        else if (Razix::RZInput::IsKeyPressed(KeyCode::Key::O)) {
            Razix::RZEngine::Get().getSceneManager().loadScene(0);
            activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
        }
        // Update the camera
        auto& cameras = activeScene->GetComponentsOfType<CameraComponent>();
        activeScene->getSceneCamera().Camera.update(dt.GetTimestepMs());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
            swapchain->Flip();
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.99f, 0.33f, 0.43f);

            Graphics::RZAPIRenderer::Begin();
            {
                Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(getWindow()->getWidth(), getWindow()->getHeight());

                renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(0.19f, 0.19f, 0.19f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::BindPushConstants(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), TransformComponent());
                Graphics::RZAPIRenderer::BindDescriptorSets(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), descriptorSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]);

                // draw related buffer bindings + Draw commands here
                {
                    // Draw the models
                    auto& mcs = activeScene->GetComponentsOfType<Graphics::RZModel>();
                    for (auto& mc : mcs) {
                        auto& meshes = mc.getMeshes();
                        for (auto& mesh : meshes) {
                            mesh->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                            mesh->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                            Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), mesh->getIndexCount());
                        }
                    }

                    // Draw the meshes
                    auto& mrcs = activeScene->GetComponentsOfType<MeshRendererComponent>();
                    for (auto& mrc : mrcs) {
                        mrc.Mesh->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                        mrc.Mesh->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                    
                        Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }

                }

                renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                // Update the uniform buffer data
                viewProjUBOData.view = cameras[0].Camera.getViewMatrix();
                viewProjUBOData.projection = cameras[0].Camera.getProjection();
                viewProjUBOData.projection[1][1] *= -1;
                viewProjUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

                // Update the lighting data
                directional_light_data.position = glm::vec3(-2.0f, 2.0f, 0.0f);// glm::vec3(2.2f, (5.0f * sin(getTimer().GetElapsedMS())), 1.0f);
                directional_light_data.ambient = glm::vec3(0.2f);
                directional_light_data.diffuse = glm::vec3(1.0f);
                directional_light_data.specular = glm::vec3(1.0f);
                directional_light_data.shininess = 32.0f;
                directional_light_data.viewPos = cameras[0].Camera.getPosition();
                dirLightUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(DirectionalLightUniformBuffer), &directional_light_data);
            }
            // Present the frame by executing the recorded commands
            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

    void OnQuit() override
    {
        // Save the current scene
        Razix::RZEngine::Get().getSceneManager().saveAllScenes();

        // Delete the models

        // Delete the textures
        albedoTexture->Release(true);
        roughness_metallicTexture->Release(true);

        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i]->Destroy();
            dirLightUniformBuffers[i]->Destroy();
        }

        for (auto& sets : descriptorSets) {
            auto& set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }

        phongLightingShader->Destroy();

        destroyCommandPipeline();

        Graphics::RZAPIRenderer::Release();
    }

    void OnResize(uint32_t width, uint32_t height) override
    {
        RAZIX_TRACE("Window Resize override sandbox application! | W : {0}, H : {1}", width, height);

        this->width = width;
        this->height = height;

        destroyCommandPipeline();

        Graphics::RZAPIRenderer::OnResize(width, height);

        buildCommandPipeline();
    }

private:
    Graphics::RZTexture2D* albedoTexture;
    Graphics::RZTexture2D* roughness_metallicTexture;

    Graphics::RZDepthTexture* depthImage;


    Graphics::RZUniformBuffer* viewProjUniformBuffers[3];  // We also use 3 UBOs w.r.t to swap chain frames
    Graphics::RZUniformBuffer* dirLightUniformBuffers[3];  // We also use 3 UBOs w.r.t to swap chain frames
    std::vector<Graphics::RZFramebuffer*>                                   framebuffers;


    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>   descriptorSets;
    Graphics::RZShader*                                                     phongLightingShader;

    Graphics::RZSwapchain* swapchain;

    Graphics::RZRenderPass*                                                 renderpass;
    Graphics::RZPipeline*                                                   pipeline;

    uint32_t                                                                width, height;

    // The current active scene that is rendered by the application
    RZScene* activeScene;

private:
    void buildPipelineResources()
    {
        // Load the textures
        albedoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/Avocado_baseColor.png", "Albedo", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
        roughness_metallicTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/Avocado_roughnessMetallic.png", "Specular", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);

        // Create the shader
        phongLightingShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/mesh_phong_lighting.rzsf");

        descriptorSets.clear();
        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);
            viewProjUniformBuffers[i]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            dirLightUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(DirectionalLightUniformBuffer), &directional_light_data);
            dirLightUniformBuffers[i]->SetData(sizeof(DirectionalLightUniformBuffer), &directional_light_data);

            // get the descriptor infos to create the descriptor sets
            auto setInfos = phongLightingShader->getSetInfos();

            int j = 0;
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                        if (!j) {
                            descriptor.texture = albedoTexture;
                            j++;
                        }
                        else
                            descriptor.texture = roughness_metallicTexture;
                    }
                    else if(setInfo.setID == 0 && descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                    else if (setInfo.setID == 1 && descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = dirLightUniformBuffers[i];
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                descriptorSets[i].push_back(descSet);
            }
        }
    }

    void buildCommandPipeline()
    {
        RAZIX_TRACE("W : {0}, H : {1}", width, height);

        // Depth Map
        depthImage = Graphics::RZDepthTexture::Create(width, height);

        // Create the render pass
        Graphics::AttachmentInfo textureTypes[2] = {
               { Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN },
               { Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH }
        };

        Graphics::RenderPassInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.textureType = textureTypes;
        renderPassInfo.name = "screen clear pass";
        renderPassInfo.clear = true;

        renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        Graphics::PipelineInfo pipelineInfo{};
        pipelineInfo.cullMode = Graphics::CullMode::NONE;
        pipelineInfo.drawType = Graphics::DrawType::TRIANGLE;
        pipelineInfo.renderpass = renderpass;
        pipelineInfo.shader = phongLightingShader;
        pipelineInfo.transparencyEnabled = true;
        pipelineInfo.depthBiasEnabled = false;

        pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer

        auto swapImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();

        framebuffers.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            Graphics::RZTexture* attachments[2];
            attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
            attachments[1] = depthImage;

            Graphics::FramebufferInfo frameBufInfo{};
            frameBufInfo.width = width;
            frameBufInfo.height = height;
            frameBufInfo.attachmentCount = 2;
            frameBufInfo.renderPass = renderpass;
            frameBufInfo.attachments = attachments;

            framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }

    void destroyCommandPipeline()
    {
        depthImage->Release(true);

        for (auto frameBuf : framebuffers)
            frameBuf->Destroy();

        // Destroy desc sets???

        renderpass->Destroy();

        pipeline->Destroy();
    }
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new SceneManagerTestApp();
}
#endif