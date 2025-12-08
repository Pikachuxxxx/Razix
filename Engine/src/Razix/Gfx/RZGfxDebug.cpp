// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGfxDebug.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #ifdef RAZIX_PLATFORM_WINDOWS
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif

    #if defined(RAZIX_PLATFORM_MACOS)
        #define VK_USE_PLATFORM_MACOS_MVK
    #endif

    #if defined(RAZIX_PLATFORM_LINUX)
        #define VK_USE_PLATFORM_WAYLAND_KHR
        #define VK_USE_PLATFORM_XCB_KHR
        #define VK_USE_PLATFORM_XLIB_KHR
        #define RAZIX_PLATFORM_WAYLAND
        #define RAZIX_PLATFORM_XCB
        #define RAZIX_PLATFORM_XLIB
    #endif
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include <d3d12.h>
#endif

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // DirectX 12 PIX Implementation
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_DIRECTX12

        static HMODULE WinPixEventRuntimeModule = NULL;

        // Function pointers for PIX
        typedef HRESULT(WINAPI* BeginEventOnCommandList)(ID3D12GraphicsCommandList*, UINT64, _In_opt_ PCSTR);
        typedef HRESULT(WINAPI* SetMarkerOnCommandList)(ID3D12GraphicsCommandList*, UINT64, _In_opt_ PCSTR);
        typedef HRESULT(WINAPI* EndEventOnCommandList)(ID3D12GraphicsCommandList*);

        void LoadPIXRuntime()
        {
    #ifndef RAZIX_GOLD_MASTER
            if (WinPixEventRuntimeModule == NULL) {
                WinPixEventRuntimeModule = LoadLibraryA("WinPixEventRuntime.dll");
                if (!WinPixEventRuntimeModule) {
                    RAZIX_CORE_WARN("[DX12] Could not load WinPixEventRuntime.dll for GPU debugging.");
                }
            }
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdBeginLabel(ID3D12GraphicsCommandList* commandList, const RZString& name, const glm::vec4& color)
        {
    #ifndef RAZIX_GOLD_MASTER
            if (!commandList) return;

            if (!WinPixEventRuntimeModule) {
                LoadPIXRuntime();
                if (!WinPixEventRuntimeModule) return;
            }

            BeginEventOnCommandList pixBeginEventOnCommandList =
                (BeginEventOnCommandList) GetProcAddress(WinPixEventRuntimeModule, "PIXBeginEventOnCommandList");

            if (pixBeginEventOnCommandList) {
                UINT64 colorARGB = static_cast<UINT64>(
                    (static_cast<UINT32>(color.a * 255.0f) << 24) |
                    (static_cast<UINT32>(color.r * 255.0f) << 16) |
                    (static_cast<UINT32>(color.g * 255.0f) << 8) |
                    (static_cast<UINT32>(color.b * 255.0f)));
                pixBeginEventOnCommandList(commandList, colorARGB, name.c_str());
            }
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdInsertLabel(ID3D12GraphicsCommandList* commandList, const RZString& name, const glm::vec4& color)
        {
    #ifndef RAZIX_GOLD_MASTER
            if (!commandList) return;

            if (!WinPixEventRuntimeModule) {
                LoadPIXRuntime();
                if (!WinPixEventRuntimeModule) return;
            }

            SetMarkerOnCommandList pixSetMarkerOnCommandList =
                (SetMarkerOnCommandList) GetProcAddress(WinPixEventRuntimeModule, "PIXSetMarkerOnCommandList");

            if (pixSetMarkerOnCommandList) {
                UINT64 colorARGB = static_cast<UINT64>(
                    (static_cast<UINT32>(color.a * 255.0f) << 24) |
                    (static_cast<UINT32>(color.r * 255.0f) << 16) |
                    (static_cast<UINT32>(color.g * 255.0f) << 8) |
                    (static_cast<UINT32>(color.b * 255.0f)));
                pixSetMarkerOnCommandList(commandList, colorARGB, name.c_str());
            }
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdEndLabel(ID3D12GraphicsCommandList* commandList)
        {
    #ifndef RAZIX_GOLD_MASTER
            if (!commandList) return;

            if (!WinPixEventRuntimeModule) {
                LoadPIXRuntime();
                if (!WinPixEventRuntimeModule) return;
            }

            EndEventOnCommandList pixEndEventOnCommandList =
                (EndEventOnCommandList) GetProcAddress(WinPixEventRuntimeModule, "PIXEndEventOnCommandList");

            if (pixEndEventOnCommandList) {
                pixEndEventOnCommandList(commandList);
            }
    #endif    // RAZIX_GOLD_MASTER
        }

#endif    // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Vulkan Debug Utils Implementation
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_VULKAN

        void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const RZString& name, const glm::vec4& color)
        {
            RAZIX_UNUSED(commandBuffer);
            RAZIX_UNUSED(name);
            RAZIX_UNUSED(color);

    #ifndef RAZIX_GOLD_MASTER
            VkDebugUtilsLabelEXT labelInfo{};
            labelInfo.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelInfo.pLabelName = name.c_str();
            labelInfo.color[0]   = color.r;
            labelInfo.color[1]   = color.g;
            labelInfo.color[2]   = color.b;
            labelInfo.color[3]   = color.a;
            RAZIX_UNUSED(labelInfo);

            rzRHI_BRIDGE_vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &labelInfo);
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const RZString& name, const glm::vec4& color)
        {
            RAZIX_UNUSED(commandBuffer);
            RAZIX_UNUSED(name);
            RAZIX_UNUSED(color);

    #ifndef RAZIX_GOLD_MASTER
            VkDebugUtilsLabelEXT labelInfo{};
            labelInfo.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelInfo.pLabelName = name.c_str();
            labelInfo.color[0]   = color.r;
            labelInfo.color[1]   = color.g;
            labelInfo.color[2]   = color.b;
            labelInfo.color[3]   = color.a;
            RAZIX_UNUSED(labelInfo);

            rzRHI_BRIDGE_vkCmdInsertDebugUtilsLabelEXT(commandBuffer, &labelInfo);
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
        {
            RAZIX_UNUSED(commandBuffer);

    #ifndef RAZIX_GOLD_MASTER
            rzRHI_BRIDGE_vkCmdEndDebugUtilsLabelEXT(commandBuffer);
    #endif    // RAZIX_GOLD_MASTER
        }

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // Cross-Platform Implementation
        //-----------------------------------------------------------------------------------

        void BeginMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const RZString& name, const glm::vec4& color)
        {
#ifndef RAZIX_GOLD_MASTER
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the current command buffer
            auto cmdBuf = Gfx::RZResourceManager::Get().getCommandBufferResource(cmdBufHandle);

            if (!cmdBuf) return;

            switch (rzGfxCtx_GetRenderAPI()) {
    #ifdef RAZIX_RENDER_API_VULKAN
                case RZ_RENDER_API_VULKAN:
                    CmdBeginDebugUtilsLabelEXT(
                        cmdBuf->vk.cmdBuf,
                        name,
                        color);
                    break;
    #endif
    #ifdef RAZIX_RENDER_API_DIRECTX12
                case RZ_RENDER_API_D3D12:
                    CmdBeginLabel(
                        cmdBuf->dx12.cmdList,
                        name,
                        color);
                    break;
    #endif
                default:
                    break;
            }
#endif    // RAZIX_GOLD_MASTER
        }

        void InsertMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const RZString& name, const glm::vec4& color)
        {
#ifndef RAZIX_GOLD_MASTER
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the current command buffer
            auto cmdBuf = Gfx::RZResourceManager::Get().getCommandBufferResource(cmdBufHandle);

            if (!cmdBuf) return;

            switch (rzGfxCtx_GetRenderAPI()) {
    #ifdef RAZIX_RENDER_API_VULKAN
                case RZ_RENDER_API_VULKAN:
                    CmdInsertDebugUtilsLabelEXT(
                        cmdBuf->vk.cmdBuf,
                        name,
                        color);
                    break;
    #endif
    #ifdef RAZIX_RENDER_API_DIRECTX12
                case RZ_RENDER_API_D3D12:
                    CmdInsertLabel(
                        cmdBuf->dx12.cmdList,
                        name,
                        color);
                    break;
    #endif
                default:
                    break;
            }
#endif    // RAZIX_GOLD_MASTER
        }

        void EndMarker(rz_gfx_cmdbuf_handle cmdBufHandle)
        {
#ifndef RAZIX_GOLD_MASTER
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the current command buffer
            auto cmdBuf = Gfx::RZResourceManager::Get().getCommandBufferResource(cmdBufHandle);

            if (!cmdBuf) return;

            switch (rzGfxCtx_GetRenderAPI()) {
    #ifdef RAZIX_RENDER_API_VULKAN
                case RZ_RENDER_API_VULKAN:
                    CmdEndDebugUtilsLabelEXT(
                        cmdBuf->vk.cmdBuf);
                    break;
    #endif
    #ifdef RAZIX_RENDER_API_DIRECTX12
                case RZ_RENDER_API_D3D12:
                    CmdEndLabel(
                        cmdBuf->dx12.cmdList);
                    break;
    #endif
                default:
                    break;
            }
#endif    // RAZIX_GOLD_MASTER
        }

    }    // namespace Gfx
}    // namespace Razix
