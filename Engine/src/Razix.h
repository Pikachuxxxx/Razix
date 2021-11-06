#pragma once

// Core
#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"
#include "Razix/Core/Engine.h"
#include "Razix/Core/IRazixSystem.h"
#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/ReferenceCounter.h"
#include "Razix/Core/SmartPointers.h"
#include "Razix/Core/RZSplashScreen.h"

// Core/OS
#include "Razix/Core/OS/RZOS.h"
#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZWindow.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

// Events
#include "Razix/Events/Event.h"
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"

// Graphics/API
#include "Razix/Graphics/API/GraphicsContext.h"
#include "Razix/Graphics/API/Swapchain.h"
#include "Razix/Graphics/API/Texture.h"

// Platform
#include "Razix/Platform/API/DirectX11/DX11Context.h"
#include "Razix/Platform/API/DirectX11/DX11Swapchain.h"
#include "Razix/Platform/API/DirectX11/DX11Texture.h"

#include "Razix/Platform/API/OpenGL/OpenGLContext.h"
#include "Razix/Platform/API/OpenGL/OpenGLSwapchain.h"
#include "Razix/Platform/API/OpenGL/OpenGLTexture.h"
#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

// Utilities
#include "Razix/Utilities/CommandLineParser.h"
#include "Razix/Utilities/LoadImage.h"
#include "Razix/Utilities/StringUtilities.h"
#include "Razix/Utilities/Timestep.h"
#include "Razix/Utilities/Timer.h"

// TODO: Add platform folder header files

//-----------Entry Point-------------
#include "Razix/Core/EntryPoint.h"
//-----------------------------------

// TODO: Remove this!
#include <glad/glad.h>
