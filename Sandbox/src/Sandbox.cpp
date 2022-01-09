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

            Graphics::RZTexture2D* logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/RazixLogo.png", "TextureAttachmentGLTest", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE, filtering);

            float vertices[8 * 3] = {
               -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f,
            };

            // This buffer layout will be somehow combined with the vertex buffers and passed to the pipeline for the Input Assembly stage
            bufferLayout.push<glm::vec3>("Position");
            bufferLayout.push<glm::vec3>("Color");
            bufferLayout.push<glm::vec2>("TexCoord");

            triVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 24, vertices, Graphics::BufferUsage::STATIC);
            triVBO->AddBufferLayout(bufferLayout);

            viewProjUniformBuffer = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            // Create the shader
            defaultShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/default.rzsf");

            // get the descriptor infos to create the descriptor sets
            auto setInfos = defaultShader->getSetInfos();

            for (auto setInfo : setInfos) {
                auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                descripotrSets.push_back(descSet);
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
            pipelineInfo.cullMode = Graphics::CullMode::BACK;
            pipelineInfo.depthBiasEnabled = false;
            pipelineInfo.drawType = Graphics::DrawType::TRIANGLE;
            pipelineInfo.renderpass = renderpass;
            pipelineInfo.shader = defaultShader;
            pipelineInfo.transparencyEnabled = false;

            pipeline = Graphics::RZPipeline::Create(pipelineInfo);

            // Create the framebuffer
            //framebuffers.resize(Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount());

            Graphics::RZTexture::Type attachmentTypes[2];
            attachmentTypes[0] = Graphics::RZTexture::Type::COLOR;
            attachmentTypes[1] = Graphics::RZTexture::Type::DEPTH;

            auto swaoImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
            auto depthImage = Graphics::RZDepthTexture::Create(getWindow()->getWidth(), getWindow()->getHeight());

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
        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
            swapchain->Flip();
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.99f, 0.33f, 0.43f);

            // Set the view port

            // Update the uniform buffer data
            viewProjUBOData.view = glm::mat4(1.0f);
            viewProjUBOData.projection = glm::mat4(1.0f);
            viewProjUniformBuffer->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            // Bind the Vertex and Index buffers
            Graphics::RZAPIRenderer::Begin();

            renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

            pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

            triVBO->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            
            Graphics::RZAPIRenderer::Draw(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), 3, Graphics::DataType::FLOAT);
            Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

private:
    Graphics::RZVertexBufferLayout              bufferLayout;
    Graphics::RZVertexBuffer*                   triVBO;
    Graphics::RZUniformBuffer*                  viewProjUniformBuffer;
    Graphics::RZShader*                         defaultShader;
    std::vector<Graphics::RZDescriptorSet*>     descripotrSets;
    Graphics::RZRenderPass*                     renderpass;
    std::vector<Graphics::RZFramebuffer*>       framebuffers;
    Graphics::RZPipeline*                       pipeline;
    Graphics::RZSwapchain* swapchain;
    // TODO: 1. Create Descriptor sets + UBO binding
    // TODO: 2. Shader binding to sets and UBO
    // TODO: 3. Create Render pass
    // TODO: 4. Create Framebuffers for each frame and you know what else to do
    // TODO: 5. Create Pipeline
    // TODO: 6. Assemble everything together
    // TODO: 7. Submit commands and render stuff + sync check for all the frames in-flight and swapchain frames
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}