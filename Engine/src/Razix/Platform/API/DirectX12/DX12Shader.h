#pragma once

#include "Razix/Graphics/RHI/API/RZShader.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3d12.h>

namespace Razix {
    namespace Graphics {

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

        private:
            std::map<ShaderStage, ID3DBlob*> m_ShaderStageBlobs;

        private:
            // https://simoncoenen.com/blog/programming/graphics/DxcCompiling
            void reflectShader();
            void createShaderModules();
        };
    }    // namespace Graphics
}    // namespace Razix

#endif