// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGfxDebug.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #define VOLK_IMPLEMENTATION
    #include <volk.h>
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

        static HMODULE WinPixEventRuntimeModule = nullptr;

        // Function pointers for PIX
        typedef HRESULT(WINAPI* BeginEventOnCommandList)(ID3D12GraphicsCommandList*, UINT64, _In_opt_ PCSTR);
        typedef HRESULT(WINAPI* SetMarkerOnCommandList)(ID3D12GraphicsCommandList*, UINT64, _In_opt_ PCSTR);
        typedef HRESULT(WINAPI* EndEventOnCommandList)(ID3D12GraphicsCommandList*);

        void LoadPIXRuntime()
        {
    #ifndef RAZIX_GOLD_MASTER
            if (WinPixEventRuntimeModule == nullptr) {
                WinPixEventRuntimeModule = LoadLibraryA("WinPixEventRuntime.dll");
                if (!WinPixEventRuntimeModule) {
                    RAZIX_CORE_WARN("[DX12] Could not load WinPixEventRuntime.dll for GPU debugging.");
                }
            }
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdBeginLabel(ID3D12GraphicsCommandList* commandList, const std::string& name, const glm::vec4& color)
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

        void CmdInsertLabel(ID3D12GraphicsCommandList* commandList, const std::string& name, const glm::vec4& color)
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

        // Function pointers for Vulkan debug utils
        //static PFN_vkCmdBeginDebugUtilsLabelEXT  vkCmdBeginDebugUtilsLabelEXT  = nullptr;
        //static PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = nullptr;
        //static PFN_vkCmdEndDebugUtilsLabelEXT    vkCmdEndDebugUtilsLabelEXT    = nullptr;

        static bool s_DebugUtilsInitialized = false;

        void InitializeDebugUtils()
        {
            if (s_DebugUtilsInitialized) return;

            VkDevice device = g_GfxCtx.vk.device;

            // Load debug utils extension function pointers
            //vkCmdBeginDebugUtilsLabelEXT  = (PFN_vkCmdBeginDebugUtilsLabelEXT) vkGetDeviceProcAddr(device, "vkCmdBeginDebugUtilsLabelEXT");
            //vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT) vkGetDeviceProcAddr(device, "vkCmdInsertDebugUtilsLabelEXT");
            //vkCmdEndDebugUtilsLabelEXT    = (PFN_vkCmdEndDebugUtilsLabelEXT) vkGetDeviceProcAddr(device, "vkCmdEndDebugUtilsLabelEXT");

            s_DebugUtilsInitialized = true;
        }

        void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const std::string& name, const glm::vec4& color)
        {
    #ifndef RAZIX_GOLD_MASTER
            if (!commandBuffer) return;

            if (!s_DebugUtilsInitialized) {
                InitializeDebugUtils();
            }

            if (vkCmdBeginDebugUtilsLabelEXT) {
                VkDebugUtilsLabelEXT labelInfo{};
                labelInfo.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                labelInfo.pLabelName = name.c_str();
                labelInfo.color[0]   = color.r;
                labelInfo.color[1]   = color.g;
                labelInfo.color[2]   = color.b;
                labelInfo.color[3]   = color.a;

                vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &labelInfo);
            }
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const std::string& name, const glm::vec4& color)
        {
    #ifndef RAZIX_GOLD_MASTER
            if (!commandBuffer) return;

            if (!s_DebugUtilsInitialized) {
                InitializeDebugUtils();
            }

            if (vkCmdInsertDebugUtilsLabelEXT) {
                VkDebugUtilsLabelEXT labelInfo{};
                labelInfo.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                labelInfo.pLabelName = name.c_str();
                labelInfo.color[0]   = color.r;
                labelInfo.color[1]   = color.g;
                labelInfo.color[2]   = color.b;
                labelInfo.color[3]   = color.a;

                vkCmdInsertDebugUtilsLabelEXT(commandBuffer, &labelInfo);
            }
    #endif    // RAZIX_GOLD_MASTER
        }

        void CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
        {
    #ifndef RAZIX_GOLD_MASTER
            if (!commandBuffer) return;

            if (!s_DebugUtilsInitialized) {
                InitializeDebugUtils();
            }

            if (vkCmdEndDebugUtilsLabelEXT) {
                vkCmdEndDebugUtilsLabelEXT(commandBuffer);
            }
    #endif    // RAZIX_GOLD_MASTER
        }

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // Cross-Platform Implementation
        //-----------------------------------------------------------------------------------

        void BeginMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const std::string& name, const glm::vec4& color)
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

        void InsertMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const std::string& name, const glm::vec4& color)
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