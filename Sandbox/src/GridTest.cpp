#if 0
    #pragma once

    #include <Razix.h>

using namespace Razix;

    #define GLM_FORCE_DEPTH_ZERO_TO_ONE

class GridTest : public Razix::RZApplication
{
private:
    struct ViewProjectionUniformBuffer
    {
        alignas(16)glm::mat4 view = glm::mat4(1.0f);
        alignas(16)glm::mat4 projection = glm::mat4(1.0f);

    }viewProjUBOData;

    struct GridUniformBuffer
    {
        alignas(4)glm::vec3 CameraPosition = glm::vec3(0.0f);
        float _padding = -1.0f;
        float Scale = 1.0f;
        float Res = 1.5f;
        float MaxDistance = 600.0f;
        float _padding2 = -1.0f;
    }gridData{};

public:
    GridTest() : RZApplication("/Sandbox/", "GridTest"), m_ActiveScene("Grid Test Scene")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------

        // Test Razix UUID Generation Here
        RZUUID uuid;

        std::string bytes = uuid.bytes();

        RAZIX_TRACE("UUID (16byte string): {0}", bytes);

        std::string s = uuid.str();

        RAZIX_TRACE("UUID (36 byte string): {0}", s);

        //for (size_t i = 0; i < 100; ++i) {
        //    RZUUID uuid;
        //    std::string s = uuid.str();
        //    std::cout << "RZUUID: " << s << std::endl;
        //}

        //std::vector<RZEntity> entities;
        //for (int i = 0; i < 100; i++) {
        //    RZEntity entity = m_ActiveScene.createEntity("Camera");
        //    RAZIX_TRACE("Entity name : {0} | UUID : {1}", entity.GetComponent<TagComponent>().Tag, entity.GetComponent<IDComponent>().UUID);
        //    //entities.push_back(entity);
        //}
    }

    ~GridTest() {}

    void OnStart() override
    {
        m_ActiveScene.DeSerialiseScene("//Scenes/gridtest.rzscn");

        // TODO: Get the components as references, such that If I update the camera it should update it's contents basically it should work!!! (dumb comment but describes the bug well)

        //RZEntity& camera = m_ActiveScene.createEntity("Camera");
        //camera.AddComponent<CameraComponent>();

        //RAZIX_TRACE("Does have camera : {0}", camera.HasComponent<CameraComponent>());

        //if (camera.HasComponent<CameraComponent>()) {
        //    CameraComponent& trans = camera.GetComponent<CameraComponent>();
        //    trans.Camera.setViewportSize(getWindow()->getWidth(), getWindow()->getHeight());
        //}

        //std::cout << "Entities count in scene : " << entities.size() << std::endl;

        auto& uuids = m_ActiveScene.GetComponentsOfType<CameraComponent>();
        for (size_t i = 0; i < uuids.size(); i++) {
            //RAZIX_TRACE("Entity name : {0}", uuids[i].Tag);
            //RAZIX_TRACE("Camera Position : {0}", glm::to_string(uuids[i].Camera.getPosition()));
            //std::cout << uuids[i].Primary;
        }

        width = getWindow()->getWidth();
        height = getWindow()->getHeight();

        Graphics::RZAPIRenderer::Create(getWindow()->getWidth(), getWindow()->getHeight());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            swapchain = Graphics::RZSwapchain::Create(getWindow()->getWidth(), getWindow()->getHeight());
        }
        else  if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {

            buildPipelineResources();
            buildCommandPipeline();

            Graphics::RZAPIRenderer::Init();
        }
    }

    void OnUpdate(const RZTimestep& dt) override
    {
        // Update the camera
        //m_Camera.update(dt.GetTimestepMs());

        auto& uuids = m_ActiveScene.GetComponentsOfType<CameraComponent>();
        for (size_t i = 0; i < uuids.size(); i++) {
            //RAZIX_TRACE("Entity name : {0}", uuids[i].Tag);
            RAZIX_TRACE("Camera Position : {0}", glm::to_string(uuids[i].Camera.getPosition()));
            //std::cout << uuids[i].Primary;
        }

        //uuids[0].Camera.update(dt.GetTimestepMs());
        m_ActiveScene.GetSceneCamera().Camera.update(dt.GetTimestepMs());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
            swapchain->Flip();
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.99f, 0.33f, 0.43f);

            //RAZIX_TRACE("FPS : {0}", RZEngine::Get().GetStatistics().FramesPerSecond);

            //////////////////////////////////////////////////////////////////////////
            // Offscreen pass
            // Bind the Vertex and Index buffers

            Graphics::RZAPIRenderer::Begin();
            {
                Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(getWindow()->getWidth(), getWindow()->getHeight());

                //RAZIX_TRACE("Elapsed time : {0}", getTimer().GetElapsed());

                renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::BindDescriptorSets(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), grid_descriptorSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());

                // TODO: Fix this!
                //auto shaderPushConstants = defaultShader->getPushConstants();
                //Graphics::RZAPIRenderer::BindPushConstants(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                gridVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                gridIBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 6);

                renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                // Update the uniform buffer data
                viewProjUBOData.view = uuids[0].Camera.getViewMatrix(); //m_Camera.getViewMatrix();//
                viewProjUBOData.projection = uuids[0].Camera.getProjection();// glm::perspective(glm::radians(45.0f), (float) getWindow()->getWidth() / getWindow()->getHeight(), 0.01f, 1000.0f); //
                viewProjUBOData.projection[1][1] *= -1;
                viewProjUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

                gridData.CameraPosition = m_Camera.getPosition();
                gridData.MaxDistance = 6000.0f;
                gridData.Res = 1.5f;
                gridData.Scale = 100.0f;
                gridUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(GridUniformBuffer), &gridData);

            }
            // Present the frame by executing the recorded commands
            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

    void OnQuit() override
    {
        m_ActiveScene.SerialiseScene("//Scenes/gridtest.rzscn");

        // Delete the textures
        logoTexture->Release();
        testTexture->Release();

        gridVBO->Destroy();
        gridIBO->Destroy();
         
        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i]->Destroy();
            gridUniformBuffers[i]->Destroy();
        }

        for (auto sets : grid_descriptorSets) {
            auto set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }

        gridShader->Destroy();

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
    Graphics::RZTexture2D*                                                      testTexture;
    Graphics::RZTexture2D*                                                      logoTexture;
    Graphics::RZDepthTexture*                                                   depthImage;
    Graphics::RZVertexBufferLayout                                              bufferLayout;
    Graphics::RZVertexBuffer*                                                   gridVBO;
    Graphics::RZIndexBuffer*                                                    gridIBO;
    Graphics::RZUniformBuffer*                                                  viewProjUniformBuffers[3];  // We also use 3 UBOs w.r.t to swap chain frames
    Graphics::RZUniformBuffer*                                                  gridUniformBuffers[3];
    Graphics::RZShader*                                                         gridShader;
    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>       grid_descriptorSets; // We use a single set per frame, so each frame has many sets that will be bind as a static sate with the cmdbuff being recorded

    Graphics::RZRenderPass*                                                     renderpass;
    std::vector<Graphics::RZFramebuffer*>                                       framebuffers; // 3 FRAMEBU8FEFRS
    Graphics::RZPipeline*                                                       pipeline;
    Graphics::RZSwapchain*                                                      swapchain;
    uint32_t                                                                    width, height;
    Graphics::Camera3D                                                          m_Camera;

    RZScene                                                                     m_ActiveScene;
    CameraComponent                                                             m_SceneCameraComponent;
