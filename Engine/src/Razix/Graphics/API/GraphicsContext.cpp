#include "rzxpch.h"
#include "GraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Platform/api/OpenGL/OpenGLContext.h"
#endif 

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKContext.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX11
#include "Razix/Platform/API/DirectX11/DX11Context.h"
#endif

namespace Razix {

    namespace Graphics {

        // Initializing the static variables
        RZGraphicsContext* RZGraphicsContext::s_Context = nullptr;
        RenderAPI RZGraphicsContext::s_RenderAPI = RenderAPI::OPENGL;

        void RZGraphicsContext::Create(const WindowProperties& properties, RZWindow* window) {

            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:    s_Context = new OpenGLContext((GLFWwindow*) window->GetNativeWindow()); break;
                case Razix::Graphics::RenderAPI::VULKAN:    s_Context = new VKContext(window);                                      break;
                case Razix::Graphics::RenderAPI::DIRECTX11: s_Context = new DX11Context(window);                                    break;
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_Context = nullptr; break;
            }
        }

        void RZGraphicsContext::Release() {
            s_Context->Destroy();
            delete s_Context;
        }

        RZGraphicsContext* RZGraphicsContext::GetContext() {
            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:    return static_cast<OpenGLContext*> (s_Context); break;
                case Razix::Graphics::RenderAPI::VULKAN:    return static_cast<VKContext*>     (s_Context); break;
                case Razix::Graphics::RenderAPI::DIRECTX11: return static_cast<DX11Context*>   (s_Context); break;
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default:                                    return s_Context; break;
            }
            return nullptr;
        }

        const std::string Graphics::RZGraphicsContext::GetRenderAPIString() {
            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:    return "OpenGL";            break;
                case Razix::Graphics::RenderAPI::VULKAN:    return "Vulkan";            break;
                case Razix::Graphics::RenderAPI::DIRECTX11: return "DirectX 11";        break;
                case Razix::Graphics::RenderAPI::DIRECTX12: return "DirectX 12";        break;
                case Razix::Graphics::RenderAPI::GXM:       return "SCE GXM (PSVita)";  break;
                case Razix::Graphics::RenderAPI::GCM:       return "SCE GCM (PS3)";     break;
                default:                                    return "None";              break;
            }
        }
    }
}


