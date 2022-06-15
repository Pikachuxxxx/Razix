#include <Razix.h>

using namespace Razix;

class MemTestClass : public Razix::RZMemoryRoot
{
public:
    uint32_t member_1 = 0;
    char     member_3 = 'A';

    MemTestClass() {}
    ~MemTestClass() {}

    uint32_t getMember_1() { return member_1; }
};

class MemoryTest : public Razix::RZApplication
{
public:
    MemoryTest()
        : RZApplication("/Sandbox/", "MemoryTest") 
    {
    }

    void OnStart() override
    {
        RAZIX_TRACE("sizeof uint32_t : {0}", sizeof(uint32_t));
        RAZIX_TRACE("sizeof std::string : {0}", sizeof(std::string));
        RAZIX_TRACE("sizeof char : {0}", sizeof(char));
        RAZIX_TRACE("sizeof MemTestClass : {0}", sizeof(MemTestClass));
        memtestClass = new MemTestClass;

        //-----------------------------------------------

        width  = getWindow()->getWidth();
        height = getWindow()->getHeight();

        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            swapchain = Graphics::RZSwapchain::Create(getWindow()->getWidth(), getWindow()->getHeight());
        } else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {

            defaultShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/default.rzsf");

            buildCommandPipeline();

            Graphics::RZAPIRenderer::Init();

            getImGuiRenderer()->init();
            getImGuiRenderer()->createPipeline(*renderpass);
        }
    }

    void OnUpdate(const RZTimestep& dt) override
    {
        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
            swapchain->Flip();
        } else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.99f, 0.33f, 0.43f);

            Graphics::RZAPIRenderer::Begin();
            {
                Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer()->UpdateViewport(getWindow()->getWidth(), getWindow()->getHeight());

                renderpass->BeginRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer(), glm::vec4(0.19f, 0.19f, 0.19f, 1.0f), framebuffers[Graphics::RZAPIRenderer::getSwapchain()->getCurrentImageIndex()], Graphics::SubPassContents::INLINE, getWindow()->getWidth(), getWindow()->getHeight());

                pipeline->Bind(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                if (getImGuiRenderer()->update(dt))
                    getImGuiRenderer()->draw(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                renderpass->EndRenderPass(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());

                // Present the frame by executing the recorded commands
                Graphics::RZAPIRenderer::Present(Graphics::RZAPIRenderer::getSwapchain()->getCurrentCommandBuffer());
            }
        } else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
        }
    }

    void OnQuit() override
    {
        // Save the current scene
        Razix::RZEngine::Get().getSceneManager().saveAllScenes();

        getImGuiRenderer()->destroy();

        defaultShader->Destroy();

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

private:
    MemTestClass* memtestClass;

    //-----------------------------------------------------------------


    Graphics::RZDepthTexture*             depthImage;
    std::vector<Graphics::RZFramebuffer*> framebuffers;
    Graphics::RZSwapchain*                swapchain;
    Graphics::RZRenderPass*               renderpass;
    Graphics::RZPipeline*                 pipeline;
    uint32_t                              width, height;
    Graphics::RZShader*                   defaultShader;

private:

    void buildCommandPipeline()
    {
        // Depth Map
        depthImage = Graphics::RZDepthTexture::Create(width, height);

        // Create the render pass
        Graphics::AttachmentInfo textureTypes[2] = {{Graphics::RZTexture::Type::COLOR, Graphics::RZTexture::Format::SCREEN},
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
        pipelineInfo.shader              = defaultShader;
        pipelineInfo.transparencyEnabled = true;
        pipelineInfo.depthBiasEnabled    = false;

        pipeline = Graphics::RZPipeline::Create(pipelineInfo);

        // Create the framebuffer
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
        depthImage->Release(true);

        for (auto frameBuf: framebuffers) frameBuf->Destroy();

        renderpass->Destroy();

        pipeline->Destroy();
    }
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application [MemoryTest]");
    return new MemoryTest();
}