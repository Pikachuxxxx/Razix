#if 0

#include <Razix.h>

using namespace Razix;

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

class Sandbox : public Razix::RZApplication
{
private:
    struct ViewProjectionUniformBuffer
    {
        alignas(16)glm::mat4 view = glm::mat4(1.0f);
        alignas(16)glm::mat4 projection = glm::mat4(1.0f);

    }viewProjUBOData;

public:
    Sandbox() : RZApplication("/Sandbox/", "Shadow Mapping"), m_ActiveScene("shadows")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------
    }

    ~Sandbox() {}

    void OnStart() override
    {
        // Load scene
        m_ActiveScene.DeSerialiseScene("//Scenes/shadows.rzscn");

        auto& cameras = m_ActiveScene.GetComponentsOfType<CameraComponent>();
        if (!cameras.size()) {
            RZEntity& camera = m_ActiveScene.createEntity("Camera");
            camera.AddComponent<CameraComponent>();
            if (camera.HasComponent<CameraComponent>()) {
                CameraComponent& cc = camera.GetComponent<CameraComponent>();
                cc.Camera.setViewportSize(getWindow()->getWidth(), getWindow()->getHeight());
            }
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

            // Load resources
            armadilloModel = new Graphics::RZModel("//Meshes/armadillo.obj");
            teapotModel = new Graphics::RZModel("//Meshes/teapot.fbx");
            avacadoModel = new Graphics::RZModel("//Meshes/Avocado.gltf");
            
            sphereMesh = Graphics::MeshFactory::CreatePrimitive(Graphics::MeshPrimitive::Sphere);
        }
    }

    void OnUpdate(const RZTimestep& dt) override
    {
        // Update the camera
        auto& cameras = m_ActiveScene.GetComponentsOfType<CameraComponent>();
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

                offscreen_renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(1.0f, 0.5f, abs(sin(getTimer().GetElapsed())), 1.0f), offscreen_framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                Graphics::RZAPIRenderer::SetDepthBias(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                offscreen_pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::BindDescriptorSets(offscreen_pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), offscreen_descriptorSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());

                // TODO: Fix this!
                //auto shaderPushConstants = defaultShader->getPushConstants();
                Graphics::RZAPIRenderer::BindPushConstants(offscreen_pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                //planeVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                //quadIBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                //Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 6);

                // Draw a loaded 3D model
                auto meshes = armadilloModel->getMeshes();
                for (auto mesh : meshes)
                {
                    mesh->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                    mesh->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                    Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), mesh->getIndexCount());
                }

                // Draw a sphere
                //sphereMesh->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                //sphereMesh->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                //
                //Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), sphereMesh->getIndexCount());

                offscreen_renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                // Update the uniform buffer data
                viewProjUBOData.view = cameras[0].Camera.getViewMatrix();
                viewProjUBOData.projection = cameras[0].Camera.getProjection();
                viewProjUBOData.projection[1][1] *= -1;
                viewProjUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);
            }
            
            // Present the frame by executing the recorded commands
            //Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            //////////////////////////////////////////////////////////////////////////
            // On screen pass
            //Graphics::RZAPIRenderer::Begin();
            
#if 1
            {
                quad_renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(1.0f, 0.5f, abs(sin(getTimer().GetElapsed())), 1.0f), quad_framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());
            
                quad_pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            
                Graphics::RZAPIRenderer::BindDescriptorSets(quad_pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), quad_descriptorSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());
            
                quadVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                quadIBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            
                Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 6);
            
                quad_renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            }
