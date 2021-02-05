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
        RZX_TRACE("Test Layer OnUpdate");
    }

    void OnEvent(Razix::Event& event) override
    {
        RZX_TRACE("{0}", event);
    }
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