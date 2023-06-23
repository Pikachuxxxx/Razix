#pragma once

#ifdef RAZIX_RENDER_API_OPENGL

    #include <glad/glad.h>
    #include <glfw/glfw3.h>

    #include "Razix/Core/RZLog.h"

    #include "Razix/Graphics/RHI/API/RZPipeline.h"
    #include "Razix/Graphics/RHI/API/RZTexture.h"
    #include "Razix/Graphics/RHI/API/RZVertexBufferLayout.h"

namespace Razix {
    namespace Graphics {
        // TODO: Move implementation to .cpp file

        namespace OpenGLUtilities {

    /* Check the OpenGL functions for any errors and reports them */
    #define GL_CALL(x)                                      \
        ::Razix::Graphics::OpenGLUtilities::GLClearError(); \
        (x);                                                \
        RAZIX_CORE_ASSERT(::Razix::Graphics::OpenGLUtilities::GLLogCall(#x, __FILE__, __LINE__), "[OpenGL Assertion Error]")

            /* Checks for any OpenGL errors */
            static void GLClearError()
            {
                while (glGetError() != GL_NO_ERROR)
                    ;
            }

            /** Logs any error reported by OpenGL Functions
             *
             * @param functionsName The name of the functions that caused the error
             * @param file The file in which the function is
             * @param line The line of the function
             */
            static bool GLLogCall(const char* functionName, const char* file, int line)
            {
                while (GLenum error = glGetError()) {
                    RAZIX_CORE_ERROR("[OpenGL] - ERROR - {0}, (Function : {1} at Line : {2}, in File : {3})", error, functionName, line, file);
                    return false;
                }
                return true;
            }

            //TODO: USe macros to generate this code automatically
            static uint32_t TextureFormatToGL(const RZTextureProperties::Format format, bool srgb)
            {
                switch (format) {
                    case RZTextureProperties::Format::R8:
                        return GL_R8;
                        break;
                    case RZTextureProperties::Format::R32_UINT:
                        return GL_R32UI;
                        break;
                    case RZTextureProperties::Format::R32_INT:
                        return GL_R32I;
                        break;
                    case RZTextureProperties::Format::R32F:
                        return GL_RG32F;
                        break;
                    case RZTextureProperties::Format::RG8:
                        return GL_RG8;
                        break;
                    case RZTextureProperties::Format::RGB8:
                        return srgb ? GL_SRGB8 : GL_RGB8;
                        break;
                    case RZTextureProperties::Format::RGB16:
                        return GL_RGB16;
                        break;
                    case RZTextureProperties::Format::RGB32:
                        return GL_RGB;
                        break;
                    case RZTextureProperties::Format::RGBA8:
                        return srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                        break;
                    case RZTextureProperties::Format::RGBA16:
                        return GL_RGBA16;
                        break;
                    case RZTextureProperties::Format::RGBA32:
                        return GL_RGBA;
                        break;
                    case RZTextureProperties::Format::RGBA32F:
                        return GL_RGBA32F;
                        break;
                    case RZTextureProperties::Format::RGB:
                        return srgb ? GL_SRGB : GL_RGB;
                        break;
                    case RZTextureProperties::Format::RGBA:
                        return srgb ? GL_SRGB_ALPHA : GL_RGBA;
                        break;
                    default:
                        RAZIX_CORE_WARN("GL]Unsupported Texture format");
                        return 0;
                        break;
                }
            }

            static uint32_t TextureWrapToGL(const RZTextureProperties::Wrapping wrap)
            {
                switch (wrap) {
                    case RZTextureProperties::Wrapping::REPEAT:
                        return GL_REPEAT;
                        break;
                    case RZTextureProperties::Wrapping::MIRRORED_REPEAT:
                        return GL_MIRRORED_REPEAT;
                        break;
                    case RZTextureProperties::Wrapping::CLAMP_TO_EDGE:
                        return GL_CLAMP_TO_EDGE;
                        break;
                    case RZTextureProperties::Wrapping::CLAMP_TO_BORDER:
                        return GL_CLAMP_TO_BORDER;
                        break;

                    default:
                        RAZIX_CORE_WARN("Unsupported Texture Wrappign mode");
                        return 0;
                        break;
                }
            }

            static uint32_t TextureFormatToInternalFormat(uint32_t format)
            {
                switch (format) {
                    case GL_SRGB8:
                        return GL_RGB;
                    case GL_SRGB8_ALPHA8:
                        return GL_RGBA;
                    case GL_RGBA:
                        return GL_RGBA;
                    case GL_RGB:
                        return GL_RGB;
                    case GL_R32I:
                    case GL_R32UI:
                    case GL_R32F:
                    case GL_R8:
                        return GL_RED;
                    case GL_RG8:
                        return GL_RG;
                    case GL_RGB8:
                        return GL_RGB;
                    case GL_RGBA8:
                        return GL_RGBA;
                    case GL_RGB16:
                        return GL_RGB;
                    case GL_RGB32F:
                        return GL_RGB;
                    case GL_RGBA32F:
                        return GL_RGBA;
                    case GL_SRGB:
                        return GL_RGB;
                    case GL_SRGB_ALPHA:
                        return GL_RGBA;

                    default:
                        RAZIX_CORE_WARN("[GL] Unsupported Texture Format");
                        return 0;
                }
            }

            static uint32_t BufferFormatToGL(BufferFormat format)
            {
                switch (format) {
                    default:
                        return 0;
                        break;
                    case BufferFormat::R8_INT:
                        return GL_R8;
                        break;
                    case BufferFormat::R8_UINT:
                        return GL_R8;
                        break;
                    case BufferFormat::R32_INT:
                        return GL_R32I;
                        break;
                    case BufferFormat::R32_UINT:
                        return GL_R32UI;
                        break;
                    case BufferFormat::R32_FLOAT:
                        return GL_R32F;
                        break;
                    case BufferFormat::R32G32_INT:
                        return GL_RG32I;
                        break;
                    case BufferFormat::R32G32_UINT:
                        return GL_RG32UI;
                        break;
                    case BufferFormat::R32G32_FLOAT:
                        return GL_RG32F;
                        break;
                    case BufferFormat::R32G32B32_INT:
                        return GL_RGB32I;
                        break;
                    case BufferFormat::R32G32B32_UINT:
                        return GL_RGB32UI;
                        break;
                    case BufferFormat::R32G32B32_FLOAT:
                        return GL_RGB32F;
                        break;
                    case BufferFormat::R32G32B32A32_INT:
                        return GL_RGB32I;
                        break;
                    case BufferFormat::R32G32B32A32_UINT:
                        return GL_RGB32UI;
                        break;
                    case BufferFormat::R32G32B32A32_FLOAT:
                        return GL_RGBA32F;
                        break;
                }
            }

            static uint32_t BufferFormatToGLType(BufferFormat format)
            {
                switch (format) {
                    case Razix::Graphics::BufferFormat::R8_INT:
                    case Razix::Graphics::BufferFormat::R32_INT:
                    case Razix::Graphics::BufferFormat::R32G32_INT:
                    case Razix::Graphics::BufferFormat::R32G32B32_INT:
                    case Razix::Graphics::BufferFormat::R32G32B32A32_INT:
                        return GL_INT;
                        break;
                    case Razix::Graphics::BufferFormat::R8_UINT:
                    case Razix::Graphics::BufferFormat::R32_UINT:
                    case Razix::Graphics::BufferFormat::R32G32_UINT:
                    case Razix::Graphics::BufferFormat::R32G32B32_UINT:
                    case Razix::Graphics::BufferFormat::R32G32B32A32_UINT:
                        return GL_UNSIGNED_INT;
                        break;
                    case Razix::Graphics::BufferFormat::R32_FLOAT:
                    case Razix::Graphics::BufferFormat::R32G32_FLOAT:
                    case Razix::Graphics::BufferFormat::R32G32B32_FLOAT:
                    case Razix::Graphics::BufferFormat::R32G32B32A32_FLOAT:
                        return GL_FLOAT;
                    default:
                        return 0;
                        break;
                }
            }

            static GLenum CullModeToGLCullModeEnum(CullMode mode)
            {
                switch (mode) {
                    case Razix::Graphics::CullMode::BACK:
                        return GL_BACK;
                        break;
                    case Razix::Graphics::CullMode::FRONT:
                        return GL_FRONT;
                        break;
                    case Razix::Graphics::CullMode::FRONTANDBACK:
                        return GL_FRONT_AND_BACK;
                        break;
                    case Razix::Graphics::CullMode::NONE:
                        return GL_BACK;
                        break;
                    default:
                        return GL_BACK;
                        break;
                }
            }

            static GLenum PolygonModeToGLPolygonModeEnum(PolygonMode mode)
            {
                switch (mode) {
                    case Razix::Graphics::PolygonMode::FILL:
                        return GL_FILL;
                        break;
                    case Razix::Graphics::PolygonMode::LINE:
                        return GL_LINE;
                        break;
                    case Razix::Graphics::PolygonMode::POINT:
                        return GL_FILL;
                        break;
                    default:
                        return GL_FILL;
                        break;
                }
            }

        }    // namespace OpenGLUtilities
    }        // namespace Graphics
}    // namespace Razix
#endif