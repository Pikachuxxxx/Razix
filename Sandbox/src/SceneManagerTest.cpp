#if 0
    #include <Razix.h>

using namespace Razix;

class SceneManagerTestApp : public Razix::RZApplication
{
private:
    struct ViewProjectionUniformBuffer
    {
        alignas(16) glm::mat4 view       = glm::mat4(1.0f);
        alignas(16) glm::mat4 projection = glm::mat4(1.0f);

    } viewProjUBOData;

    struct DirectionalLightUniformBuffer
    {
        alignas(16) glm::vec3 position;

        alignas(16) glm::vec3 ambient;
        alignas(16) glm::vec3 diffuse;
        alignas(16) glm::vec3 specular;

        float     shininess;
        glm::vec3 viewPos;
        float     _padding;
    } directional_light_data;

public:
    SceneManagerTestApp()
        : RZApplication("/Sandbox/", "SceneManager Test App")
    {
        Razix::RZInput::SelectGLFWInputManager();
        Razix::RZApplication::Get().Init();

        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------

        // Init Graphics Context
        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Initialize it
        RAZIX_CORE_INFO("Creating Graphics Context...");
        Graphics::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
        RAZIX_CORE_INFO("Initializing Graphics Context...");
        Graphics::RZGraphicsContext::GetContext()->Init();
        //-------------------------------------------------------------------------------------

        Razix::RZApplication::Get().Init();
    }

    ~SceneManagerTestApp() {}

    void OnStart() override
    {
        //Get the current active scene
        RZSceneManager::Get().loadScene(1);
        activeScene = RZSceneManager::Get().getCurrentScene();

        if (!activeScene) {
            RAZIX_TRACE("Creatng new scene...");
            RZScene* modelLightScene = new RZScene("Scene_2");
            RZSceneManager::Get().enqueScene(modelLightScene);
            RZSceneManager::Get().loadScene();
            activeScene = RZSceneManager::Get().getCurrentScene();
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

        auto scripts = activeScene->GetComponentsOfType<LuaScriptComponent>();
        if (!scripts.size()) {
            RZEntity scriptableEntity = activeScene->createEntity("ScriptableEntity");
            scriptableEntity.AddComponent<LuaScriptComponent>();
            if (scriptableEntity.HasComponent<LuaScriptComponent>()) {
                LuaScriptComponent& lsc = scriptableEntity.GetComponent<LuaScriptComponent>();
                lsc.loadScript("//Scripts/hello_razix.lua");
            }

            RZEntity imguiEntity = activeScene->createEntity("guiEntity");
            imguiEntity.AddComponent<LuaScriptComponent>();
            if (imguiEntity.HasComponent<LuaScriptComponent>()) {
                LuaScriptComponent& lsc = imguiEntity.GetComponent<LuaScriptComponent>();
                lsc.loadScript("//Scripts/imgui_test.lua");
            }
        }

        width  = getWindow()->getWidth();
        height = getWindow()->getHeight();

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN || Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            buildPipelineResources();
            buildCommandPipeline();

            Graphics::RZAPIRenderer::Init();

            getImGuiRenderer()->init();
            getImGuiRenderer()->createPipeline(*renderpass);

            // Add some model entities
            auto& modelEnitties = activeScene->GetComponentsOfType<Graphics::RZModel>();
            if (!modelEnitties.size()) {
                // Avocado
                auto& armadilloModelEntity = activeScene->createEntity("Armadillo");
                armadilloModelEntity.AddComponent<Graphics::RZModel>("//Meshes/Avocado.gltf");
                // Plane
                auto& planeEntity = activeScene->createEntity("Ground");
                planeEntity.AddComponent<MeshRendererComponent>(Graphics::MeshFactory::CreatePrimitive(Graphics::MeshPrimitive::Plane));
            }
        }
    }

