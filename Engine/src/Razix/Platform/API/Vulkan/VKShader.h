#pragma once

#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZVertexBufferLayout.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include <vulkan/vulkan.h>

namespace Razix  { 
    namespace Graphics {
        class VKShader : public RZShader
        {
        public:
            VKShader(const std::string& filePath);
            ~VKShader();

            void Bind() const override;
            void Unbind() const override;
            void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) override;

            /* Gets the vertex input variables attribute descriptions of the vertex data that are being sent to the shaders via VBO */
            inline const std::vector<VkVertexInputAttributeDescription>& getVertexAttribDescriptions() const { return m_VertexInputAttributeDescriptions; }
            /* Gets the buffer layout information in engine internal format, this is how the shader expects the vertex buffer data to be packed while uploading to the GPU */
            inline const RZVertexBufferLayout& getBufferLayout() const { return m_BufferLayout; }
        private:
            std::vector<VkVertexInputAttributeDescription>                          m_VertexInputAttributeDescriptions; /* Vulkan handle for vertex input attribute description that is used by IA/VS for understating vertex data  */
            RZVertexBufferLayout                                                    m_BufferLayout;                     /* Detailed description of the input data format that has been extracted from the shader                    */
            uint32_t                                                                m_VertexInputStride = 0;            /* The stride of the vertex data that is extracted from the information                                     */
            std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> m_VKSetBindingLayouts;              /* Vulkan handle for descriptor layout binding information per descriptor set location                      */
            std::unordered_map<uint32_t, std::vector<DescriptorLayoutBinding>>      m_SetLayoutBindings;                /* This describes the Layout bindings information of the resources bound per set i.e. per SetID             */
            std::unordered_map<uint32_t, VkDescriptorSetLayout>                     m_SetLayouts;                       /* Descriptor set layouts and their corresponding set IDs                                                   */
        private:
            void init();
        };
    }
}
#endif