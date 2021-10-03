#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::Application
{
public:
    Sandbox() : Application("/Sandbox/","Sandbox")
    {
        //RAZIX_TRACE("Default Render API : {0}", Razix::Graphics::GraphicsContext::GetRenderAPIString());
        Razix::Graphics::GraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::DIRECTX11);
    }

    ~Sandbox() {

    }

};

Razix::Application* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}