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
        RZX_TRACE("Mouse Position : {0}, {1}", Razix::Input::GetMouseX(), Razix::Input::GetMouseY());
    }

    void OnEvent(Razix::Event& event) override { }
};

class Sandbox : public Razix::Application
{
public:
    Sandbox()
    {
        PushLayer(new TestLayer());
    }

    ~Sandbox()
    {

    }
};

Razix::Application* Razix::CreateApplication()
{
    return new Sandbox();
}