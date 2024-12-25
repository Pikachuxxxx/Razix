// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#include "Razix/Utilities/RZStringUtilities.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKShader.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12Shader.h"
#endif

namespace Razix {
    namespace Gfx {

        GET_INSTANCE_SIZE_IMPL(Shader)

        static std::string ShaderBindaryFileExtension;
        static std::string ShaderBindaryFileDirectory;

        void RZShader::Create(void* where, const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN:
                    ShaderBindaryFileExtension = ".spv";
                    ShaderBindaryFileDirectory = "Compiled/SPIRV/";
                    new (where) VKShader(desc RZ_DEBUG_E_ARG_NAME);
                    break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12:
                    ShaderBindaryFileExtension = ".cso";
                    ShaderBindaryFileDirectory = "Compiled/CSO/";
                    new (where) DX12Shader(desc RZ_DEBUG_E_ARG_NAME);
                    break;
#endif
                default: break;
            }
        }

        std::map<ShaderStage, std::string> RZShader::ParseRZSF(const std::string& filePath)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_TRACE("Parsing .rzsf shader : {0}", filePath);

            std::string rzsfSource = RZVirtualFileSystem::Get().readTextFile(filePath);

            // Break the shader into lines
            std::vector<std::string>           lines = Razix::Utilities::GetLines(rzsfSource);
            ShaderStage                        stage = ShaderStage::kNone;
            std::map<ShaderStage, std::string> shaders;

            for (u32 i = 0; i < lines.size(); i++) {
                std::string str = std::string(lines[i]);
                str             = Utilities::TrimWhitespaces(str);

                if (Razix::Utilities::StartsWith(str, "#shader")) {
                    if (Razix::Utilities::StringContains(str, "vertex")) {
                        stage                                           = ShaderStage::kVertex;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    } else if (Razix::Utilities::StringContains(str, "geometry")) {
                        stage                                           = ShaderStage::kGeometry;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    } else if (Razix::Utilities::StringContains(str, "fragment")) {
                        stage                                           = ShaderStage::kPixel;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    } else if (Razix::Utilities::StringContains(str, "compute")) {
                        stage                                           = ShaderStage::kCompute;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    }
                }
#if 0
// TODO: Add token parsing for #elif defined
else if (Razix::Utilities::StartsWith(str, "#ifdef")) {
                    std::string rem                  = "#ifdef ";
                    str                              = Utilities::RemoveStringRange(str, 0, 7);
                    str                              = Razix::Utilities::RemoveSpaces(str);
                    std::vector<std::string> defines = Razix::Utilities::SplitString(str, "||");
                } else
#endif
                if (Razix::Utilities::StartsWith(str, "#include")) {
                    str = Utilities::RemoveStringRange(str, 0, 9);
                    // Adding the shader extension to load the apt shader
                    str += ShaderBindaryFileExtension;
                    str = ShaderBindaryFileDirectory + str;
                    shaders.at(stage).append(str);
                }
            }
            return shaders;
        }

        void RZShader::updateBindVarsHeaps()
        {
            // Since we update the descriptors in each set using references we can just iterate though all sets and update them normally
            // Only update the userSets
            for (auto& set: m_SceneParams.userSets) {
                auto setResource = RZResourceManager::Get().getDescriptorSetResource(set);
                if (setResource)
                    setResource->UpdateSet(m_DescriptorsPerHeap[setResource->getSetIdx()]);
            }
        }
    }    // namespace Gfx
}    // namespace Razix
