// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Shader.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Platform/API/DirectX12/DX12Context.h"
#include "Razix/Platform/API/DirectX12/DX12Utilities.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Utilities/RZStringUtilities.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3dcompiler.h>

    // Don't use the windows SDK includes, use the latest from vendor
    #include <vendor/dxc/inc/d3d12shader.h>
    #include <vendor/dxc/inc/dxcapi.h>

namespace Razix {
    namespace Graphics {

        static u32 GetComponentCount(u32 mask)
        {
            u32 count = 0;
            if (mask & 0x1) count++;
            if (mask & 0x2) count++;
            if (mask & 0x4) count++;
            if (mask & 0x8) count++;
            return count;
        }

        DX12Shader::DX12Shader(const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Desc = desc;

            // Attach the binary extension before loading shaders
            setShaderFilePath(desc.filePath);

            m_Desc.name = Razix::Utilities::GetFileName(desc.filePath);

            // Read the *.rzsf shader file to get the necessary shader stages and it's corresponding compiled shader file virtual path
            m_ParsedRZSF = RZShader::ParseRZSF(desc.filePath);

            CrossCompileShaders(m_ParsedRZSF, ShaderSourceType::HLSL);

            // Reflect the shaders using SPIR-V Reflect to extract the necessary information about descriptors and inputs to the shaders
            reflectShader();

            // Create the shader modules and the pipeline shader stage create infos that will be bound to the pipeline
            createShaderModules();
        }

        RAZIX_CLEANUP_RESOURCE_IMPL(DX12Shader)
        {
            // Destroy the shader blobs
            for (const auto& csoSource: m_ParsedRZSF)
                D3D_SAFE_RELEASE(m_ShaderStageBlobs[csoSource.first]);
        }

        void DX12Shader::Bind() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12Shader::Unbind() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

        void DX12Shader::CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (srcType != ShaderSourceType::SPIRV)
                return;
        }

        void DX12Shader::GenerateDescriptorHeaps()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void DX12Shader::reflectShader()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            bool potentiallyBindless = false;

            IDxcUtils* shaderReflectionUtils = nullptr;
            CHECK_HRESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&shaderReflectionUtils)));

            // Reflect the CSO/DXIL shader to extract all the necessary information
            for (const auto& csoSource: m_ParsedRZSF) {
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + csoSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);

                i64        csoByteSize = RZFileSystem::GetFileSize(outPath);
                const u32* csoByteCode = reinterpret_cast<u32*>(RZVirtualFileSystem::Get().readFile(virtualPath));
                // Create a blob from the CSO data
                IDxcBlobEncoding* csoBlob = nullptr;
                CHECK_HRESULT(shaderReflectionUtils->CreateBlob(csoByteCode, (u32) csoByteSize, CP_UTF8, &csoBlob));

                DxcBuffer shaderSourceBuffer = {};
                shaderSourceBuffer.Ptr       = csoBlob->GetBufferPointer();
                shaderSourceBuffer.Size      = csoBlob->GetBufferSize();
                shaderSourceBuffer.Encoding  = 0;

                ID3D12ShaderReflection* shaderReflection = nullptr;
                CHECK_HRESULT(shaderReflectionUtils->CreateReflection(&shaderSourceBuffer, IID_PPV_ARGS(&shaderReflection)));
                // Get the shader desc reflection data
                D3D12_SHADER_DESC shaderDesc{};
                CHECK_HRESULT(shaderReflection->GetDesc(&shaderDesc));

                //--------------------------
                // Get the vertex input desc
                //--------------------------
                if (csoSource.first == ShaderStage::Vertex) {
                    m_VertexInputStride = 0;

                    // TODO: Fill D3D12_INPUT_ELEMENT_DESC
                    for (u32 i = 0; i < shaderDesc.InputParameters; ++i) {
                        D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
                        CHECK_HRESULT(shaderReflection->GetInputParameterDesc(i, &paramDesc));
                        RAZIX_CORE_INFO("[DX12] [Shader Reflection] Input Params: {0} type: {1} elements/register: {2}", paramDesc.SemanticName, paramDesc.ComponentType, paramDesc.Register);

                        u32 componentCount = GetComponentCount(paramDesc.Mask);
                        RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t input param component count : {0}", componentCount);
                        DXGI_FORMAT format     = DX12Utilities::GetFormatFromComponentType(paramDesc.ComponentType, componentCount);
                        u32         formatSize = DX12Utilities::GetFormatSize(format);

                        D3D12_INPUT_ELEMENT_DESC inputElement = {};
                        inputElement.SemanticName             = paramDesc.SemanticName;
                        inputElement.SemanticIndex            = paramDesc.SemanticIndex;
                        inputElement.Format                   = format;
                        inputElement.InputSlot                = 0;
                        inputElement.AlignedByteOffset        = D3D12_APPEND_ALIGNED_ELEMENT;
                        inputElement.InputSlotClass           = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                        inputElement.InstanceDataStepRate     = 0;

                        m_VertexInputAttributeDescriptions.push_back(inputElement);

                        // TODO: Create the buffer layout for Razix engine
                        //pushBufferLayout((VkFormat) inputVar.format, inputVar.name, m_BufferLayout);

                        m_VertexInputStride += formatSize;
                    }
                }

                delete csoByteCode;
                D3D_SAFE_RELEASE(csoBlob);
                D3D_SAFE_RELEASE(shaderReflection);
            }

            D3D_SAFE_RELEASE(shaderReflectionUtils);
        }

        void DX12Shader::createShaderModules()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (const auto& spvSource: m_ParsedRZSF) {
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);

                i64        csoByteSize = RZFileSystem::GetFileSize(outPath);
                const u32* csoByteCode = reinterpret_cast<u32*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                ID3DBlob* blob = m_ShaderStageBlobs[spvSource.first];
                CHECK_HRESULT(D3DCreateBlob(csoByteSize, &blob));
                memcpy(blob->GetBufferPointer(), csoByteCode, csoByteSize);

                // TODO: Name tag shader blobs D3D12_TAG_OBJECT(blob);

                delete csoByteCode;
            }
        }
    }    // namespace Graphics
}    // namespace Razix

#endif