// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Pipeline.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include "Razix/Platform/API/DirectX12/DX12Context.h"
    #include "Razix/Platform/API/DirectX12/DX12DrawCommandBuffer.h"
    #include "Razix/Platform/API/DirectX12/DX12Shader.h"
    #include "Razix/Platform/API/DirectX12/DX12Utilities.h"

    #include <d3dx12/d3dx12.h>

namespace Razix {
    namespace Graphics {

        // Function to convert const char* to LPCWSTR
        LPCWSTR ConvertToLPCWSTR(const char* str)
        {
            // Calculate the number of wide characters required
            int wideCharLength = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);

            // Allocate buffer for wide characters
            wchar_t* wideStr = new wchar_t[wideCharLength];

            // Perform the actual conversion
            MultiByteToWideChar(CP_ACP, 0, str, -1, wideStr, wideCharLength);

            return wideStr;
        }

        DX12Pipeline::DX12Pipeline(const RZPipelineDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc = desc;

            auto        shaderResource     = RZResourceManager::Get().getPool<RZShader>().get(m_Desc.shader);
            DX12Shader* dx12ShaderResource = static_cast<DX12Shader*>(shaderResource);

            //----------------------------
            // Vertex Input Layout Stage
            //----------------------------
            // D3D12_INPUT_ELEMENT_DESC -> get this from shader reflection.
            auto& inputDescs = dx12ShaderResource->getVertexAttribDescriptions();

            //----------------------------
            // Root Signature
            //----------------------------
            // Get this from shader reflection/api etc.
            // For the test of HELLO_TRIANGLE let's give one manually as a ref to compare it with reflection gen.
            D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.NumParameters     = 0;
            rootSignatureDesc.pParameters       = nullptr;
            rootSignatureDesc.NumStaticSamplers = 0;
            rootSignatureDesc.pStaticSamplers   = nullptr;
            rootSignatureDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
            ID3DBlob* pSerializedRootSignature  = nullptr;
            ID3DBlob* pErrorBlob                = nullptr;
            CHECK_HRESULT(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRootSignature, &pErrorBlob));

            // We manually create the root signature here, get this from the DX12Shader instance by reflecting
            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateRootSignature(0, pSerializedRootSignature->GetBufferPointer(), pSerializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)));

            //----------------------------
            // Primitive Topology Stage
            //----------------------------
            // Set by IA on command list

            //----------------------------
            // Rasterization Stage
            //----------------------------

            //----------------------------
            // Depth Stencil Stage
            //----------------------------
            D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
            depthStencilDesc.DepthEnable                  = FALSE;
            depthStencilDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
            depthStencilDesc.StencilEnable                = FALSE;
            depthStencilDesc.StencilReadMask              = D3D12_DEFAULT_STENCIL_READ_MASK;
            depthStencilDesc.StencilWriteMask             = D3D12_DEFAULT_STENCIL_WRITE_MASK;
            depthStencilDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS;
            depthStencilDesc.BackFace                     = depthStencilDesc.FrontFace;    // Use same settings for back face

            //----------------------------
            // Pipeline
            //----------------------------

            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
            psoDesc.InputLayout                        = {inputDescs.data(), dx12ShaderResource->getVertexAttribDescriptionsCount()};
            psoDesc.pRootSignature                     = m_pRootSignature;
            psoDesc.VS                                 = CD3DX12_SHADER_BYTECODE(dx12ShaderResource->getShaderStageBlob(ShaderStage::Vertex));
            psoDesc.PS                                 = CD3DX12_SHADER_BYTECODE(dx12ShaderResource->getShaderStageBlob(ShaderStage::Pixel));
            psoDesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.DepthStencilState                  = depthStencilDesc;
            psoDesc.SampleMask                         = UINT_MAX;
            psoDesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets                   = 1;
            psoDesc.RTVFormats[0]                      = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.SampleDesc.Count                   = 1;

            CHECK_HRESULT(DX12Context::Get()->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

            D3D12_TAG_OBJECT(m_PipelineState, ConvertToLPCWSTR(bufferName.c_str()));
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
            auto commandList       = static_cast<DX12DrawCommandBuffer*>(cmdBufferResource)->getD3DCommandList();

            commandList->SetGraphicsRootSignature(m_pRootSignature);

            commandList->SetPipelineState(m_PipelineState);

            // Set by IA on command list
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
    }    // namespace Graphics
}    // namespace Razix
#endif