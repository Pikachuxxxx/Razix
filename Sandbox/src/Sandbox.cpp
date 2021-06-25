#include <Razix.h>

class TestLayer : public Razix::Layer
{
public:
    TestLayer()
        : Layer("Test Layer")
    {

    }

    void OnUpdate() override
    {
        RAZIX_TRACE("Mouse Position : {0}, {1}", Razix::Input::GetMouseX(), Razix::Input::GetMouseY());
    }

    void OnEvent(Razix::Event& event) override { }
};

class Sandbox : public Razix::Application
{
public:
    Sandbox() : Application("Sandbox")
    {
        PushLayer(new TestLayer());
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