#endif
            
            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

    void OnQuit() override
    {

        m_ActiveScene.SerialiseScene("//Scenes/shadows.rzscn");

        sphereMesh->Destroy();
        armadilloModel->Destroy();
        teapotModel->Destroy();
        avacadoModel->Destroy();

        // Delete the textures
        testTexture->Release();

        quadIBO->Destroy();
        quadVBO->Destroy();
        planeVBO->Destroy();

        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i]->Destroy();
        }

        for (auto sets : offscreen_descriptorSets) {
            auto set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }

        defaultShader->Destroy();
        quadShader->Destroy();

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

    Graphics::RZDepthTexture*                                                   shadow_depth_map;
    Graphics::RZDepthTexture*                                                   depthImageQuad;

    
    Graphics::RZVertexBuffer*                                                   planeVBO;
    Graphics::RZVertexBuffer*                                                   quadVBO;
    Graphics::RZIndexBuffer*                                                    quadIBO;
    Graphics::RZUniformBuffer*                                                  viewProjUniformBuffers[3];  // We also use 3 UBOs w.r.t to swap chain frames
    std::vector<Graphics::RZFramebuffer*>                                       offscreen_framebuffers;
    std::vector<Graphics::RZFramebuffer*>                                       quad_framebuffers;


    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>       offscreen_descriptorSets; // We use a single set per frame, so each frame has many sets that will be bind as a static sate with the cmdbuff being recorded
        std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>   quad_descriptorSets; 
    Graphics::RZShader*                                                         defaultShader;
    Graphics::RZShader*                                                         quadShader;

    Graphics::RZSwapchain*                                                      swapchain;

    // Offscreen Pass from Shadow POV
    Graphics::RZRenderPass*                                                     offscreen_renderpass;
    Graphics::RZPipeline*                                                       offscreen_pipeline;

    Graphics::RZRenderPass*                                                     quad_renderpass;
    Graphics::RZPipeline*                                                       quad_pipeline;

    uint32_t                                                                    width, height;
    RZScene                                                                     m_ActiveScene;

    Graphics::RZModel*                                                          armadilloModel;
    Graphics::RZModel*                                                          teapotModel;
    Graphics::RZModel*                                                          avacadoModel;

    Graphics::RZMesh*                                                           sphereMesh;

