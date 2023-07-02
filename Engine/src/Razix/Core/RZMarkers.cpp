// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMarkers.h"

#include "Razix/Graphics/RHI/RHI.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"
    #include "Razix/Platform/API/Vulkan/VKCommandBuffer.h"
#endif

void BeginMarker(const std::string& name, glm::vec4 color)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf = Razix::Graphics::RHI::GetCurrentCommandBuffer();

    switch (Razix::Graphics::RZGraphicsContext::GetRenderAPI()) {
        case Razix::Graphics::RenderAPI::VULKAN:
            Razix::Graphics::VKUtilities::CmdBeginDebugUtilsLabelEXT(static_cast<Razix::Graphics::VKCommandBuffer*>(cmdBuf)->getBuffer(), name, color);
            break;
        case Razix::Graphics::RenderAPI::D3D12: break;
    }
}

void InsertMarker(const std::string& name, glm::vec4 color)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf = Razix::Graphics::RHI::GetCurrentCommandBuffer();
    if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
    }

    switch (Razix::Graphics::RZGraphicsContext::GetRenderAPI()) {
        case Razix::Graphics::RenderAPI::VULKAN:
            Razix::Graphics::VKUtilities::CmdInsertDebugUtilsLabelEXT(static_cast<Razix::Graphics::VKCommandBuffer*>(cmdBuf)->getBuffer(), name, color);
            break;
        case Razix::Graphics::RenderAPI::D3D12: break;
    }
}

void EndMarker()
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf = Razix::Graphics::RHI::GetCurrentCommandBuffer();
    if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
    }

    switch (Razix::Graphics::RZGraphicsContext::GetRenderAPI()) {
        case Razix::Graphics::RenderAPI::VULKAN:
            Razix::Graphics::VKUtilities::CmdEndDebugUtilsLabelEXT(static_cast<Razix::Graphics::VKCommandBuffer*>(cmdBuf)->getBuffer());
            break;
        case Razix::Graphics::RenderAPI::D3D12: break;
    }
}
