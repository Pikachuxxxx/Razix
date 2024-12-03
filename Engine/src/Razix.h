#pragma once

// Core
// clang-format off
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZCore.h"
// clang-format on
#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/SplashScreen/RZSplashScreen.h"
#include "Razix/Core/System/IRZSystem.h"
#include "Razix/Core/UUID/RZUUID.h"
#include "Razix/Core/Version/RazixVersion.h"

// Core/OS
#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZKeyCodes.h"
#include "Razix/Core/OS/RZOS.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/OS/RZWindow.h"

// Events
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

// Graphics API
#include "Razix/Gfx/RHI/API/RZAPIDesc.h"
#include "Razix/Gfx/RHI/API/RZAPIHandles.h"
#include "Razix/Gfx/RHI/API/RZBufferLayout.h"
#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"
#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZFramebuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZRenderPass.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZSwapchain.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

// Graphics/Cameras
#include "Razix/Gfx/Cameras/Camera3D.h"

// Graphics
#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"

#include "Razix/Gfx/RZSprite.h"

// Scene
#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

// Utilities
#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/RZLoadImage.h"
#include "Razix/Utilities/RZStringUtilities.h"
#include "Razix/Utilities/RZTimer.h"
#include "Razix/Utilities/RZTimestep.h"
#include "Razix/Utilities/TRZSingleton.h"

// Internal
// Razix Memory
#include "RZMemory.h"
// Razix STL
#include "RZSTL/rzstl.h"

#include <imgui/imgui.h>

//-----------Entry Point-------------
#include "Razix/Core/EntryPoint.h"
//-----------------------------------
