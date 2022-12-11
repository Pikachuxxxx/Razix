// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAPIRenderer.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/Materials/RZMaterial.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKAPIRenderer.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLAPIRenderer.h"
#endif

namespace Razix {
    namespace Graphics {

        RZAPIRenderer* RZAPIRenderer::s_APIInstance = nullptr;

        void RZAPIRenderer::Create(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_INFO("Creating API Renderer...");
            RAZIX_CORE_INFO("\t Backend API : {0}", Graphics::RZGraphicsContext::GetRenderAPIString());

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: s_APIInstance = new OpenGLAPIRenderer(width, height); break;
                case Razix::Graphics::RenderAPI::VULKAN: s_APIInstance = new VKAPIRenderer(width, height); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: s_APIInstance = nullptr; break;
            }
        }

        void RZAPIRenderer::Release()
        {
            Graphics::RZMaterial::ReleaseDefaultTexture();
            // Shutting down the shader library
            Graphics::RZShaderLibrary::Get().ShutDown();

            s_APIInstance->DestroyAPIImpl();
            delete s_APIInstance;

            // Release the context at last
            Graphics::RZGraphicsContext::GetContext()->Release();
        }

    }    // namespace Graphics
}    // namespace Razix