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

public:
    GridTest() : RZApplication("/Sandbox/", "GridTest")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------
    }

    ~GridTest() {}

    void OnStart() override
    {
        width = getWindow()->getWidth();
        height = getWindow()->getHeight();

        Graphics::RZAPIRenderer::Create(getWindow()->getWidth(), getWindow()->getHeight());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            offscreen_swapchain = Graphics::RZSwapchain::Create(getWindow()->getWidth(), getWindow()->getHeight());
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
        m_Camera.update(dt.GetTimestepMs());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
            offscreen_swapchain->Flip();
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

                offscreen_pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::BindDescriptorSets(offscreen_pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), offscreen_descripotrSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());

                // TODO: Fix this!
                //auto shaderPushConstants = defaultShader->getPushConstants();
                Graphics::RZAPIRenderer::BindPushConstants(offscreen_pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                triVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                triIBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 6);

                offscreen_renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                // Update the uniform buffer data
                viewProjUBOData.view = m_Camera.getViewMatrix();
                viewProjUBOData.projection = glm::perspective(glm::radians(45.0f), (float) getWindow()->getWidth() / getWindow()->getHeight(), 0.01f, 1000.0f);
                viewProjUBOData.projection[1][1] *= -1;
                viewProjUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);
            }
            // Present the frame by executing the recorded commands
            //Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            //////////////////////////////////////////////////////////////////////////
            //Graphics::RZAPIRenderer::OnResize(width, height);

            // Bind the Vertex and Index buffers
            //Graphics::RZAPIRenderer::Begin();
            {
                Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(getWindow()->getWidth(), getWindow()->getHeight());

                renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(1.0f, 0.5f, 1.0f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::BindDescriptorSets(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), descripotrSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());

                quadVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                triIBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 6);

                renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            }
            // Present the frame by executing the recorded commands
            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

    void OnQuit() override
    {
        // Delete the textures
        logoTexture->Release();
        testTexture->Release();

        triVBO->Destroy();
        triIBO->Destroy();
        quadVBO->Destroy();

        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i]->Destroy();
        }

        for (auto sets : offscreen_descripotrSets) {
            auto set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }

        for (auto sets : descripotrSets) {
            auto set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }

        offscreenRT->Release(true);

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
    Graphics::RZTexture2D* testTexture;
    Graphics::RZTexture2D* logoTexture;
    Graphics::RZDepthTexture* depthImage;
    Graphics::RZVertexBufferLayout                                              bufferLayout;
    Graphics::RZVertexBuffer* triVBO;
    Graphics::RZIndexBuffer* triIBO;
    Graphics::RZVertexBuffer* quadVBO;
    Graphics::RZUniformBuffer* viewProjUniformBuffers[3];  // We also use 3 UBOs w.r.t to swap chain frames
    Graphics::RZShader* defaultShader;
    Graphics::RZShader* quadShader;
    Graphics::RZShader* gridShader;
    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>       offscreen_descripotrSets; // We use a single set per frame, so each frame has many sets that will be bind as a static sate with the cmdbuff being recorded
    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>       descripotrSets; // We use a single set per frame, so each frame has many sets that will be bind as a static sate with the cmdbuff being recorded

    Graphics::RZRenderPass* offscreen_renderpass;
    std::vector<Graphics::RZFramebuffer*>                                       offscreen_framebuffers; // 3 FRAMEBU8FEFRS
    Graphics::RZPipeline* offscreen_pipeline;
    Graphics::RZSwapchain* offscreen_swapchain;

    Graphics::RZRenderPass* renderpass;
    std::vector<Graphics::RZFramebuffer*>                                       framebuffers; // 3 FRAMEBU8FEFRS
    Graphics::RZPipeline* pipeline;
    Graphics::RZSwapchain* swapchain;

    uint32_t                                                                    width, height;


    Graphics::Camera3D                                                          m_Camera;

    Graphics::RZRenderTexture* offscreenRT;

