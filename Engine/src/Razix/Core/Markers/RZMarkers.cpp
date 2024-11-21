// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMarkers.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/RHI.h"

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
    auto cmdBuf            = Razix::Gfx::RHI::GetCurrentCommandBuffer();
    auto cmdBufferResource = Razix::Gfx::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuf);

    switch (Razix::Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
        case Razix::Gfx::RenderAPI::VULKAN:
            Razix::Gfx::VKUtilities::CmdBeginDebugUtilsLabelEXT(static_cast<Razix::Gfx::VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer(), name, color);
            break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        case Razix::Gfx::RenderAPI::D3D12:
            Razix::Gfx::DX12Utilities::CmdBeginLabel(static_cast<Razix::Gfx::DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList(), name, color);
            break;
#endif
        default: break;
    }
}

void InsertMarker(const std::string& name, glm::vec4 color)
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf            = Razix::Gfx::RHI::GetCurrentCommandBuffer();
    auto cmdBufferResource = Razix::Gfx::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuf);

    switch (Razix::Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
        case Razix::Gfx::RenderAPI::VULKAN:
            Razix::Gfx::VKUtilities::CmdInsertDebugUtilsLabelEXT(static_cast<Razix::Gfx::VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer(), name, color);
            break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        case Razix::Gfx::RenderAPI::D3D12: break;
#endif
        default: break;
    }
}

void EndMarker()
{
    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

    // Get the current command buffer
    auto cmdBuf            = Razix::Gfx::RHI::GetCurrentCommandBuffer();
    auto cmdBufferResource = Razix::Gfx::RZResourceManager::Get().getDrawCommandBufferResource(cmdBuf);

    switch (Razix::Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
        case Razix::Gfx::RenderAPI::VULKAN:
            Razix::Gfx::VKUtilities::CmdEndDebugUtilsLabelEXT(static_cast<Razix::Gfx::VKDrawCommandBuffer*>(cmdBufferResource)->getBuffer());
            break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
        case Razix::Gfx::RenderAPI::D3D12:
            Razix::Gfx::DX12Utilities::CmdEndLabel(static_cast<Razix::Gfx::DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList());
            break;
#endif
        default: break;
    }
}