private:
    void buildPipelineResources()
    {
        testTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_256.png", "TextureAttachment1", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
       
        float plane_vertices[8 * 4] = {
           -2.5f, 0.0f, -2.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            2.5f, 0.0f, -2.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            2.5f, 0.0f,  2.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
           -2.5f, 0.0f,  2.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        float quad_vertices[5 * 4] = {
           -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
           -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
        };

        uint32_t indices[6] = {
            0, 1, 2, 2, 3, 0
        };

        Graphics::RZVertexBufferLayout planeBufferLayout;
        planeBufferLayout.push<glm::vec3>("Position");
        planeBufferLayout.push<glm::vec3>("Color");
        planeBufferLayout.push<glm::vec2>("TexCoord");
        planeVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 8 * 4, plane_vertices, Graphics::BufferUsage::STATIC);
        planeVBO->AddBufferLayout(planeBufferLayout);

        Graphics::RZVertexBufferLayout quadBufferLayout;
        quadBufferLayout.push<glm::vec3>("Position");
        quadBufferLayout.push<glm::vec2>("TexCoord");
        quadVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 5 * 4, quad_vertices, Graphics::BufferUsage::STATIC);
        quadVBO->AddBufferLayout(quadBufferLayout);

        quadIBO = Graphics::RZIndexBuffer::Create(indices, 6, Graphics::BufferUsage::STATIC);

        // Create the shader
        defaultShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/shadows.rzsf");
        quadShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/quad.rzsf");

        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);
            viewProjUniformBuffers[i]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            // get the descriptor infos to create the descriptor sets
            auto setInfos = defaultShader->getSetInfos();
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                        descriptor.texture = testTexture;
                    else
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                offscreen_descriptorSets[i].push_back(descSet);
            }
        }
    }

    void buildCommandPipeline()
    {
        RAZIX_TRACE("W : {0}, H : {1}", width, height);

        shadow_depth_map      = Graphics::RZDepthTexture::Create(width, height);
        depthImageQuad  = Graphics::RZDepthTexture::Create(width, height);

        quad_descriptorSets.clear();
        for (size_t i = 0; i < 3; i++) {
            // get the descriptor infos to create the descriptor sets
            auto setInfos = quadShader->getSetInfos();
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                        descriptor.texture = shadow_depth_map;
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                quad_descriptorSets[i].push_back(descSet);
            }
        }

        offscreen_pass();

        onscreen_pass();
    }

    void destroyCommandPipeline()
    {
        shadow_depth_map->Release(true);
        depthImageQuad->Release(true);

        for (auto sets : quad_descriptorSets) {
            auto set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }

        for (auto frameBuf : offscreen_framebuffers)
            frameBuf->Destroy();

        for (auto qframeBuf : quad_framebuffers)
            qframeBuf->Destroy();

        offscreen_renderpass->Destroy();
        quad_renderpass->Destroy();

        offscreen_pipeline->Destroy();
        quad_pipeline->Destroy();
    }

    void offscreen_pass()
    {
        // Off-screen render pass
        Graphics::AttachmentInfo offscreen_rp_textureTypes[1] = {
            { Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH }
        };

        Graphics::RenderPassInfo offscreen_rpInfo{};
        offscreen_rpInfo.attachmentCount = 1;
        offscreen_rpInfo.clear = true;
        offscreen_rpInfo.name = "off-screen pass";
        offscreen_rpInfo.textureType = offscreen_rp_textureTypes;

         offscreen_renderpass = Graphics::RZRenderPass::Create(offscreen_rpInfo);

        // Create the off screen pass graphics pipeline
        Graphics::PipelineInfo offscreen_pipelineInfo{};
        offscreen_pipelineInfo.cullMode             = Graphics::CullMode::NONE;
        offscreen_pipelineInfo.drawType             = Graphics::DrawType::TRIANGLE;
        offscreen_pipelineInfo.renderpass           = offscreen_renderpass;
        offscreen_pipelineInfo.shader               = defaultShader;
        offscreen_pipelineInfo.depthBiasEnabled     = true;
        offscreen_pipelineInfo.transparencyEnabled  = false;

        offscreen_pipeline = Graphics::RZPipeline::Create(offscreen_pipelineInfo);

        // Create the framebuffer

        offscreen_framebuffers.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            Graphics::RZTexture* attachments[1];
            //attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
            attachments[0] = shadow_depth_map;

            Graphics::FramebufferInfo frameBufInfo{};
            frameBufInfo.width = width;
            frameBufInfo.height = height;
            frameBufInfo.attachmentCount = 1;
            frameBufInfo.renderPass = offscreen_renderpass;
            frameBufInfo.attachments = attachments;

            offscreen_framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }

    void onscreen_pass()
    {
        // Create the render pass
        Graphics::AttachmentInfo textureTypes[2] = {
               { Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN },
               { Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH }
        };

        Graphics::RenderPassInfo renderPassInfo{};
        renderPassInfo.attachmentCount  = 2;
        renderPassInfo.textureType      = textureTypes;
        renderPassInfo.name             = "screen clear pass";
        renderPassInfo.clear            = true;

        quad_renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        Graphics::PipelineInfo pipelineInfo{};
        pipelineInfo.cullMode               = Graphics::CullMode::NONE;
        pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
        pipelineInfo.renderpass             = quad_renderpass;
        pipelineInfo.shader                 = quadShader;
        pipelineInfo.transparencyEnabled    = true;
        pipelineInfo.depthBiasEnabled       = false;

        quad_pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer

        auto swapImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();

        quad_framebuffers.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            Graphics::RZTexture* attachments[2];
            attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
            attachments[1] = depthImageQuad;

            Graphics::FramebufferInfo frameBufInfo{};
            frameBufInfo.width = width;
            frameBufInfo.height = height;
            frameBufInfo.attachmentCount = 2;
            frameBufInfo.renderPass = quad_renderpass;
            frameBufInfo.attachments = attachments;

            quad_framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}
#endif
