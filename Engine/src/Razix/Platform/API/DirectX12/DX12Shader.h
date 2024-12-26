#pragma once

#include "Razix/Gfx/RHI/API/RZShader.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>
    // Don't use the windows SDK includes, use the latest from vendor
    #include <vendor/dxc/inc/d3d12shader.h>
    #include <vendor/dxc/inc/dxcapi.h>

namespace Razix {
    namespace Gfx {

        class DX12Shader final : public RZShader
        {
        public:
            DX12Shader(const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12Shader() {}

            RAZIX_CLEANUP_RESOURCE

            virtual void GenerateUserDescriptorHeaps() override;

            RAZIX_INLINE const std::vector<D3D12_INPUT_ELEMENT_DESC>& getVertexAttribDescriptions() const { return m_VertexInputAttributeDescriptions; }
            RAZIX_INLINE u32                                          getVertexAttribDescriptionsCount() const { return m_VertexInputAttributeDescriptions.size(); }
            RAZIX_INLINE ID3DBlob*                                    getShaderStageBlob(ShaderStage stage) { return m_ShaderStageBlobs[stage]; }

        private:
            std::map<ShaderStage, ID3DBlob*>      m_ShaderStageBlobs;
            std::vector<D3D12_INPUT_ELEMENT_DESC> m_VertexInputAttributeDescriptions;

        private:
            // https://simoncoenen.com/blog/programming/graphics/DxcCompiling
            void reflectShader();
            void reflectVertexInputParams(ID3D12ShaderReflection* shaderReflection, D3D12_SHADER_DESC shaderDesc);
            void reflectDescriptorTables(ShaderStage stage, ID3D12ShaderReflection* shaderReflection, D3D12_SHADER_DESC shaderDesc);
            void createRootSigParams();
            void createShaderModules();
        };
    }    // namespace Gfx
}    // namespace Razix

#endif