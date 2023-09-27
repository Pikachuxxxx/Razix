// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Utilities/RZStringUtilities.h"

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLShader.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKShader.h"
#endif

namespace Razix {
    namespace Graphics {

        GET_INSTANCE_SIZE_IMPL(Shader)

        void RZShader::Create(void* where, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL: new (where) OpenGLShader(filePath); break;
                case Razix::Graphics::RenderAPI::VULKAN: new (where) VKShader(filePath RZ_DEBUG_E_ARG_NAME); break;
                case Razix::Graphics::RenderAPI::D3D11:
                case Razix::Graphics::RenderAPI::D3D12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
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
            ShaderStage                        stage = ShaderStage::NONE;
            std::map<ShaderStage, std::string> shaders;

            for (u32 i = 0; i < lines.size(); i++) {
                std::string str = std::string(lines[i]);
                str             = Utilities::RemoveTabs(str);

                if (Razix::Utilities::StartsWith(str, "#shader")) {
                    if (Razix::Utilities::StringContains(str, "vertex")) {
                        stage                                           = ShaderStage::Vertex;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    } else if (Razix::Utilities::StringContains(str, "geometry")) {
                        stage                                           = ShaderStage::Geometry;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    } else if (Razix::Utilities::StringContains(str, "fragment")) {
                        stage                                           = ShaderStage::Pixel;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    }
                } else if (Razix::Utilities::StartsWith(str, "#ifdef")) {
                    std::string rem                  = "#ifdef ";
                    str                              = Utilities::RemoveStringRange(str, 0, 7);
                    str                              = Razix::Utilities::RemoveSpaces(str);
                    std::vector<std::string> defines = Razix::Utilities::SplitString(str, "||");
                } else if (Razix::Utilities::StartsWith(str, "#include")) {
                    str = Utilities::RemoveStringRange(str, 0, 9);
                    shaders.at(stage).append(str);
                }
            }
            return shaders;
        }

        void RZShader::updateBindVarsHeaps()
        {
            // Since we update the descriptors in each set using references we can just iterate though all sets and update them normally
            // Only update the userSets
            for (auto& set: m_SceneParams.userSets)
                set->UpdateSet(m_DescriptorsPerHeap[set->getSetIdx()]);
        }
    }    // namespace Graphics
}    // namespace Razix