// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Platform/api/OpenGL/OpenGLContext.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKContext.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX11
    #include "Razix/Platform/API/DirectX11/DX11Context.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12Context.h"
#endif

#include "Razix/Gfx/RHI/API/RZSampler.h"

#include "Razix/Gfx/Materials/RZMaterial.h"
#include "Razix/Gfx/RZShaderLibrary.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        GraphicsFeatures g_GraphicsFeatures = {};

        // Initializing the static variables
        RZGraphicsContext* RZGraphicsContext::s_Context = nullptr;
        // The Engine uses Vulkan aS the default render API
        RenderAPI RZGraphicsContext::s_RenderAPI = RenderAPI::VULKAN;

        /**
         * Razix Gfx StartUp and Shutdown flow 
         * Graphics Context 
         * 1. Gfx::RZResourceManager::Get().StartUp();
         * 2.   RZSwapchain textures
         * 3.   Context -> Create + Init --> PostGraphicsContextInit() --> SamplerPresets + Default Material
         * 4. RHI::Create
         * 5. Render Frame
         * 6. RHI::Release
         * 7.   Destroy Swapchain back buffers
         * 8.   GraphicsContext::Release
         * 9.       PreGraphicsContextDestroy
         * 10.          SamplerPresets + Default Material Destroy
         * 11.          ShaderLibrary Shutdown
         * 12.          Gfx::RZResourceManager::Get().ShutDown();
         * 13.  s_Context->Destroy() -> Destroy swapchain/device and instance/context 
         */

        // [Inconsistency!] Swapchain release happens in RHI but creation is done in GraphicsContext::Init()
        // [Inconsistency!] ShaderLibrary startup happens in RZEngine::PostGraphicsIgnition but clean up happens in GraphicsContext::Release

        void RZGraphicsContext::Create(const WindowProperties& properties, RZWindow* window)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (s_Context) {
                RAZIX_CORE_WARN("[Graphics Context] Attempting to create context again! Ignoring call.");
                return;
            }

            // Initialize the Resource Manager here!
            Gfx::RZResourceManager::Get().StartUp();

            switch (s_RenderAPI) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Gfx::RenderAPI::OPENGL: s_Context = new OpenGLContext((GLFWwindow*) window->GetNativeWindow()); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: s_Context = new VKContext(window); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12: s_Context = new DX12Context(window); break;
#endif
                default: s_Context = nullptr; break;
            }
        }

        void RZGraphicsContext::Release()
        {
            PreGraphicsContextDestroy();

            if (s_Context) {
                s_Context->Destroy();
                delete s_Context;    // This is causing unnecessary crashes
                s_Context = nullptr;
            }
        }

        void RZGraphicsContext::PostGraphicsContextInit()
        {
            RAZIX_CORE_TRACE("[RHI] Creating Sampler Presets");
            Gfx::RZSampler::CreateSamplerPresets();

            RAZIX_CORE_TRACE("[RHI] Creating Default Texture");
            Gfx::RZMaterial::InitDefaultTexture();
        }

        void RZGraphicsContext::PreGraphicsContextDestroy()
        {
            Gfx::RZSampler::DestroySamplerPresets();
            Gfx::RZMaterial::ReleaseDefaultTexture();

            // Render subsystems cleanup
            Gfx::RZShaderLibrary::Get().ShutDown();
            Gfx::RZResourceManager::Get().ShutDown();
        }

        RZGraphicsContext* RZGraphicsContext::GetContext()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (!s_Context) {
                RAZIX_CORE_WARN("[Graphics Context] Trying to get context before it has been created. Returning nullptr!");
                return nullptr;
            }

            switch (s_RenderAPI) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Gfx::RenderAPI::OPENGL: return static_cast<OpenGLContext*>(s_Context); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: return static_cast<VKContext*>(s_Context); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12: return static_cast<DX12Context*>(s_Context); break;
#endif
                default: return s_Context; break;
            }
            RAZIX_CORE_ASSERT(s_Context, "[Graphics Context] Failed to get context!")
            return nullptr;
        }

        const std::string Gfx::RZGraphicsContext::GetRenderAPIString()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (s_RenderAPI) {
                case Razix::Gfx::RenderAPI::VULKAN: return "Vulkan"; break;
                case Razix::Gfx::RenderAPI::D3D12: return "DirectX 12"; break;
                case Razix::Gfx::RenderAPI::GXM: return "SCE GXM (PSVita)"; break;
                case Razix::Gfx::RenderAPI::GCM: return "SCE GCM (PS3)"; break;
                default: return "None"; break;
            }
        }
    }    // namespace Gfx
}    // namespace Razix
