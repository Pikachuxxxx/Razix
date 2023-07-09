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

#include "Razix/Graphics/Resources/RZResourceManager.h"

namespace Razix {

    namespace Graphics {

        // Initializing the static variables
        RZGraphicsContext* RZGraphicsContext::s_Context = nullptr;
        // The Engine uses Vulkan aS the default render API
        RenderAPI RZGraphicsContext::s_RenderAPI = RenderAPI::VULKAN;

        void RZGraphicsContext::Create(const WindowProperties& properties, RZWindow* window)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Initialize the Resource Manager here!
            Graphics::RZResourceManager::Get().StartUp();

            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL: s_Context = new OpenGLContext((GLFWwindow*) window->GetNativeWindow()); break;
                case Razix::Graphics::RenderAPI::VULKAN: s_Context = new VKContext(window); break;
                case Razix::Graphics::RenderAPI::D3D11: s_Context = new DX11Context(window); break;
                case Razix::Graphics::RenderAPI::D3D12: s_Context = new DX12Context(window); break;
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_Context = nullptr; break;
            }
        }

        void RZGraphicsContext::Release()
        {
            // Shutdown the Resource System
            Graphics::RZResourceManager::Get().ShutDown();

            s_Context->Destroy();
            //delete s_Context; // This is causing unnecessary crashes
        }

        RZGraphicsContext* RZGraphicsContext::GetContext()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL: return static_cast<OpenGLContext*>(s_Context); break;
                case Razix::Graphics::RenderAPI::VULKAN: return static_cast<VKContext*>(s_Context); break;
                case Razix::Graphics::RenderAPI::D3D11: return static_cast<DX11Context*>(s_Context); break;
                case Razix::Graphics::RenderAPI::D3D12: return static_cast<DX12Context*>(s_Context); break;
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return s_Context; break;
            }
            return nullptr;
        }

        const std::string Graphics::RZGraphicsContext::GetRenderAPIString()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL: return "OpenGL"; break;
                case Razix::Graphics::RenderAPI::VULKAN: return "Vulkan"; break;
                case Razix::Graphics::RenderAPI::D3D11: return "DirectX 11"; break;
                case Razix::Graphics::RenderAPI::D3D12: return "DirectX 12"; break;
                case Razix::Graphics::RenderAPI::GXM: return "SCE GXM (PSVita)"; break;
                case Razix::Graphics::RenderAPI::GCM: return "SCE GCM (PS3)"; break;
                default: return "None"; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix
