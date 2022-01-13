#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::RZApplication
{
private:
    struct ViewProjectionUniformBuffer
    {
        alignas(16)glm::mat4 view       = glm::mat4(1.0f);
        alignas(16)glm::mat4 projection = glm::mat4(1.0f);

    }viewProjUBOData;

public:
    Sandbox() : RZApplication("/Sandbox/","Sandbox")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------
    }

    ~Sandbox()              
    {

    }

    void OnStart() override
    {
        Graphics::RZAPIRenderer::Create(getWindow()->getWidth(), getWindow()->getHeight());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
          swapchain = Graphics::RZSwapchain::Create(getWindow()->getWidth(), getWindow()->getHeight());
        }
        else  if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {

            Graphics::RZTexture::Filtering filtering = {};
            filtering.minFilter = Graphics::RZTexture::Filtering::FilterMode::LINEAR;
            filtering.magFilter = Graphics::RZTexture::Filtering::FilterMode::LINEAR;

            Graphics::RZTexture2D* testTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_256.png", "TextureAttachment1", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE, filtering);
            Graphics::RZTexture2D* logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_512.png", "TextureAttachment2", Graphics::RZTexture::Wrapping::REPEAT, filtering);

            float vertices[8 * 4] = {
               -2.5f, -2.5f, -2.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                2.5f, -2.5f, -2.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                2.5f,  2.5f, -2.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
               -2.5f,  2.5f, -2.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
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

            // Create the shader
            defaultShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/default.rzsf");

      
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
                    descripotrSets[i].push_back(descSet);
                }
            }
            

            // Create the render pass
            Graphics::AttachmentInfo textureTypes[2] = {
                   { Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN },
                   { Graphics::RZTexture::Type::DEPTH, Graphics::RZTexture::Format::DEPTH }
            };

            Graphics::RenderPassInfo renderPassInfo{};
            renderPassInfo.attachmentCount = 2;
            renderPassInfo.textureType = textureTypes;
            renderPassInfo.name = "screen clear";
            renderPassInfo.clear = true;

            renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

            // Create the graphics pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode = Graphics::CullMode::NONE;
            pipelineInfo.depthBiasEnabled = false;
            pipelineInfo.drawType = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass = renderpass;
            pipelineInfo.shader = defaultShader;
            pipelineInfo.transparencyEnabled = false;

            pipeline = Graphics::RZPipeline::Create(pipelineInfo);

            // Create the framebuffer
            Graphics::RZTexture::Type attachmentTypes[2];
            attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
            attachmentTypes[1] = Graphics::RZTexture::Type::DEPTH;

            auto swaoImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
            auto depthImage = Graphics::RZDepthTexture::Create(getWindow()->getWidth(), getWindow()->getHeight());

            //testPassTexture = Graphics::RZTexture2D::Create("test", 1200, 720, nullptr, Graphics::RZTexture::Format::SCREEN, Graphics::RZTexture::Wrapping::REPEAT, filtering);

            for (size_t i = 0; i < Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount(); i++) {
                Graphics::RZTexture* attachments[2];
                attachments[0] = Graphics::RZAPIRenderer::getSwapchain()->GetImage(i);
                attachments[1] = depthImage;

                Graphics::FramebufferInfo frameBufInfo{};
                frameBufInfo.width = getWindow()->getWidth();
                frameBufInfo.height = getWindow()->getHeight();
                frameBufInfo.attachmentCount = 2;
                frameBufInfo.renderPass = renderpass;
                frameBufInfo.attachmentTypes = attachmentTypes;
                frameBufInfo.attachments = attachments;

                framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
            }

            Graphics::RZAPIRenderer::Init();
        }
        
    }

    void OnUpdate(const RZTimestep& dt) override 
    {
        // Update the camera
        m_Camera.update(dt.GetTimestepMs());

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
            swapchain->Flip();
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.99f, 0.33f, 0.43f);

            if (RZInput::IsMouseButtonPressed(KeyCode::MouseKey::ButtonRight))
                RAZIX_TRACE("Mouse right is pressed");
            if (RZInput::IsMouseButtonHeld(KeyCode::MouseKey::ButtonLeft))
                RAZIX_TRACE("Mouse left is held");

            // Bind the Vertex and Index buffers
            Graphics::RZAPIRenderer::Begin();

            //RAZIX_TRACE("Elapsed time : {0}", getTimer().GetElapsed());
            //RAZIX_TRACE("FPS : {0}", RZEngine::Get().GetStatistics().FramesPerSecond);

            renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(1.0f, 0.5f, abs(sin(getTimer().GetElapsed())), 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

            pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            Graphics::RZAPIRenderer::BindDescriptorSets(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), descripotrSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());

            //auto shaderPushConstants = defaultShader->getPushConstants();
            // TODO: Fix this!
            Graphics::RZAPIRenderer::BindPushConstants(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            triVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            triIBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            Graphics::RZAPIRenderer::DrawIndexed(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 6);

            renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            // Update the uniform buffer data
            viewProjUBOData.view = m_Camera.getViewMatrix();//glm::mat4(1.0f);//
            viewProjUBOData.projection = glm::perspective(glm::radians(45.0f), (float) getWindow()->getWidth() / getWindow()->getHeight(), 0.01f, 100.0f);
            viewProjUBOData.projection[1][1] *= -1;
            viewProjUniformBuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()]->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

private:
    Graphics::RZVertexBufferLayout                                              bufferLayout;
    Graphics::RZVertexBuffer*                                                   triVBO;
    Graphics::RZIndexBuffer*                                                    triIBO;
    Graphics::RZUniformBuffer*                                                  viewProjUniformBuffers[3];  // We also use 3 UBOs wrt to swap chain frames
    Graphics::RZShader*                                                         defaultShader;
    std::unordered_map<uint32_t, std::vector<Graphics::RZDescriptorSet*>>       descripotrSets; // We use a single set per frame, so each frame has many sets that will be bind as a static sate with the cmdbuf being recorded
    Graphics::RZRenderPass*                                                     renderpass;
    std::vector<Graphics::RZFramebuffer*>                                       framebuffers; // 3 FRAMEBU8FEFRS
    Graphics::RZPipeline*                                                       pipeline;
    Graphics::RZSwapchain*                                                      swapchain;

    Graphics::Camera3D                                                          m_Camera;

    Graphics::RZTexture2D* testPassTexture;
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}