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

        /**
         * Load PIX runtime library for GPU debugging
         */
        void LoadPIXRuntime();

        /**
         * Begin a PIX debug label on DirectX 12 command list
         * @param commandList DirectX 12 command list
         * @param name Label name
         * @param color Label color
         */
        void CmdBeginLabel(ID3D12GraphicsCommandList* commandList, const std::string& name, const glm::vec4& color);

        /**
         * Insert a PIX debug marker on DirectX 12 command list
         * @param commandList DirectX 12 command list
         * @param name Marker name
         * @param color Marker color
         */
        void CmdInsertLabel(ID3D12GraphicsCommandList* commandList, const std::string& name, const glm::vec4& color);

        /**
         * End a PIX debug label on DirectX 12 command list
         * @param commandList DirectX 12 command list
         */
        void CmdEndLabel(ID3D12GraphicsCommandList* commandList);

#endif    // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Vulkan Debug Utils Integration
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_VULKAN

        /**
         * Initialize Vulkan debug utilities extension function pointers
         */
        void InitializeDebugUtils();

        /**
         * Begin a debug label on Vulkan command buffer
         * @param commandBuffer Vulkan command buffer
         * @param name Label name
         * @param color Label color
         */
        void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const std::string& name, const glm::vec4& color);

        /**
         * Insert a debug marker on Vulkan command buffer
         * @param commandBuffer Vulkan command buffer
         * @param name Marker name
         * @param color Marker color
         */
        void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const std::string& name, const glm::vec4& color);

        /**
         * End a debug label on Vulkan command buffer
         * @param commandBuffer Vulkan command buffer
         */
        void CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer);

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // Cross-Platform Debug Marker Interface
        //-----------------------------------------------------------------------------------

        /**
         * Begin a debug marker region for GPU profiling (cross-platform)
         * @param cmdBufHandle Command buffer handle
         * @param name Name of the marker region (non-const reference for internal reasons)
         * @param color Color for GPU debuggers
         */
        RAZIX_API void BeginMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const std::string& name, const glm::vec4& color);

        /**
         * Insert a single debug marker (cross-platform)
         * @param cmdBufHandle Command buffer handle
         * @param name Name of the marker
         * @param color Color for GPU debuggers
         */
        RAZIX_API void InsertMarker(rz_gfx_cmdbuf_handle cmdBufHandle, const std::string& name, const glm::vec4& color);

        /**
         * End a debug marker region (cross-platform)
         * @param cmdBufHandle Command buffer handle
         */
        RAZIX_API void EndMarker(rz_gfx_cmdbuf_handle cmdBufHandle);

    }    // namespace Gfx
}    // namespace Razix