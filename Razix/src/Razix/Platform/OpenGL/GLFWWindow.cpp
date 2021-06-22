#include "rzxpch.h"
#include "GLFWWindow.h"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"

#include <GLFW/glfw3.h>
//TODO: Remove this, the context handle using Refs and Init
#ifdef RAZIX_RENDER_API_OPENGL
#include "Platform/OpenGL/OpenGLContext.h"
#endif // RAZXI_RENDER_API_OPENGL

namespace Razix
{
	static bool sGLFWInitialized = false;

	GLFWWindow::GLFWWindow(const WindowProperties& properties)
	{
		Init(properties);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}

	void GLFWWindow::OnWindowUpdate()
	{
#ifdef RAZIX_RENDER_API_OPENGL
		glfwPollEvents();
		m_Context->SwapBuffers();
#endif // RAZXI_RENDER_API_OPENGL
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.Vsync = enabled;
	}

	bool GLFWWindow::IsVSync() const
	{
		return m_Data.Vsync;
	}

	void GLFWWindow::Construct()
	{
		ConstructionFunc = GLFWConstructionFunc;
	}

	Window* GLFWWindow::GLFWConstructionFunc(const WindowProperties& properties)
	{
		return new GLFWWindow(properties);
	}

	void GLFWWindow::Init(const WindowProperties& properties)
	{
		m_Data.Title = properties.Title;
		m_Data.Width = properties.Width;
		m_Data.Height = properties.Height;
		//m_Data.API = properties.API; // use this with a switch statement to choose a proper rendering API

		RAZIX_CORE_INFO("Creating Window... \n \t\t\t Title : {0} (Width : {1}, Height : {2})", properties.Title, properties.Width, properties.Height);

		glfwSetErrorCallback([](int errorCode, const char* description)
		{
			RAZIX_CORE_ERROR("GLFW Error! code : {0} description : {1}", errorCode, description);
		});

		// TODO: Replace all this with WIN32 API
		if (!sGLFWInitialized)
		{
			int success = glfwInit();
			RAZIX_CORE_ASSERT(success, "Could not initialize GLFW");

			sGLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
		#endif

		m_Window = glfwCreateWindow((int)properties.Width, (int)properties.Height, properties.Title.c_str(), nullptr, nullptr);

#ifdef RAZIX_RENDER_API_OPENGL
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
#endif

		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Setting up event callbacks function via the dispatcher
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);

		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void GLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
}