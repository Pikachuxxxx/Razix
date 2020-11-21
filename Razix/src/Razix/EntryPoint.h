#pragma once

#include "Application.h"

#ifdef RZX_PLATFORM_WINDOWS

extern Razix::Application* Razix::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Razix::CreateApplication();
	app->Run();
	delete app;
}

#endif