#include <Razix.h>

class Sandbox : public Razix::Application
{
public:
    Sandbox() : Application("/Sandbox/","Sandbox")
    {
       
    }

    ~Sandbox()
    {

    }
};

Razix::Application* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}