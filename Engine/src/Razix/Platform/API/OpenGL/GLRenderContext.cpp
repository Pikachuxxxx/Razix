\    // clang-format off
#include "rzxpch.h"
// clang-format on
#include "GLRenderContext.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Graphics/Resources/RZResourceManager.h"

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

        GLRenderContext::GLRenderContext(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RendererTitle = "OpenGL";
            m_Width         = width;
            m_Height        = height;

            m_DrawCommandBuffers.resize(MAX_SWAPCHAIN_BUFFERS);
            for (u32 i = 0; i < MAX_SWAPCHAIN_BUFFERS; i++) {
                m_DrawCommandBuffers[i] = Graphics::RZCommandBuffer::Create();
                m_DrawCommandBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Frame Command Buffers #" + std::to_string(i)));
            }
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

        void GLRenderContext::SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
        {
        }

        void GLRenderContext::PresentAPIImpl(RZSemaphore* waitSemaphore)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Get the swapchain from the m_Context and call Flip on it
            m_Context->getSwapchain()->Flip();
            //glfwSwapBuffers(m_Context->getGLFWWindow());
        }

        void GLRenderContext::BindPipelineImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
        {
            auto pp = RZResourceManager::Get().getPool<RZPipeline>().get(pipeline);
            pp->Bind(cmdBuffer);
        }

        void GLRenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets, u32 startSetIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind all the uniform, storage and samplers to the right binding slots here using the information from the descriptors

            // First bind the shader
            auto pp     = RZResourceManager::Get().getPipelineResource(pipeline);
            auto shader = RZResourceManager::Get().getPool<RZShader>().get(pp->getDesc().shader);
            shader->Bind();

            auto glShader = static_cast<OpenGLShader*>(shader);

            for (auto& set: descriptorSets) {
                for (auto& descriptor: static_cast<OpenGLDescriptorSet*>(set)->getDescriptors()) {
                    // Let's bind all the uniform buffers first
                    if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer) {
                        // Bind the shader to uniform buffer block index
                        // TODO: USe type name instead of the actual variable name, where the fuck do I store that as (type_id)
                        unsigned int BindingIndex = glGetUniformBlockIndex(glShader->getProgramID(), descriptor.typeName.c_str());
                        glUniformBlockBinding(glShader->getProgramID(), BindingIndex, descriptor.bindingInfo.location.binding);

                        // Bind the buffer itself
                        auto buffer = RZResourceManager::Get().getUniformBufferResource(descriptor.uniformBuffer);
                        buffer->Bind();

                        // Assuming it's updated from the renderer side
                        // Time to perform slot binding
                        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, descriptor.bindingInfo.location.binding, static_cast<OpenGLUniformBuffer*>(buffer)->getHandle(), descriptor.offset, descriptor.size));
                    } else if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined) {
                        // Bind the texture
                        //descriptor.texture->Bind(descriptor.bindingInfo.location.binding);
                    }
                }
            }
        }

        void GLRenderContext::BindUserDescriptorSetsAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets, u32 startSetIdx)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Bind all the uniform, storage and samplers to the right binding slots here using the information from the descriptors
            // First bind the shader
            auto pp     = RZResourceManager::Get().getPipelineResource(pipeline);
            auto shader = RZResourceManager::Get().getPool<RZShader>().get(pp->getDesc().shader);
            shader->Bind();

            for (u32 i = 0; i < totalSets; i++) {
                const RZDescriptorSet*     set         = descriptorSets[i];
                const OpenGLDescriptorSet* glSet       = static_cast<const OpenGLDescriptorSet*>(set);
                auto                       descriptors = glSet->getDescriptors();
                for (const auto descriptor: descriptors) {
                    // Let's bind all the uniform buffers first
                    if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer) {
                        auto buffer = RZResourceManager::Get().getUniformBufferResource(descriptor.uniformBuffer);
                        buffer->Bind();
                        // Assuming it's updated from the renderer side
                        // Time to perform slot binding
                        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, descriptor.bindingInfo.location.binding, static_cast<OpenGLUniformBuffer*>(buffer)->getHandle(), descriptor.offset, descriptor.size));
                    } else if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined) {
                        // First enable the right slot
                        glActiveTexture(GL_TEXTURE0 + descriptor.bindingInfo.location.binding);
                        // Bind the texture
                        //descriptor.texture->Bind(descriptor.bindingInfo.location.binding);
                    }
                }
            }
        }

        void GLRenderContext::SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
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

        void GLRenderContext::EnableBindlessTexturesImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::BindDescriptorSetAPImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::BindPushDescriptorsImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::InsertImageMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZTextureHandle texture, PipelineBarrierInfo pipelineBarrierInfo, ImageMemoryBarrierInfo imgBarrierInfo)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::InsertBufferMemoryBarrierImpl(RZCommandBuffer* cmdBuffer, RZUniformBuffer* buffer, PipelineBarrierInfo pipelineBarrierInfo, BufferMemoryBarrierInfo bufBarrierInfo)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void GLRenderContext::SetViewportImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)
        {
            glViewport(x, y, width, height);
        }

        void GLRenderContext::DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().Draws++;
            // TODO: Use pipeline to set the primitive mode here
            glDrawArrays(GL_TRIANGLES, 0, count);
        }

        void GLRenderContext::DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount /*= 1*/, u32 firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, u32 firstInstance /*= 0*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RZEngine::Get().GetStatistics().NumDrawCalls++;
            RZEngine::Get().GetStatistics().IndexedDraws++;
            // TODO: Use pipeline object to set the primitive mode here
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        }

        void GLRenderContext::DestroyAPIImpl()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }

        void GLRenderContext::OnResizeAPIImpl(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            glViewport(0, 0, width, height);
        }

        Razix::Graphics::RZSwapchain* GLRenderContext::GetSwapchainImpl()
        {
            return static_cast<RZSwapchain*>(OpenGLContext::Get()->getSwapchain());
        }

        void GLRenderContext::BindPushConstantsAPIImpl(RZPipelineHandle pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Find the uniform in the set and tag that with this
            auto pp     = RZResourceManager::Get().getPipelineResource(pipeline);
            auto shader = RZResourceManager::Get().getPool<RZShader>().get(pp->getDesc().shader);
            shader->Bind();

            // now use the push constant to create uniform buffer and bind it
            OpenGLUniformBuffer ubo({"UNNAMED_OPENGL_PUSH_CONSTANT", pushConstant.size, pushConstant.data});
            ubo.Bind();
            ubo.SetData(pushConstant.size, pushConstant.data);

            GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, pushConstant.bindingInfo.location.binding, ubo.getHandle(), pushConstant.offset, pushConstant.size));

            ubo.Destroy();
        }

        void GLRenderContext::SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        }
    }    // namespace Graphics
}    // namespace Razix