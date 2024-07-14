// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Pipeline.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"
    #include "Razix/Platform/API/DirectX12/DX12Context.h"
    #include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"

    #include <d3dx12/d3dx12.h>

namespace Razix {
    namespace Graphics {

        DX12Pipeline::DX12Pipeline(const RZPipelineDesc& desc)
        {
            m_Desc = desc;

            //----------------------------
            // Root Signature
            //----------------------------
            // get this from shader reflection/api etc.

            //----------------------------
            // Vertex Input Layout Stage
            //----------------------------
            // D3D12_INPUT_ELEMENT_DESC -> get this from shader reflection.

            //----------------------------
            // Primitive Topology Stage
            //----------------------------
            // Set by IA on command list

            //----------------------------
            // Rasterization Stage
            //----------------------------
        }

        RAZIX_CLEANUP_RESOURCE_IMPL(DX12Pipeline)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            D3D_SAFE_RELEASE(m_PipelineState);
        }

        void DX12Pipeline::Bind(RZDrawCommandBufferHandle cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto cmdBufferResource = RZResourceManager::Get().getDrawCommandBufferResource(cmdBuffer);
            static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList()->SetPipelineState(m_PipelineState);
        }

    }    // namespace Graphics
}    // namespace Razix
#endif