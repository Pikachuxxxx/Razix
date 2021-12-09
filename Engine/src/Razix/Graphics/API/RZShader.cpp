#include "rzxpch.h"
#include "RZShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#include "Razix/Utilities/RZStringUtilities.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLShader.h"
#endif

namespace Razix {
    namespace Graphics {

        RZShader* RZShader::Create(const std::string& filePath)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:       return new OpenGLShader(filePath); break;
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

        std::map<ShaderStage, std::string> RZShader::ParseRZSF(const std::string& filePath)
        {
            RAZIX_CORE_TRACE("Parsing .rzsf shader : {0}", filePath);

            std::string rzsfSource = RZVirtualFileSystem::Get().readTextFile(filePath);

            // Break the shader into lines
            std::vector<std::string> lines = Razix::Utilities::GetLines(rzsfSource);
            ShaderStage stage = ShaderStage::NONE;
            std::map<ShaderStage, std::string> shaders;

            for (uint32_t i = 0; i < lines.size(); i++) {
                std::string str = std::string(lines[i]);

                if (Razix::Utilities::StartsWith(str, "#shader")) {
                    if (Razix::Utilities::StringContains(str, "vertex")) {
                        stage = ShaderStage::VERTEX;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    }
                    else if(Razix::Utilities::StringContains(str, "fragment")) {
                        stage = ShaderStage::PIXEL;
                        std::map<ShaderStage, std::string>::iterator it = shaders.begin();
                        shaders.insert(it, std::pair<ShaderStage, std::string>(stage, ""));
                    }
                }
                else if (Razix::Utilities::StringContains(str, "#ifdef")) {
                    std::string rem = "#ifdef ";
                    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
                }
                else if(Razix::Utilities::StringContains(str, "#include")){
                    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
                    RAZIX_CORE_TRACE("Loading compiled shader : {0}", str);
                    str = str.erase(0, 9);
                    std::string compiledShader = RZVirtualFileSystem::Get().readTextFile("//RazixContent/Shaders/" + str);
                }
            }

            return shaders;
        }

        void RZShader::CrossCompileShader(const std::string& source, ShaderSourceType srcType, ShaderSourceType dst)
        {

        }
    }
}