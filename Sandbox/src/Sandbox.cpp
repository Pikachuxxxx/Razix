#include <Razix.h>

class TestLayer : public Razix::Layer
{
public:
    TestLayer()
        : Layer("Test Layer")
    {

    }

    void OnUpdate() override { }

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