#pragma once

// Core
#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"
#include "Razix/Core/Engine.h"
#include "Razix/Core/IRazixSystem.h"
#include "Razix/Core/Application.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/ReferenceCounter.h"
#include "Razix/Core/SmartPointers.h"
#include "Razix/Core/SplashScreen.h"

// Core/OS
#include "Razix/Core/OS/OS.h"
#include "Razix/Core/OS/RazixInput.h"
#include "Razix/Core/OS/FileSystem.h"
#include "Razix/Core/OS/Window.h"
#include "Razix/Core/OS/VFS.h"

// Events
#include "Razix/Events/Event.h"
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"

// Graphics/API
#include "Razix/Graphics/API/GraphicsContext.h"

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
