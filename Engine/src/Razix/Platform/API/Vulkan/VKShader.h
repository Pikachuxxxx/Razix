#pragma once

#include "Razix/Graphics/RHI/API/RZShader.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {
        class VKShader final : public RZShader
        {
        public:
            VKShader(const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            ~VKShader() {}

            /* Releases the vulkan texture resources */
            RAZIX_CLEANUP_RESOURCE

            void Bind() const override;
            void Unbind() const override;
            void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) override;
            void GenerateDescriptorHeaps() override;

            /* Gets the buffer layout information in engine internal format, this is how the shader expects the vertex buffer data to be packed while uploading to the GPU */
            inline const RZVertexBufferLayout& getBufferLayout() const { return m_BufferLayout; }
            /* Gets the vertex input variables attribute descriptions of the vertex data that are being sent to the shaders via VBO */
            inline const std::vector<VkVertexInputAttributeDescription>& getVertexAttribDescriptions() const { return m_VertexInputAttributeDescriptions; }
            /* Gets Descriptor set info that is used to create the descriptor sets */
            inline const DescriptorsPerHeapMap getDescriptorSetInfos() const { return m_DescriptorsPerHeap; }
            /* Gets the pipeline layout that encapsulates the descriptor sets and push constants information while creating the graphics pipeline */
            inline const VkPipelineLayout& getPipelineLayout() const { return m_PipelineLayout; }
            /* Gets list of per shader stage create infos */
            std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

        private:
            std::vector<VkVertexInputAttributeDescription>           m_VertexInputAttributeDescriptions; /* Vulkan handle for vertex input attribute description that is used by IA/VS for understating vertex data  */
            std::map<u32, std::vector<VkDescriptorSetLayoutBinding>> m_VKSetBindingLayouts;              /* Vulkan handle for descriptor layout binding information per descriptor set location                      */
            std::map<u32, VkDescriptorSetLayout>                     m_PerSetLayouts;                    /* Descriptor set layouts and their corresponding set IDs                                                   */
            std::map<ShaderStage, VkPipelineShaderStageCreateInfo>   m_ShaderCreateInfos;                /* Shader module abstractions that will be used while creating the pipeline to bind the shaders             */
            std::vector<VkPushConstantRange>                         m_VKPushConstants;                  /* Encapsulates the push constants in the shaders                                                           */
            VkPipelineLayout                                         m_PipelineLayout;                   /* Pipeline layout encapsulates the descriptor sets and push constants info for creating graphics pipeline  */

        private:
            void reflectShader();
            void createShaderModules();
        };
    }    // namespace Graphics
}    // namespace Razix
#endif