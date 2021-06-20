#pragma once

// Using the forward declared the application creating function, that we assume was defined on the client side
extern Razix::Application* Razix::CreateApplication();

#ifdef RAZIX_PLATFORM_WINDOWS

// Engine Entry point - main method
int main(int argc, char** argv)
{
    // Engine Systems Initializations
    // -> Logging System
    Razix::Debug::Log::InitLogger();

    // Application auto Initialization by the Engine
    auto app = Razix::CreateApplication();
    app->Run();
    delete app;

    // Shutdown the Engine systems
    Razix::Debug::Log::ShutdownLogger();
}
#endif