    void OnUpdate(const RZTimestep& dt) override
    {
        if (Razix::RZInput::IsKeyPressed(KeyCode::Key::P)) {
            RZSceneManager::Get().loadScene(1);
            activeScene = RZSceneManager::Get().getCurrentScene();
        } else if (Razix::RZInput::IsKeyPressed(KeyCode::Key::O)) {
            RZSceneManager::Get().loadScene(0);
            activeScene = RZSceneManager::Get().getCurrentScene();
        }
        // Update the camera
        auto& cameras = activeScene->GetComponentsOfType<CameraComponent>();
        activeScene->getSceneCamera().Camera.update(dt.GetTimestepMs());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN || Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Graphics::RZAPIRenderer::Begin();
            {
                Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(getWindow()->getWidth(), getWindow()->getHeight());

                renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(0.99f, 0.33f, 0.43f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                auto tc = TransformComponent();
                //tc.Rotation = glm::vec3(sin(dt.GetElapsedMs()) * 25.0f, 0.0f, 0.0f) * dt.GetTimestepMs();
                glm::mat4 transform = tc.GetTransform();

                auto& modelMatrix = phongLightingShader->getPushConstants()[0];

                modelMatrix.data = glm::value_ptr(transform);
                modelMatrix.size = sizeof(glm::mat4);

                Graphics::RZAPIRenderer::BindPushConstant(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), modelMatrix);
                Graphics::RZAPIRenderer::BindDescriptorSets(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), descriptorSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]);

                // draw related buffer bindings + Draw commands here
                {
                    // Draw the models
                    auto& mcs = activeScene->GetComponentsOfType<Graphics::RZModel>();
                    for (auto& mc: mcs) {
                        auto& meshes = mc.getMeshes();
                        for (auto& mesh: meshes) {
                            mesh->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                            mesh->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                            Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), mesh->getIndexCount());
                        }
                    }

                    // Draw the meshes
                    auto& mrcs = activeScene->GetComponentsOfType<MeshRendererComponent>();
                    for (auto& mrc: mrcs) {
                        if (mrc.Mesh != nullptr) {
                            mrc.Mesh->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                            mrc.Mesh->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                            Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                        }
                    }
                }

                if (getImGuiRenderer()->update(dt))
                    getImGuiRenderer()->draw(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                // Update the uniform buffer data
                viewProjUBOData.view       = cameras[0].Camera.getViewMatrix();
                viewProjUBOData.projection = cameras[0].Camera.getProjection();
                if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN)
                    viewProjUBOData.projection[1][1] *= -1;
                viewProjUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

                // Update the lighting data
                directional_light_data.position  = glm::vec3(-2.0f, 2.0f, 0.0f);    // glm::vec3(2.2f, (5.0f * sin(getTimer().GetElapsedMS())), 1.0f);
                directional_light_data.ambient   = glm::vec3(0.2f);
                directional_light_data.diffuse   = glm::vec3(1.0f);
                directional_light_data.specular  = glm::vec3(1.0f);
                directional_light_data.shininess = 32.0f;
                directional_light_data.viewPos   = cameras[0].Camera.getPosition();
                dirLightUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(DirectionalLightUniformBuffer), &directional_light_data);
            }
            // Present the frame by executing the recorded commands
            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        } else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

    void OnQuit() override
    {
        // Save the current scene
        RZSceneManager::Get().saveAllScenes();

        getImGuiRenderer()->destroy();

        RAZIX_TRACE("Current Active Scene index : {0}", RZSceneManager::Get().getCurrentSceneIndex());
        // Delete the models
        auto& mcs = activeScene->GetComponentsOfType<Graphics::RZModel>();
        for (Graphics::RZModel model: mcs)
            model.Destroy();

        //auto& mrcs = activeScene->GetComponentsOfType<MeshRendererComponent>();
        //for (auto& mesh : mrcs)
        //    mesh.Mesh->Destroy();

        auto csidx = RZSceneManager::Get().getCurrentSceneIndex();
        RZSceneManager::Get().loadScene(csidx ? 0 : 1);
        activeScene = RZSceneManager::Get().getCurrentScene();
        mcs         = activeScene->GetComponentsOfType<Graphics::RZModel>();
        for (Graphics::RZModel model: mcs)
            model.Destroy();

        //mrcs = activeScene->GetComponentsOfType<MeshRendererComponent>();
        //for (auto& mesh : mrcs) {
        //    if(mesh.Mesh)
        //        mesh.Mesh->Destroy();
        //}

        // Delete the textures
        albedoTexture->Release(true);
        roughness_metallicTexture->Release(true);

        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            viewProjUniformBuffers[i]->Destroy();
            dirLightUniformBuffers[i]->Destroy();
        }

        for (auto& sets: descriptorSets) {
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

        this->width  = width;
        this->height = height;

        destroyCommandPipeline();

        Graphics::RZAPIRenderer::OnResize(width, height);

        buildCommandPipeline();
    }

    void OnImGui() override
    {
        // FIXME: This doesn't work IDK why
        //ImGui::Begin("App imgui window");
        //ImGui::Text("Scene manager test application");
        //ImGui::End();
    }

