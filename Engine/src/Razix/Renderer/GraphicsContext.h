#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

namespace Razix
{
    /* The Render API to use for rendering the application */
    enum class RenderAPI 
    {
        OPENGL = 0,
        VULKAN,
        DIRECTX11,
        DIRECTX12,  // Not Supported yet!
        GXM,        // Not Supported yet!
        GCM         // Not Supported yet!
    };

    /**
     * The Graphics Context that manages the context of the underlying graphics API
     */
    class RAZIX_API GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        inline std::string GetRenderAPIString() const {
            switch (m_RenderAPI) {
            case Razix::RenderAPI::OPENGL:
                return "OpenGL";
                break;
            case Razix::RenderAPI::VULKAN:
                return "Vulkan";
                break;
            case Razix::RenderAPI::DIRECTX11:
                return "DirectX 11";
                break;
            case Razix::RenderAPI::DIRECTX12:
                return "DirectX 12";
                break;
            case Razix::RenderAPI::GXM:
                return "SCE GXM (PSVita)";
                break;
            case Razix::RenderAPI::GCM:
                return "SCE GCM (PS3)";
                break;
            default:
                return "None";
                break;
            }
        }
        inline const RenderAPI& GetRenderAPI() const { return m_RenderAPI; }
        void SetRenderAPI(RenderAPI api) { m_RenderAPI = api; }

    private:
        RenderAPI m_RenderAPI = RenderAPI::OPENGL;
    };
}