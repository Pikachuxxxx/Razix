// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAPIRenderer.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKRenderer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZAPIRenderer* RZAPIRenderer::s_APIInstance = nullptr;

        void RZAPIRenderer::Create(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_INFO("Creating API Renderer...");
            RAZIX_CORE_INFO("\t Backend API : {0}", Graphics::RZGraphicsContext::GetRenderAPIString());
            //-------------------------------------------------------------------------------------
            // Creating the Graphics Context and Initialize it
            RAZIX_CORE_INFO("Creating Graphics Context...");
            Graphics::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
            RAZIX_CORE_INFO("Initializing Graphics Context...");
            Graphics::RZGraphicsContext::GetContext()->Init();
            //-------------------------------------------------------------------------------------

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: break;
                case Razix::Graphics::RenderAPI::VULKAN: s_APIInstance = new VKRenderer(width, height); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_APIInstance = nullptr; break;
            }
        }

        void RZAPIRenderer::Release()
        {
            s_APIInstance->DestroyAPIImpl();
            delete s_APIInstance;

            // Release the context at last
            Graphics::RZGraphicsContext::GetContext()->Release();
        }

    }    // namespace Graphics
}    // namespace Razix