private:
    void buildPipelineResources()
    {
        testTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_256.png", "TextureAttachment1", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
        logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_512.png", "TextureAttachment2", Graphics::RZTexture::Wrapping::REPEAT);

        // Create the render targets as the same size of the resize cause it's fookin less code
        offscreenRT = Graphics::RZRenderTexture::Create(width, height, Graphics::RZTexture::Format::RGBA);

        float vertices[8 * 4] = {
           -2.5f, -2.5f, -2.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            2.5f, -2.5f, -2.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            2.5f, -2.5f,  2.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
           -2.5f, -2.5f,  2.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
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

        // This buffer layout will be somehow combined with the vertex buffers and passed to the pipeline for the Input Assembly stage
        bufferLayout.push<glm::vec3>("Position");
        bufferLayout.push<glm::vec3>("Color");
        bufferLayout.push<glm::vec2>("TexCoord");

        triVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 8 * 4, vertices, Graphics::BufferUsage::STATIC);
        triVBO->AddBufferLayout(bufferLayout);
        triIBO = Graphics::RZIndexBuffer::Create(indices, 6, Graphics::BufferUsage::STATIC);

        Graphics::RZVertexBufferLayout quadBufferLayout;
        quadBufferLayout.push<glm::vec3>("Position");
        quadBufferLayout.push<glm::vec2>("TexCoord");
        quadVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 5 * 4, quad_vertices, Graphics::BufferUsage::STATIC);
        quadVBO->AddBufferLayout(quadBufferLayout);

        // Create the shader
        defaultShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/default.rzsf");
        quadShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/quad_vignette.rzsf");

        for (size_t i = 0; i < 3; i++) {
            viewProjUniformBuffers[i] = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);
            viewProjUniformBuffers[i]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            // get the descriptor infos to create the descriptor sets
            auto setInfos = defaultShader->getSetInfos();
            int j = 0;
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                        if (!j) {
                            descriptor.texture = testTexture;
                            j++;
                        }
                        else
                            descriptor.texture = logoTexture;
                    }
                    else {
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                    }
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                offscreen_descripotrSets[i].push_back(descSet);
            }
        }

        for (size_t i = 0; i < 3; i++) {
            // get the descriptor infos to create the descriptor sets
            auto setInfos = quadShader->getSetInfos();
            int j = 0;
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                        if (!j) {
                            descriptor.texture = offscreenRT;
                            j++;
                        }
                        else
                            descriptor.texture = logoTexture;
                    }
                    else {
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                    }
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                descripotrSets[i].push_back(descSet);
            }
        }
    }

    void buildCommandPipeline()
    {

        RAZIX_TRACE("W : {0}, H : {1}", width, height);

        // Create the render pass
        Graphics::AttachmentInfo textureTypes[2] = {
               { Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::RGBA },
               { Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH }
        };

        Graphics::RenderPassInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.textureType = textureTypes;
        renderPassInfo.name = "Off-screen clear";
        renderPassInfo.clear = true;

        offscreen_renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        Graphics::PipelineInfo pipelineInfo{};
        pipelineInfo.cullMode = Graphics::CullMode::NONE;
        pipelineInfo.depthBiasEnabled = false;
        pipelineInfo.drawType = Graphics::DrawType::TRIANGLE;
        pipelineInfo.renderpass = offscreen_renderpass;
        pipelineInfo.shader = defaultShader;
        pipelineInfo.transparencyEnabled = false;

        offscreen_pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer
        Graphics::RZTexture::Type attachmentTypes[2];
        attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
        attachmentTypes[1] = Graphics::RZTexture::Type::DEPTH;

        auto swaoImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
        depthImage = Graphics::RZDepthTexture::Create(width, height);

        offscreenRT->Resize(width, height);

        offscreen_framebuffers.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            Graphics::RZTexture* attachments[2];
            attachments[0] = offscreenRT;
            attachments[1] = depthImage;

            Graphics::FramebufferInfo frameBufInfo{};
            frameBufInfo.width = width;
            frameBufInfo.height = height;
            frameBufInfo.attachmentCount = 2;
            frameBufInfo.renderPass = offscreen_renderpass;
            frameBufInfo.attachmentTypes = attachmentTypes;
            frameBufInfo.attachments = attachments;

            offscreen_framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }

        //////////////////////////////////////////////////////////////////////////
        // Prepare the on screen pipeline

        for (auto sets : descripotrSets) {
            auto set = sets.second;
            for (size_t i = 0; i < set.size(); i++) {
                set[i]->Destroy();
            }
        }
        descripotrSets.clear();
        for (size_t i = 0; i < 3; i++) {
            // get the descriptor infos to create the descriptor sets
            auto setInfos = quadShader->getSetInfos();
            int j = 0;
            for (auto& setInfo : setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor : setInfo.descriptors) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                        if (!j) {
                            descriptor.texture = offscreenRT;
                            j++;
                        }
                        else
                            descriptor.texture = logoTexture;
                    }
                    else {
                        descriptor.uniformBuffer = viewProjUniformBuffers[i];
                    }
                }
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                descripotrSets[i].push_back(descSet);
            }
        }

        // Create the render pass
        textureTypes[0].format = Graphics::RZTexture::Format::SCREEN;

        renderPassInfo.attachmentCount = 2;
        renderPassInfo.textureType = textureTypes;
        renderPassInfo.name = "On-screen clear";
        renderPassInfo.clear = true;
        renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        pipelineInfo.renderpass = renderpass;
        pipelineInfo.shader = quadShader;
        pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer
        framebuffers.clear();
        for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
            Graphics::RZTexture* on_screenattachments[2];
            on_screenattachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
            on_screenattachments[1] = depthImage;

            Graphics::FramebufferInfo frameBufInfo{};
            frameBufInfo.width = width;
            frameBufInfo.height = height;
            frameBufInfo.attachmentCount = 2;
            frameBufInfo.renderPass = renderpass;
            frameBufInfo.attachmentTypes = attachmentTypes;
            frameBufInfo.attachments = on_screenattachments;

            framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }

    void destroyCommandPipeline()
    {
        depthImage->Release(true);

        for (auto frameBuf : offscreen_framebuffers)
            frameBuf->Destroy();

        offscreen_pipeline->Destroy();

        offscreen_renderpass->Destroy();


        for (auto OnframeBuf : framebuffers)
            OnframeBuf->Destroy();

        pipeline->Destroy();

        renderpass->Destroy();
    }
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new GridTest();
}