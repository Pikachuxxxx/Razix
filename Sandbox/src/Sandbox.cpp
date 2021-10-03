#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::Application
{
public:
    Sandbox() : Application("/Sandbox/","Sandbox")
    {
        // This doesn't work as the RenderAPI is set by the De-serialized data or by the command line
    }

    ~Sandbox() {

    }

    void OnUpdate(const Timestep& dt) override {
        if (Razix::Graphics::GraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
            Razix::Graphics::GraphicsContext::Get()->ClearWithColor(0.97f, 0.58f, 0.25f);
        else if (Razix::Graphics::GraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::GraphicsContext::Get()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }
};

Razix::Application* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}