#include <Razix.h>

class Sandbox : public Razix::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Razix::Application* Razix::CreateApplication()
{
	return new Sandbox();
}