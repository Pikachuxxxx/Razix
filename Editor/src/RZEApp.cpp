#pragma once

#include <Razix.h>

#include <QApplication>
#include <QVulkanInstance>

#include "RZENativeWindow.h"
#include "UI/Windows/RZEMainWindow.h"
#include "UI/Windows/RZEVulkanWindow.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

#include <vulkan/vulkan.h>

static QApplication* qrzeditorApp = nullptr;

using namespace Razix;

class RazixEditorApp : public Razix::RZApplication
{
public:
    Razix::Editor::RZEMainWindow    mainWindow;
    Razix::Editor::RZEVulkanWindow* vulkanWindow;

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

    // The current active scene that is rendered by the application
    Razix::RZScene*                                                              activeScene = nullptr;
    Razix::Graphics::RZTexture2D*                                                albedoTexture;
    Razix::Graphics::RZTexture2D*                                                roughness_metallicTexture;
    Razix::Graphics::RZDepthTexture*                                             depthImage;
    Razix::Graphics::RZUniformBuffer*                                            viewProjUniformBuffers[3];    // We also use 3 UBOs w.r.t to swap chain frames
    Razix::Graphics::RZUniformBuffer*                                            dirLightUniformBuffers[3];    // We also use 3 UBOs w.r.t to swap chain frames
    std::vector<Razix::Graphics::RZFramebuffer*>                                 framebuffers;
    std::unordered_map<uint32_t, std::vector<Razix::Graphics::RZDescriptorSet*>> descriptorSets;
    Razix::Graphics::RZShader*                                                   phongLightingShader;
    Razix::Graphics::RZSwapchain*                                                swapchain;
    Razix::Graphics::RZRenderPass*                                               renderpass;
    Razix::Graphics::RZPipeline*                                                 pipeline;
    uint32_t                                                                     width, height;

public:
    // TODO: In future we will pass multiple native window handles (for multiple viewports, debug rendering, content viewers etc) for now only a single viewport is sufficient
    RazixEditorApp()
        : RZApplication("/Sandbox/", "Razix Editor")
    {
        // Show the Editor Application after the engine static initialization is complete

        Razix::RZApplication::Get().setAppType(Razix::AppType::EDITOR);

        mainWindow.resize(1280, 720);
        mainWindow.show();

        // get the hwnd handle

        // create a native window derived form RZWindow and store this as the window handle pointer
        Razix::WindowProperties properties{};
        // TODO: Use the application signature title for this
        properties.Title  = "Razix Editor";
        properties.Width  = mainWindow.width();
        properties.Height = mainWindow.height();

        //HWND hWnd = (HWND) vulkanWindow->winId();

        HWND hWnd = (HWND) mainWindow.winId();

        Razix::Editor::RZENativeWindow::Construct();
        Razix::RZApplication::setViewportWindow(Razix::RZWindow::Create(&hWnd, properties));
        Razix::RZApplication::Get().setViewportHWND(hWnd);

        // Init Graphics Context
        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Initialize it
        RAZIX_CORE_INFO("Creating Graphics Context...");
        Graphics::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
        RAZIX_CORE_INFO("Initializing Graphics Context...");
        Graphics::RZGraphicsContext::GetContext()->Init();
        //-------------------------------------------------------------------------------------

        vulkanWindow = new Razix::Editor::RZEVulkanWindow;
        vulkanWindow->resize(1280, 720);
        vulkanWindow->show();
        vulkanWindow->setTitle("Vulkan Window");

        Razix::RZApplication::Get().Init();

        VkSurfaceKHR                surface = QVulkanInstance::surfaceForWindow(vulkanWindow);
        Razix::Graphics::VKContext* context = static_cast<Razix::Graphics::VKContext*>(Razix::Graphics::RZGraphicsContext::GetContext());
        context->CreateSurface(&surface);
        context->SetupDeviceAndSC();
    }

private:
    void RAZIX_CALL OnStart() override
    {
        //Razix::RZEngine::Get().getSceneManager().loadScene(0);
        //activeScene = Razix::RZEngine::Get().getSceneManager().getCurrentScene();

        if (!activeScene) {
            RAZIX_TRACE("Creatng new scene...");
            Razix::RZScene* editormodelLightScene = new Razix::RZScene("Editor_Scene_1");
            Razix::RZEngine::Get().getSceneManager().enqueScene(editormodelLightScene);
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

        // Add some model entities
        auto& modelEnitties = activeScene->GetComponentsOfType<Graphics::RZModel>();
        if (!modelEnitties.size()) {
            // Avocado
            auto& armadilloModelEntity = activeScene->createEntity("Avocado");
            armadilloModelEntity.AddComponent<Graphics::RZModel>("//Meshes/Avocado.gltf");
        }

        width  = getWindow()->getWidth();
        height = getWindow()->getHeight();

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN || Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            buildPipelineResources();
            buildCommandPipeline();

            Graphics::RZAPIRenderer::Init();

            getImGuiRenderer()->init();
            getImGuiRenderer()->createPipeline(*renderpass);
        }
    }

    void RAZIX_CALL OnUpdate(const Razix::RZTimestep& dt) override
    {
        // Update the camera
        auto& cameras = activeScene->GetComponentsOfType<CameraComponent>();
        activeScene->getSceneCamera().Camera.update(dt.GetTimestepMs());

#if 1
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
#endif

        // To avoid the blocking QApplication::exec() we use this and we nicely have a proper event update loop
        // But this makes the engine and editor to run the from the same main thread!!! (of course once the engine become multi-threaded this won't affect much as only a few systems would be on the main thread)
        // This also simplifies engine-editor communication for now
        vulkanWindow->getQVKInstance().presentQueued(vulkanWindow);
        qrzeditorApp->processEvents();
    }

    void RAZIX_CALL OnResize(uint32_t width, uint32_t height) override
    {
    }

    void RAZIX_CALL OnQuit() override
    {
        // Save the current scene
        Razix::RZEngine::Get().getSceneManager().saveAllScenes();

        getImGuiRenderer()->destroy();

        RAZIX_TRACE("Current Active Scene index : {0}", Razix::RZEngine::Get().getSceneManager().getCurrentSceneIndex());
        // Delete the models
        auto& mcs = activeScene->GetComponentsOfType<Graphics::RZModel>();
        for (Graphics::RZModel model: mcs)
            model.Destroy();

        //auto& mrcs = activeScene->GetComponentsOfType<MeshRendererComponent>();
        //for (auto& mesh : mrcs)
        //    mesh.Mesh->Destroy();

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

    //----------------------------------------------------------------

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
    RAZIX_INFO("Creating Razix Editor Application");

    qrzeditorApp = new QApplication(argc, argv);
    return new RazixEditorApp();
}