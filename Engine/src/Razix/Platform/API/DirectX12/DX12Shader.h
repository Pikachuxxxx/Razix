#pragma once

#include "Razix/Gfx/RHI/API/RZShader.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Gfx {

        class DX12Shader final : public RZShader
        {
        public:
            DX12Shader(const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~DX12Shader() {}

            RAZIX_CLEANUP_RESOURCE

            void Bind() const override;
            void Unbind() const override;
            void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) override;
            void GenerateDescriptorHeaps() override;

            RAZIX_INLINE const std::vector<D3D12_INPUT_ELEMENT_DESC>& getVertexAttribDescriptions() const { return m_VertexInputAttributeDescriptions; }
            RAZIX_INLINE u32                                          getVertexAttribDescriptionsCount() const { return m_VertexInputAttributeDescriptions.size(); }
            RAZIX_INLINE ID3DBlob*                                    getShaderStageBlob(ShaderStage stage) { return m_ShaderStageBlobs[stage]; }

        private:
            std::map<ShaderStage, ID3DBlob*>      m_ShaderStageBlobs;
            std::vector<D3D12_INPUT_ELEMENT_DESC> m_VertexInputAttributeDescriptions;

        private:
            // https://simoncoenen.com/blog/programming/graphics/DxcCompiling
            void reflectShader();
            void createShaderModules();
        };
    }    // namespace Gfx
}    // namespace Razix

#endif