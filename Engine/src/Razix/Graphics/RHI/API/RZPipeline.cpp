// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPipeline.h"

#include "Razix/Graphics/RHI/API/RZAPIDesc.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKPipeline.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLPipeline.h"
#endif

namespace Razix {
    namespace Graphics {

        // TEMP FIX
        class DX12Pipeline
        {};

        GET_INSTANCE_SIZE_IMPL(Pipeline)

        void RZPipeline::Create(void* where, const RZPipelineDesc& pipelineInfo RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLPipeline(pipelineInfo); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKPipeline(pipelineInfo RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX11
                case Razix::Graphics::RenderAPI::D3D11:
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12:
#endif
                default: break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix