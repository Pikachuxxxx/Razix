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

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {

    namespace Gfx {

        // Initializing the static variables
        RZGraphicsContext* RZGraphicsContext::s_Context = nullptr;
        // The Engine uses Vulkan aS the default render API
        RenderAPI RZGraphicsContext::s_RenderAPI = RenderAPI::VULKAN;

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
            // Shutdown the Resource System
            Gfx::RZResourceManager::Get().ShutDown();

            if (s_Context) {
                s_Context->Destroy();
                delete s_Context;    // This is causing unnecessary crashes
                s_Context = nullptr;
            }
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
