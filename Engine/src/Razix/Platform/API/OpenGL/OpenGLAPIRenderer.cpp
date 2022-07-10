#include "rzxpch.h"
#include "OpenGLAPIRenderer.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace Razix {
    namespace Graphics {

        OpenGLAPIRenderer::OpenGLAPIRenderer(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererTitle = "OpenGL";
            m_Width = width;
            m_Height = height;
        }

        OpenGLAPIRenderer::~OpenGLAPIRenderer()
        {
            // m_context->Release();
        }

        void OpenGLAPIRenderer::InitAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = OpenGLContext::Get();
        }

        void OpenGLAPIRenderer::BeginAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        void OpenGLAPIRenderer::PresentAPIImple(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            glfwSwapBuffers(m_Context->getGLFWWindow());
        }

        void OpenGLAPIRenderer::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        void OpenGLAPIRenderer::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        void OpenGLAPIRenderer::SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            glScissor(x, y, width, height);
        }

        void OpenGLAPIRenderer::DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: Use pipeline to set the primitive mode here
            glDrawArrays(GL_TRIANGLES, 0, count);
        }

        void OpenGLAPIRenderer::DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount /*= 1*/, uint32_t firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, uint32_t firstInstance /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: Use pipeline to set the primitive mode here
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        }

        void OpenGLAPIRenderer::DestroyAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        void OpenGLAPIRenderer::OnResizeAPIImpl(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            glViewport(0, 0, width, height);
        }

        Razix::Graphics::RZSwapchain* OpenGLAPIRenderer::GetSwapchainImpl()
        {
            return nullptr;
        }

        void OpenGLAPIRenderer::BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, size_t blockSize, void* data)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        }

        void OpenGLAPIRenderer::SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }
    }
}