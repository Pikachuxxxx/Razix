#include "rzxpch.h"
#include "GraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Platform/api/OpenGL/OpenGLContext.h"
#endif 

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKContext.h"
#endif

namespace Razix {

    namespace Graphics {

        // Initializing the static variables
        GraphicsContext* GraphicsContext::s_Context = nullptr;
        RenderAPI GraphicsContext::s_RenderAPI = RenderAPI::OPENGL;

        void GraphicsContext::Create(const WindowProperties& properties, Window* window) {

            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:    s_Context = new OpenGLContext((GLFWwindow*) window->GetNativeWindow()); break;
                case Razix::Graphics::RenderAPI::VULKAN:    s_Context = new VKContext();                                            break;
                case Razix::Graphics::RenderAPI::DIRECTX11: break;
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_Context = nullptr; break;
            }
        }

        void GraphicsContext::Release() {
            delete s_Context;
        }

        GraphicsContext* GraphicsContext::Get() {
            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:    return (OpenGLContext*) s_Context; break;
                case Razix::Graphics::RenderAPI::VULKAN:    return (VKContext*)     s_Context; break;
                case Razix::Graphics::RenderAPI::DIRECTX11: break;
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default:                                    return s_Context; break;
            }
            return nullptr;
        }

        const std::string Graphics::GraphicsContext::GetRenderAPIString() {
            switch (s_RenderAPI) {
                case Razix::Graphics::RenderAPI::OPENGL:
                    return "OpenGL";
                    break;
                case Razix::Graphics::RenderAPI::VULKAN:
                    return "Vulkan";
                    break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                    return "DirectX 11";
                    break;
                case Razix::Graphics::RenderAPI::DIRECTX12:
                    return "DirectX 12";
                    break;
                case Razix::Graphics::RenderAPI::GXM:
                    return "SCE GXM (PSVita)";
                    break;
                case Razix::Graphics::RenderAPI::GCM:
                    return "SCE GCM (PS3)";
                    break;
                default:
                    return "None";
                    break;
            }
        }
    }
}