private:
    Graphics::RZTexture2D*                                                albedoTexture;
    Graphics::RZTexture2D*                                                roughness_metallicTexture;
    Graphics::RZDepthTexture*                                             depthImage;
    Graphics::RZUniformBuffer*                                            viewProjUniformBuffers[3];    // We also use 3 UBOs w.r.t to swap chain frames
    Graphics::RZUniformBuffer*                                            dirLightUniformBuffers[3];    // We also use 3 UBOs w.r.t to swap chain frames
    std::vector<Graphics::RZFramebuffer*>                                 framebuffers;
    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>> descriptorSets;
    Graphics::RZShader*                                                   phongLightingShader;
    Graphics::RZSwapchain*                                                swapchain;
    Graphics::RZRenderPass*                                               renderpass;
    Graphics::RZPipeline*                                                 pipeline;
    uint32_t                                                              width, height;

    // The current active scene that is rendered by the application
    RZScene* activeScene = nullptr;

private:
    void buildPipelineResources()
    {
        // Load the textures
        albedoTexture             = Graphics::RZTexture2D::CreateFromFile("//Textures/Avocado_baseColor.png", "Albedo", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
        roughness_metallicTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/Avocado_roughnessMetallic.png", "Specular", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);

        // Create the shader
        phongLightingShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/mesh_phong_lighting.rzsf");

        descriptorSets.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            viewProjUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData, "ViewProjectionUBO");
            viewProjUniformBuffers[i]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            dirLightUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(DirectionalLightUniformBuffer), &directional_light_data, "LightUBO");
            dirLightUniformBuffers[i]->SetData(sizeof(DirectionalLightUniformBuffer), &directional_light_data);

            // get the descriptor infos to create the descriptor sets
            auto& setInfos = phongLightingShader->getSetsCreateInfos();

            int j = 0;
            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                        if (!j) {
                            descriptor.texture = albedoTexture;
                            j++;
                        } else
                            descriptor.texture = roughness_metallicTexture;
                    } else if (setInfo.first == 0 && descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                    else if (setInfo.first == 1 && descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = dirLightUniformBuffers[i];
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second);
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
            {Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN},
            {Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH}};

        Graphics::RenderPassInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.textureType     = textureTypes;
        renderPassInfo.name            = "screen clear pass";
        renderPassInfo.clear           = true;

        renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        Graphics::PipelineInfo pipelineInfo{};
        pipelineInfo.cullMode            = Graphics::CullMode::NONE;
        pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
        pipelineInfo.renderpass          = renderpass;
        pipelineInfo.shader              = phongLightingShader;
        pipelineInfo.transparencyEnabled = true;
        pipelineInfo.depthBiasEnabled    = false;

        pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer

        auto swapImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();

        framebuffers.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            Graphics::RZTexture* attachments[2];
            attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
            attachments[1] = depthImage;

            Graphics::FramebufferInfo frameBufInfo{};
            frameBufInfo.width           = width;
            frameBufInfo.height          = height;
            frameBufInfo.attachmentCount = 2;
            frameBufInfo.renderPass      = renderpass;
            frameBufInfo.attachments     = attachments;

            framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }

    void destroyCommandPipeline()
    {
        if (depthImage)
            depthImage->Release(true);

        for (auto frameBuf: framebuffers)
            frameBuf->Destroy();

        renderpass->Destroy();

        pipeline->Destroy();
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new SceneManagerTestApp();
}

void main(int argc, char** argv)
{
    EngineMain(argc, argv);
}
#endif