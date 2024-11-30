// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Shader.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Platform/API/DirectX12/DX12Context.h"
#include "Razix/Platform/API/DirectX12/DX12Utilities.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Utilities/RZStringUtilities.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

    #include <d3dcompiler.h>

    // Don't use the windows SDK includes, use the latest from vendor
    #include <vendor/dxc/inc/d3d12shader.h>
    #include <vendor/dxc/inc/dxcapi.h>

namespace Razix {
    namespace Gfx {

        // Mask represents the number of components in binary form.
        // ex. float3 would be represented by 7 (0xb111), float2 by 3
        static u32 GetComponentCount(u32 mask)
        {
            u32 count = 0;
            while (mask) {
                count += mask & 1;
                mask >>= 1;
            }
            return count;
        }

        static u32 GetStrideFromDXGIFormat(DXGI_FORMAT format)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (format) {
                case DXGI_FORMAT_R8_SINT:
                    return sizeof(int);
                case DXGI_FORMAT_R32_FLOAT:
                    return sizeof(FLOAT);
                case DXGI_FORMAT_R32G32_FLOAT:
                    return sizeof(glm::vec2);
                case DXGI_FORMAT_R32G32B32_FLOAT:
                    return sizeof(glm::vec3);
                case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    return sizeof(glm::vec4);
                case DXGI_FORMAT_R32G32_SINT:
                    return sizeof(glm::ivec2);
                case DXGI_FORMAT_R32G32B32_SINT:
                    return sizeof(glm::ivec3);
                case DXGI_FORMAT_R32G32B32A32_SINT:
                    return sizeof(glm::ivec4);
                case DXGI_FORMAT_R32G32_UINT:
                    return sizeof(glm::uvec2);
                case DXGI_FORMAT_R32G32B32_UINT:
                    return sizeof(glm::uvec3);
                case DXGI_FORMAT_R32G32B32A32_UINT:
                    return sizeof(glm::uvec4);    //Need uintvec?
                case DXGI_FORMAT_R32_UINT:
                    return sizeof(u32);
                default:
                    RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                    return 0;
            }

            return 0;
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

                        // Store the buffer layout in engine friendly format
                        DX12Utilities::PushBufferLayout(format, paramDesc.SemanticName, m_BufferLayout);

                        // Store the stride for vertex data
                        m_VertexInputStride += formatSize;
                    }
                }

                //--------------------------
                // Reflect descriptor tables (Buffers/Textures etc.)
                //--------------------------
                for (u32 i = 0; i < shaderDesc.BoundResources; i++) {
                    D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc{};
                    CHECK_HRESULT(shaderReflection->GetResourceBindingDesc(i, &shaderInputBindDesc));

                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t Name: {0}", shaderInputBindDesc.Name);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t Type: {0}", shaderInputBindDesc.Type);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t BindPoint: {0}", shaderInputBindDesc.BindPoint);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t BindCount: {0}", shaderInputBindDesc.BindCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t uFlags: {0}", shaderInputBindDesc.uFlags);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t ReturnType: {0}", shaderInputBindDesc.ReturnType);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t Dimension: {0}", shaderInputBindDesc.Dimension);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t NumSamples: {0}", shaderInputBindDesc.NumSamples);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t Space: {0}", shaderInputBindDesc.Space);
                    RAZIX_CORE_TRACE("[DX12] [Shader Input Bind Desc] \t uID: {0}", shaderInputBindDesc.uID);

                    //0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0

                    RZDescriptor* rzDescriptor = new RZDescriptor;
                    u32           setIndex     = 0;

                    auto& descriptors_in_set = m_DescriptorsPerHeap[setIndex];
                    descriptors_in_set.push_back(std::move(*rzDescriptor));

                    delete rzDescriptor;
                }

    // Print some debug info
    #if RAZIX_DEBUG
                {
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t ConstantBuffers: {0}", shaderDesc.ConstantBuffers);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t BoundResources: {0}", shaderDesc.BoundResources);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t InputParameters: {0}", shaderDesc.InputParameters);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t OutputParameters: {0}", shaderDesc.OutputParameters);
                    RAZIX_CORE_TRACE("---------------------------------------------------");
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t InstructionCount: {0}", shaderDesc.InstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TempRegisterCount: {0}", shaderDesc.TempRegisterCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TempArrayCount: {0}", shaderDesc.TempArrayCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t DefCount: {0}", shaderDesc.DefCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t DclCount: {0}", shaderDesc.DclCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TextureNormalInstructions: {0}", shaderDesc.TextureNormalInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TextureLoadInstructions: {0}", shaderDesc.TextureLoadInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TextureCompInstructions: {0}", shaderDesc.TextureCompInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TextureBiasInstructions: {0}", shaderDesc.TextureBiasInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TextureGradientInstructions: {0}", shaderDesc.TextureGradientInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t FloatInstructionCount: {0}", shaderDesc.FloatInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t IntInstructionCount: {0}", shaderDesc.IntInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t UintInstructionCount: {0}", shaderDesc.UintInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t StaticFlowControlCount: {0}", shaderDesc.StaticFlowControlCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t DynamicFlowControlCount: {0}", shaderDesc.DynamicFlowControlCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t MacroInstructionCount: {0}", shaderDesc.MacroInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t ArrayInstructionCount: {0}", shaderDesc.ArrayInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t CutInstructionCount: {0}", shaderDesc.CutInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t EmitInstructionCount: {0}", shaderDesc.EmitInstructionCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t GSOutputTopology: {0}", shaderDesc.GSOutputTopology);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t GSMaxOutputVertexCount: {0}", shaderDesc.GSMaxOutputVertexCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t InputPrimitive: {0}", shaderDesc.InputPrimitive);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t PatchConstantParameters: {0}", shaderDesc.PatchConstantParameters);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t cGSInstanceCount: {0}", shaderDesc.cGSInstanceCount);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t cControlPoints: {0}", shaderDesc.cControlPoints);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t HSOutputPrimitive: {0}", shaderDesc.HSOutputPrimitive);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t HSPartitioning: {0}", shaderDesc.HSPartitioning);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t TessellatorDomain: {0}", shaderDesc.TessellatorDomain);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t cBarrierInstructions: {0}", shaderDesc.cBarrierInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t cInterlockedInstructions: {0}", shaderDesc.cInterlockedInstructions);
                    RAZIX_CORE_TRACE("[DX12] [Shader Reflection] \t cTextureStoreInstructions: {0}", shaderDesc.cTextureStoreInstructions);
                }
    #endif

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

                ID3DBlob* blob = nullptr;
                CHECK_HRESULT(D3DCreateBlob(csoByteSize, &blob));
                memcpy(blob->GetBufferPointer(), csoByteCode, csoByteSize);
                m_ShaderStageBlobs[spvSource.first] = blob;

                // TODO: Name tag shader blobs D3D12_TAG_OBJECT(blob);

                delete csoByteCode;
            }
        }
    }    // namespace Gfx
}    // namespace Razix

#endif