// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RHI.h"

#include "Razix/Core/RZApplication.h"

#include "Razix/Graphics/Materials/RZMaterial.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKRenderContext.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/GLRenderContext.h"
#endif

namespace Razix {
    namespace Graphics {

        RHI* RHI::s_APIInstance = nullptr;

        void RHI::Create(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_INFO("Creating API Renderer...");
            RAZIX_CORE_INFO("\t Backend API : {0}", Graphics::RZGraphicsContext::GetRenderAPIString());

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: s_APIInstance = new GLRenderContext(width, height); break;
                case Razix::Graphics::RenderAPI::VULKAN: s_APIInstance = new VKRenderContext(width, height); break;
                case Razix::Graphics::RenderAPI::D3D12:    // to be implemented soon
                default: s_APIInstance = nullptr; break;    
            } 
        }

        void RHI::Release()
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