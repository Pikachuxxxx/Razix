#include "rzxpch.h"
#include "RZShader.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

namespace Razix {
    namespace Graphics {

        RZShader* RZShader::Create(const std::string& filePath)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:
                case Razix::Graphics::RenderAPI::VULKAN:
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }

        void RZShader::readShader()
        {

        }

        void RZShader::ReflectShader(const ShaderSourceType& sourceType)
        {

        }

        void RZShader::ReflectGLSLShader()
        {

        }

        void RZShader::ReflectSPIRVShader()
        {

        }

        void RZShader::ReflectHLSLShader()
        {

        }

        void RZShader::ReflectPSSLShader()
        {

        }

        void RZShader::ReflectCgShader()
        {

        }

    }
}