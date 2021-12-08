#include "rzxpch.h"
#include "OpenGLShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"
#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    namespace Graphics {

        OpenGLShader::OpenGLShader(const std::string& filePath)
        {
            m_ShaderFilePath = filePath;

            m_Name = Razix::Utilities::GetFileName(filePath);
            m_Source = RZVirtualFileSystem::Get().readTextFile(filePath);

            Init();
        }

        OpenGLShader::~OpenGLShader()
        {
            GLCall(glDeleteProgram(m_ProgramID));
        }

        void OpenGLShader::Bind() const
        {
            GLCall(glUseProgram(m_ProgramID));
        }

        void OpenGLShader::Unbind() const
        {
            GLCall(glUseProgram(0));
        }

        void OpenGLShader::Init()
        {

        }
    }
}

