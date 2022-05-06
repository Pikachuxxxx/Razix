#pragma once

// Core
#include "Razix/Core/IRZSystem.h"
#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZLog.h"
#include "Razix/Core/RZSmartPointers.h"
#include "Razix/Core/RZSplashScreen.h"
#include "Razix/Core/RZUUID.h"
#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/ReferenceCounter.h"

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
#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZDescriptorSet.h"
#include "Razix/Graphics/API/RZFramebuffer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZRenderPass.h"
#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"
#include "Razix/Graphics/API/RZVertexBufferLayout.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"

// Graphics/Cameras
#include "Razix/Graphics/Cameras/Camera3D.h"

// Graphics
#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZSprite.h"

// Scene
#include "Razix/Scene/RZComponents.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

// Utilities
#include "Razix/Utilities/LoadImage.h"
#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/RZStringUtilities.h"
#include "Razix/Utilities/RZTimer.h"
#include "Razix/Utilities/RZTimestep.h"
#include "Razix/Utilities/TRZSingleton.h"

//-----------Entry Point-------------
#include "Razix/Core/EntryPoint.h"
//-----------------------------------
