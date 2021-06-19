#pragma once

#ifdef RAZIX_PLATFORM_WINDOWS

// Using the forward declared the application creating function, that we assume was defined on the client side
extern Razix::Application* Razix::CreateApplication();

// Engine Entry point - main method
int main(int argc, char** argv)
{
    // Engine Systems Initializations
    // -> Logging System
    Razix::Log::InitLogger();

    RAZIX_CORE_INFO("Initialized Core Engine Logger");
    RAZIX_INFO("Initialized Engine Application Logger");

    // Application auto Initialization by the Engine
    RAZIX_INFO("Creating Razix Application");
    auto app = Razix::CreateApplication();
    app->Run();
    delete app;
}

#endif