private:
    void buildPipelineResources()
    {
        testTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_256.png", "TextureAttachment1", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
        logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_512.png", "TextureAttachment2", Graphics::RZTexture::Wrapping::REPEAT);

        float vertices[8 * 4] = {
           -2000.5f, -2.5f, -2000.5f, 0.0f, 0.0f,
            2000.5f, -2.5f, -2000.5f, 1.0f, 0.0f,
            2000.5f, -2.5f,  2000.5f, 1.0f, 1.0f,
           -2000.5f, -2.5f,  2000.5f, 0.0f, 1.0f
        };

        uint32_t indices[6] = {
            0, 1, 2, 2, 3, 0
        };

        // This buffer layout will be somehow combined with the vertex buffers and passed to the pipeline for the Input Assembly stage
        bufferLayout.push<glm::vec3>("Position");
        bufferLayout.push<glm::vec2>("TexCoord");

        gridVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 8 * 4, vertices, Graphics::BufferUsage::STATIC);
        gridVBO->AddBufferLayout(bufferLayout);
        gridIBO = Graphics::RZIndexBuffer::Create(indices, 6, Graphics::BufferUsage::STATIC);

        // Create the shader
        gridShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/grid.rzsf");

        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);
            viewProjUniformBuffers[i]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            gridUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(GridUniformBuffer), &gridData);
            gridUniformBuffers[i]->SetData(sizeof(GridUniformBuffer), &gridData);

            // get the descriptor infos to create the descriptor sets
            auto setInfos = gridShader->getSetInfos();
            int j = 0;
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (!j) {
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                        j++;
                    }
                    else
                        descriptor.uniformBuffer = gridUniformBuffers[i];
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                grid_descriptorSets[i].push_back(descSet);
            }
        }
    }

    void buildCommandPipeline()
    {

        RAZIX_TRACE("W : {0}, H : {1}", width, height);

        // Create the render pass
        Graphics::AttachmentInfo textureTypes[2] = {
               { Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN },
               { Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH }
        };

        Graphics::RenderPassInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.textureType = textureTypes;
        renderPassInfo.name = "Off-screen clear";
        renderPassInfo.clear = true;

        renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        Graphics::PipelineInfo pipelineInfo{};
        pipelineInfo.cullMode = Graphics::CullMode::NONE;
        pipelineInfo.depthBiasEnabled = false;
        pipelineInfo.drawType = Graphics::DrawType::TRIANGLE;
        pipelineInfo.renderpass = renderpass;
        pipelineInfo.shader = gridShader;
        pipelineInfo.transparencyEnabled = true;

        pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer
        Graphics::RZTexture::Type attachmentTypes[2];
        attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
        attachmentTypes[1] = Graphics::RZTexture::Type::DEPTH;

        auto swaoImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
        depthImage = Graphics::RZDepthTexture::Create(width, height);

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
            frameBufInfo.attachmentTypes = attachmentTypes;
            frameBufInfo.attachments = attachments;

            framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }

    void destroyCommandPipeline()
    {
        depthImage->Release(true);

        for (auto frameBuf : framebuffers)
            frameBuf->Destroy();

        renderpass->Destroy();

        pipeline->Destroy();
    }
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new GridTest();
}
#endif