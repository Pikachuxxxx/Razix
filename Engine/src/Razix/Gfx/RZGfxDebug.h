#pragma once

#include "Razix/Gfx/RHI/RHI.h"
#include <string>

#include <glm/glm.hpp>

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // DirectX 12 PIX Integration
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_DIRECTX12

        void LoadPIXRuntime();
        void CmdBeginLabel(ID3D12GraphicsCommandList* commandList, const std::string& name, const glm::vec4& color);
        void CmdInsertLabel(ID3D12GraphicsCommandList* commandList, const std::string& name, const glm::vec4& color);
        void CmdEndLabel(ID3D12GraphicsCommandList* commandList);

#endif    // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Vulkan Debug Utils Integration
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_VULKAN

        void InitializeDebugUtils();
        void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const std::string& name, const glm::vec4& color);
        void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const std::string& name, const glm::vec4& color);
        void CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer);

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // Cross-Platform Debug Marker Interface
        //-----------------------------------------------------------------------------------

        RAZIX_API void BeginMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const std::string& name, const glm::vec4& color);
        RAZIX_API void InsertMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const std::string& name, const glm::vec4& color);
        RAZIX_API void EndMarker(rz_gfx_cmdbuf_handle cmdBufHandle);

    }    // namespace Gfx
}    // namespace Razix