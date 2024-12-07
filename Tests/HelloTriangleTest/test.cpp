#if 0
    #pragma once
    #include <Razix.h>

// This is a test without a RZApplication

void main(int argc, char** argv)
{
    // Custom Engine start up of necessary modules
    Razix::Debug::RZLog::StartUp();

    // OS specific start up
    auto windowsOS = new Razix::WindowsOS();
    Razix::RZOS::SetInstance(windowsOS);
    windowsOS->Init();

    // Set the API
    Razix::Gfx::RZGraphicsContext::SetRenderAPI(Razix::Gfx::RenderAPI::VULKAN);

    // Create a Window
    Razix::WindowProperties props;
    Razix::RZWindow*        window;
    props.Title = "Razix Engine Test - Hello Triangle | version : " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + Razix::Gfx::RZGraphicsContext::GetRenderAPIString() + ">";

    window = Razix::RZWindow::Create(props);

    Razix::RZInput::SelectGLFWInputManager();

    // Init Graphics Context
    //-------------------------------------------------------------------------------------
    // Creating the Graphics Context and Initialize it
    RAZIX_CORE_INFO("Creating Graphics Context...");
    Razix::Gfx::RZGraphicsContext::Create(props, window);
    RAZIX_CORE_INFO("Initializing Graphics Context...");
    Razix::Gfx::RZGraphicsContext::GetContext()->Init();
    //-------------------------------------------------------------------------------------
    #if 0

    std::vector<Razix::Gfx::RZCommandBuffer*> CommandBuffers(3);
    for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
        CommandBuffers[i] = Razix::Gfx::RZCommandBuffer::Create();
        CommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Main Command Buffers"));
    }

    // Create the API renderer to issue render commands
    Razix::Gfx::RHI::Create(window->getWidth(), window->getHeight());
    Razix::Gfx::RHI::Init();

    Razix::Gfx::RHI::AcquireImage({});

    Razix::Gfx::RHI::Begin(CommandBuffers[Razix::Gfx::RHI::getSwapchain()->getCurrentImageIndex()]);

    Razix::Gfx::RenderingInfo rendering_info{};
    rendering_info.colorAttachments = {{Razix::Gfx::RHI::getSwapchain()->GetCurrentImage(), {true, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)}}};
    rendering_info.resize           = true;
    Razix::Gfx::RHI::BeginRendering(Razix::Gfx::RHI::getCurrentCommandBuffer(), rendering_info);

    Razix::Gfx::RHI::EndRendering(Razix::Gfx::RHI::getCurrentCommandBuffer());

    Razix::Gfx::RHI::Submit(Razix::Gfx::RHI::getCurrentCommandBuffer());

    Razix::Gfx::RHI::SubmitWork({}, {});

    Razix::Gfx::RHI::Present({});

    Razix::Gfx::RZGraphicsContext::Release();
    #endif

    bool closeWindow = false;
    while (!closeWindow) {
        //if (Razix::RZInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
        //    closeWindow = true;

        window->ProcessInput();
    }
}
#endif

void main()
{
}