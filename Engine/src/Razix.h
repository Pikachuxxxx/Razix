#pragma once

// Core
// clang-format off
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZCore.h"
// clang-format on

// Containers
#include "Razix/Core/Containers/arrays.h"
#include "Razix/Core/Containers/bits.h"
#include "Razix/Core/Containers/hash_functors.h"
#include "Razix/Core/Containers/hash_map.h"
#include "Razix/Core/Containers/initializer_list.h"
#include "Razix/Core/Containers/queue.h"
#include "Razix/Core/Containers/ref_counter.h"
#include "Razix/Core/Containers/ring_buffer.h"
#include "Razix/Core/Containers/smart_pointers.h"
#include "Razix/Core/Containers/stack.h"
#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Containers/string_utils.h"

// Std
#include "Razix/Core/std/atomics.h"
#include "Razix/Core/std/spinlock.h"
#include "Razix/Core/std/sprintf.h"
#include "Razix/Core/std/type_traits.h"
#include "Razix/Core/std/utility.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Core/System/IRZSystem.h"
#include "Razix/Core/UUID/RZUUID.h"
#include "Razix/Core/Version/RazixVersion.h"

// Core/OS
#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZKeyCodes.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/OS/RZWindow.h"

// Events
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

// Graphics API
#include "Razix/Gfx/RHI/RHI.h"

// Graphics/Cameras
#include "Razix/Gfx/Cameras/Camera3D.h"

// Graphics (FrameGraph and Resources)
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"
#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"
//#include "Razix/Gfx/RZMesh.h"
//#include "Razix/Gfx/RZMeshFactory.h"

// Scene
#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

// Utilities
#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Utils/RZColorUtilities.h"
#include "Razix/Core/Utils/RZCommandLineParser.h"
#include "Razix/Core/Utils/RZLoadImage.h"
#include "Razix/Core/Utils/RZTime.h"
#include "Razix/Core/Utils/RZTimestep.h"
#include "Razix/Core/Utils/TRZSingleton.h"

// Internal
// Razix Memory
#include "Razix/Core/Memory/RZMemory.h"

#include <imgui/imgui.h>

//-----------Entry Point-------------
#include "Razix/Core/EntryPoint.h"
//-----------------------------------
