#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::Application
{
    unsigned int m_VAO = 0, m_VBO, m_IBO;

public:
    Sandbox() : Application("/Sandbox/","Sandbox")
    {
        //RAZIX_TRACE("Default Render API : {0}", Razix::Graphics::GraphicsContext::GetRenderAPIString());
    }

    ~Sandbox() {

    }

};

Razix::Application* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}