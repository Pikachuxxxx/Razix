#include "rzxpch.h"
#include "Window.h"

namespace Razix
{
	Window* (*Window::ConstructionFunc) (const WindowProperties&) = nullptr;

	Window* Window::Create(const WindowProperties& properties /*= WindowProperties()*/)
	{
		RAZIX_ASSERT(ConstructionFunc, "Windows construction funcition not found!");
		return ConstructionFunc(properties);
	}
}