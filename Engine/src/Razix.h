#pragma once

// Core
// clang-format off
#include "Razix/Core/RZDataTypes.h"
#include "Razix/Core/RZCore.h"
// clang-format on
#include "Razix/Core/System/IRZSystem.h"
#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/SplashScreen/RZSplashScreen.h"
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
#include "Razix/Graphics/RHI/API/RZAPIDesc.h"
#include "Razix/Graphics/RHI/API/RZAPIHandles.h"
#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZFramebuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZRenderPass.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"
#include "Razix/Graphics/RHI/API/RZBufferLayout.h"

#include "Razix/Graphics/RHI/RHI.h"

// Graphics/Cameras
#include "Razix/Graphics/Cameras/Camera3D.h"

// Graphics
#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZSprite.h"

// Scene
#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

// Utilities
#include "Razix/Utilities/LoadImage.h"
#include "Razix/Utilities/RZCommandLineParser.h"
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
