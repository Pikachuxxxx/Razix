\    // clang-format off
#include "rzxpch.h"
// clang-format on
#include "GLRenderContext.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Platform/API/OpenGL/OpenGLDescriptorSet.h"
#include "Razix/Platform/API/OpenGL/OpenGLPipeline.h"
#include "Razix/Platform/API/OpenGL/OpenGLShader.h"
#include "Razix/Platform/API/OpenGL/OpenGLUniformBuffer.h"
#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

    namespace Razix
{
    namespace Graphics {

        GLRenderContext::GLRenderContext(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererTitle = "OpenGL";
            m_Width         = width;
            m_Height        = height;
        }

        GLRenderContext::~GLRenderContext()
        {
            // m_context->Release();
        }

        void GLRenderContext::InitAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = OpenGLContext::Get();
        }

        void GLRenderContext::BeginAPIImpl(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            m_CurrentCommandBuffer = cmdBuffer;
        }

        void GLRenderContext::SubmitImpl(RZCommandBuffer* cmdBuffer)
        {
        }

        void GLRenderContext::SubmitWorkImpl()
        {
        }

        void GLRenderContext::PresentAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the swapchain from the m_Context and call Flip on it
            m_Context->getSwapchain()->Flip();
            //glfwSwapBuffers(m_Context->getGLFWWindow());
        }

        void GLRenderContext::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind all the uniform, storage and samplers to the right binding slots here using the information from the descriptors

            // First bind the shader
            auto shader = static_cast<OpenGLPipeline*>(pipeline)->getShader();
            shader->Bind();

            auto glShader = static_cast<OpenGLShader*>(shader);

            for (auto& set: descriptorSets) {
                for (auto& descriptor: static_cast<OpenGLDescriptorSet*>(set)->getDescriptors()) {
                    // Let's bind all the uniform buffers first
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        // Bind the shader to uniform buffer block index
                        // TODO: USe type name instead of the actual variable name, where the fuck do I store that as (type_id)
                        unsigned int BindingIndex = glGetUniformBlockIndex(glShader->getProgramID(), descriptor.typeName.c_str());
                        glUniformBlockBinding(glShader->getProgramID(), BindingIndex, descriptor.bindingInfo.binding);

                        // Bind the buffer itself
                        descriptor.uniformBuffer->Bind();

                        // Assuming it's updated from the renderer side
                        // Time to perform slot binding
                        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, descriptor.bindingInfo.binding, static_cast<OpenGLUniformBuffer*>(descriptor.uniformBuffer)->getHandle(), descriptor.offset, descriptor.size));
                    } else if (descriptor.bindingInfo.type == DescriptorType::IMAGE_SAMPLER) {
                        // Bind the texture
                        descriptor.texture->Bind(descriptor.bindingInfo.binding);
                    }
                }
            }
        }

        void GLRenderContext::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind all the uniform, storage and samplers to the right binding slots here using the information from the descriptors
            // First bind the shader
            auto shader = static_cast<OpenGLPipeline*>(pipeline)->getShader();
            shader->Bind();

            for (uint32_t i = 0; i < totalSets; i++) {
                auto& set = descriptorSets[i];
                for (auto& descriptor: static_cast<OpenGLDescriptorSet*>(set)->getDescriptors()) {
                    // Let's bind all the uniform buffers first
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER) {
                        descriptor.uniformBuffer->Bind();
                        // Assuming it's updated from the renderer side
                        // Time to perform slot binding
                        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, descriptor.bindingInfo.binding, static_cast<OpenGLUniformBuffer*>(descriptor.uniformBuffer)->getHandle(), descriptor.offset, descriptor.size));
                    } else if (descriptor.bindingInfo.type == DescriptorType::IMAGE_SAMPLER) {
                        // First enable the right slot
                        glActiveTexture(GL_TEXTURE0 + descriptor.bindingInfo.binding);
                        // Bind the texture
                        descriptor.texture->Bind(descriptor.bindingInfo.binding);
                    }
                }
            }
        }

        void GLRenderContext::SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            glScissor(x, y, width, height);
        }

        void GLRenderContext::BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::EndRenderingImpl(RZCommandBuffer* cmdBuffer)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().Draws++;
            // TODO: Use pipeline to set the primitive mode here
            glDrawArrays(GL_TRIANGLES, 0, count);
        }

        void GLRenderContext::DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount /*= 1*/, uint32_t firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, uint32_t firstInstance /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().IndexedDraws++;
            // TODO: Use pipeline object to set the primitive mode here
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, nullptr);
        }

        void GLRenderContext::DestroyAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void GLRenderContext::OnResizeAPIImpl(uint32_t width, uint32_t height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            glViewport(0, 0, width, height);
        }

        Razix::Graphics::RZSwapchain* GLRenderContext::GetSwapchainImpl()
        {
            return static_cast<RZSwapchain*>(OpenGLContext::Get()->getSwapchain());
        }

        void GLRenderContext::BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Find the uniform in the set and tag that with this
            auto shader = static_cast<OpenGLPipeline*>(pipeline)->getShader();
            shader->Bind();

            // now use the push constant to create uniform buffer and bind it
            OpenGLUniformBuffer ubo(pushConstant.size, pushConstant.data);
            ubo.Bind();
            ubo.SetData(pushConstant.size, pushConstant.data);

            GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, pushConstant.bindingInfo.binding, ubo.getHandle(), pushConstant.offset, pushConstant.size));

            ubo.Destroy();
        }

        void GLRenderContext::SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }
    }    // namespace Graphics
}    // namespace Razix