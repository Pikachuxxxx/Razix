#pragma once
#include <Razix.h>

// This is a test without a RZApplication

    // Create a Window
Razix::WindowProperties props;
Razix::RZWindow*        window;
Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    props.Title = "Razix Engine Test - Hello Triangle | version : " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + Razix::Graphics::RZGraphicsContext::GetRenderAPIString() + ">";
    window = Razix::RZWindow::Create(props);


    // Init Graphics Context
    //-------------------------------------------------------------------------------------
    // Creating the Graphics Context and Initialize it
    RAZIX_CORE_INFO("Creating Graphics Context...");
    Razix::Graphics::RZGraphicsContext::Create(props, window);
    RAZIX_CORE_INFO("Initializing Graphics Context...");
    Razix::Graphics::RZGraphicsContext::GetContext()->Init();
    //-------------------------------------------------------------------------------------

    return nullptr;
}

void main(int argc, char** argv)
{
    EngineMain(argc, argv);

    // Set the API
    Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);

#if 0

    std::vector<Razix::Graphics::RZCommandBuffer*> CommandBuffers(3);
    for (size_t i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
        CommandBuffers[i] = Razix::Graphics::RZCommandBuffer::Create();
        CommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Main Command Buffers"));
    }

    // Create the API renderer to issue render commands
    Razix::Graphics::RHI::Create(window->getWidth(), window->getHeight());
    Razix::Graphics::RHI::Init();

    Razix::Graphics::RHI::AcquireImage({});

    Razix::Graphics::RHI::Begin(CommandBuffers[Razix::Graphics::RHI::getSwapchain()->getCurrentImageIndex()]);

    Razix::Graphics::RenderingInfo rendering_info{};
    rendering_info.colorAttachments = {{Razix::Graphics::RHI::getSwapchain()->GetCurrentImage(), {true, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)}}};
    rendering_info.resize           = true;
    Razix::Graphics::RHI::BeginRendering(Razix::Graphics::RHI::getCurrentCommandBuffer(), rendering_info);

    Razix::Graphics::RHI::EndRendering(Razix::Graphics::RHI::getCurrentCommandBuffer());

    Razix::Graphics::RHI::Submit(Razix::Graphics::RHI::getCurrentCommandBuffer());

    Razix::Graphics::RHI::SubmitWork({}, {});

    Razix::Graphics::RHI::Present({});

    Razix::Graphics::RZGraphicsContext::Release();
#endif

    bool closeWindow = false;
    while (!closeWindow) {
        if (Razix::RZInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
            closeWindow = true;

        window->ProcessInput();
    }

    EngineExit();
}