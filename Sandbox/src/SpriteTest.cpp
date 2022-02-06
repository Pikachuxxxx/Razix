#if 1

#include <Razix.h>

using namespace Razix;

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

class SpriteTest : public Razix::RZApplication
{
private:

public:
    SpriteTest() : RZApplication("/Sandbox/", "SpriteTest"), m_ActiveScene("SpriteTest")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------
    }

    ~SpriteTest() {}

    void OnStart() override
    {
        m_ActiveScene.SerialiseScene("//Scenes/SpriteTest.rzscn");
        m_ActiveScene.DeSerialiseScene("//Scenes/SpriteTest.rzscn");

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

            // Update the sprite's color
            razixLogoSprite->setColour(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
            razixLogoSprite->setRotation(sin(getTimer().GetElapsed()));

            Graphics::RZAPIRenderer::Begin();
            {
                Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(getWindow()->getWidth(), getWindow()->getHeight());

                renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(1.0f, 0.5f, 1.0f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

               // Graphics::RZAPIRenderer::BindDescriptorSets(pipeline, Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), descripotrSets[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex());

                razixLogoSprite->getVertexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
                razixLogoSprite->getIndexBuffer()->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

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
        m_ActiveScene.SerialiseScene("//Scenes/Sandbox.rzscn");

        razixLogoSprite->destroy();

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
    Graphics::RZSprite*                                                         razixLogoSprite;

    Graphics::RZDepthTexture*                                                   depthImage;
    Graphics::RZRenderPass*                                                     renderpass;
    std::vector<Graphics::RZFramebuffer*>                                       framebuffers; // 3 FRAMEBU8FEFRS
    Graphics::RZPipeline*                                                       pipeline;
    
    // Only needed for OpenGL, Context holds the swapchain for Vulkan and DX12
    Graphics::RZSwapchain* swapchain;

    uint32_t                                                                    width, height;

    Graphics::Camera3D                                                          m_Camera;
    RZScene                                                                     m_ActiveScene;

    Graphics::RZTexture2D*                                                      testTexture;

private:
    void buildPipelineResources()
    {
        //testTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/TestGrid_256.png", "TextureAttachment1", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
        razixLogoSprite = new Graphics::RZSprite(glm::vec2(0.2, 0.2), 20.0f, glm::vec2(200.0f, 200.0f), glm::vec4(0.254, 0.45, 0.78, 1.0f));
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
        renderPassInfo.name = "screen clear pass";
        renderPassInfo.clear = true;

        renderpass = Graphics::RZRenderPass::Create(renderPassInfo);

        // Create the graphics pipeline
        Graphics::PipelineInfo pipelineInfo{};
        pipelineInfo.cullMode = Graphics::CullMode::NONE;
        pipelineInfo.drawType = Graphics::DrawType::TRIANGLE;
        pipelineInfo.renderpass = renderpass;
        pipelineInfo.shader = razixLogoSprite->getSimpleShader();
        pipelineInfo.transparencyEnabled = true;
        pipelineInfo.depthBiasEnabled = false;

        pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer

        auto swapImgCount = Graphics::RZAPIRenderer::getSwapchain()->GetSwapchainImageCount();
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
            frameBufInfo.attachments = attachments;

            framebuffers.push_back(Graphics::RZFramebuffer::Create(frameBufInfo));
        }
    }

    void destroyCommandPipeline()
    {
        depthImage->Release(true);

        for (auto OnframeBuf : framebuffers)
            OnframeBuf->Destroy();

        pipeline->Destroy();

        renderpass->Destroy();
    }
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new SpriteTest();
}
#endif