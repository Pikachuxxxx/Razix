// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/RHI.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKDrawCommandBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"
#endif

void BeginMarker(const std::string& name, glm::vec4 color)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf            = Razix::Graphics::RHI::GetCurrentCommandBuffer();
    auto cmdBufferResource = Razix::Graphics::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuf);

    switch (Razix::Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
        case Razix::Graphics::RenderAPI::VULKAN:
            Razix::Graphics::VKUtilities::CmdBeginDebugUtilsLabelEXT(static_cast<Razix::Graphics::VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer(), name, color);
            break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        case Razix::Graphics::RenderAPI::D3D12:
            Razix::Graphics::DX12Utilities::CmdBeginLabel(static_cast<Razix::Graphics::DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList(), name, color);
            break;
#endif
    }
}

void InsertMarker(const std::string& name, glm::vec4 color)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf            = Razix::Graphics::RHI::GetCurrentCommandBuffer();
    auto cmdBufferResource = Razix::Graphics::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuf);

    switch (Razix::Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
        case Razix::Graphics::RenderAPI::VULKAN:
            Razix::Graphics::VKUtilities::CmdInsertDebugUtilsLabelEXT(static_cast<Razix::Graphics::VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer(), name, color);
            break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        case Razix::Graphics::RenderAPI::D3D12: break;
#endif
    }
}

void EndMarker()
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf            = Razix::Graphics::RHI::GetCurrentCommandBuffer();
    auto cmdBufferResource = Razix::Graphics::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuf);

    switch (Razix::Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
        case Razix::Graphics::RenderAPI::VULKAN:
            Razix::Graphics::VKUtilities::CmdEndDebugUtilsLabelEXT(static_cast<Razix::Graphics::VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer());
            break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        case Razix::Graphics::RenderAPI::D3D12:
            Razix::Graphics::DX12Utilities::CmdEndLabel(static_cast<Razix::Graphics::DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList());
            break;
#endif
    }
}
