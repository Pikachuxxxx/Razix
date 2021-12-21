#include "rzxpch.h"
#include "OpenGLShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#ifdef RAZIX_RENDER_API_OPENGL

#include "Razix/Platform/API/OpenGL/OpenGLUtilities.h"
#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    namespace Graphics {

        enum root_signature_spaces
        {
            PUSH_CONSTANT_REGISTER_SPACE = 0,
            DYNAMIC_OFFSET_SPACE,
            DESCRIPTOR_TABLE_INITIAL_SPACE,
        };

        OpenGLShader::OpenGLShader(const std::string& filePath)
        {
            m_ShaderFilePath = filePath;

            m_Name = Razix::Utilities::GetFileName(filePath);

            // Read the *.rzsf shader file to get the necessary shader stages and it's corresponding compiled shader file virtual path
            m_ParsedRZSF = RZShader::ParseRZSF(filePath);

            // Cross compile the shaders if necessary to reflect it onto GLSL
            // TODO: Make this shit dynamic!
            CrossCompileShaders(m_ParsedRZSF, ShaderSourceType::SPIRV);

            init();
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

        void OpenGLShader::CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType)
        {
            if (srcType == ShaderSourceType::GLSL)
                return;
            else if (srcType == ShaderSourceType::SPIRV) {

            }
        }

        void OpenGLShader::init()
        {
            for (auto& source : m_ParsedRZSF) {

                // Read SPIR-V from disk or similar and store it into a 32-bit byte code
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + source.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                uint32_t* spvSource = reinterpret_cast<uint32_t*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                std::vector<uint32_t> spv(spvSource, spvSource + fileSize / sizeof(uint32_t));

                RAZIX_CORE_TRACE("Loading compiled shader : {0}", virtualPath);

                spirv_cross::CompilerGLSL glsl(std::move(spv));

                // The SPIR-V is now parsed, and we can perform reflection on it
                spirv_cross::ShaderResources resources = glsl.get_shader_resources();

                // Get the input variables info to create the buffer layout for glsl only in the vertex shader
                if (source.first == ShaderStage::VERTEX) {
                    uint32_t stride = 0;
                    for (const spirv_cross::Resource& resource : resources.stage_inputs) {
                        const spirv_cross::SPIRType& InputType = glsl.get_type(resource.type_id);

                        RAZIX_CORE_TRACE("Input variable name : {0}, type : {1}", resource.name, InputType.vecsize);
                        // Push the input vertex layout
                        pushTypeToBuffer(InputType, m_Layout, resource.name);
                    }
                }

                // reflecting the sampled images in the shader
                for (auto& resource : resources.sampled_images) {
                    // Get the shader and binding info for sampler and get the type of the sampler
                    uint32_t set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
                    uint32_t binding = glsl.get_decoration(resource.id, spv::DecorationBinding);

                    RAZIX_CORE_TRACE("Image Sampler binding : {0}, set : {1}", binding, set);

                    // Modify the decoration to prepare it for GLSL.
                    glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);

                    // Some arbitrary remapping if we want (WTF is this???)
                    glsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);

                    DescriptorLayoutBinding bindingLayout = {};
                    bindingLayout.binding   = binding;
                    bindingLayout.count     = 1;
                    bindingLayout.stage     = source.first == ShaderStage::VERTEX ? ShaderStage::VERTEX : (source.first == ShaderStage::PIXEL ? ShaderStage::PIXEL : ShaderStage::NONE);
                    bindingLayout.type      = DescriptorType::IMAGE_SAMPLER;

                    m_BindingLayouts.push_back(bindingLayout);
                }

                // Changing some parameters of the spv so that it's compatible with OpenGL glsl
                for (auto const& image : resources.separate_images) {
                    auto set{ glsl.get_decoration(image.id, spv::Decoration::DecorationDescriptorSet) };
                    glsl.set_decoration(image.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
                }
                for (auto const& input : resources.subpass_inputs) {
                    auto set{ glsl.get_decoration(input.id, spv::Decoration::DecorationDescriptorSet) };
                    glsl.set_decoration(input.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
                }

                // Reflecting the Uniform buffer data
                for (auto& uniform_buffer : resources.uniform_buffers) {
                    auto set{ glsl.get_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet) };
                    glsl.set_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);

                    uint32_t binding = glsl.get_decoration(uniform_buffer.id, spv::DecorationBinding);
                    auto& bufferType = glsl.get_type(uniform_buffer.type_id); 

                    auto bufferSize = glsl.get_declared_struct_size(bufferType);
                    int memberCount = (int) bufferType.member_types.size();

                    RAZIX_CORE_TRACE("Uniform buffer info | binding : {0}, set : {1}, bufferSize : {2}, memberCount : {3}", binding, set, bufferSize, memberCount);
                }
            }
        }

        void OpenGLShader::pushTypeToBuffer(const spirv_cross::SPIRType type, Graphics::RZVertexBufferLayout& layout, const std::string& name)
        {
            if(type.basetype == spirv_cross::SPIRType::Float) {
                switch (type.vecsize) {
                    case 1:
                        layout.push<float>(name);
                        break;
                    case 2:
                        layout.push<glm::vec2>(name);
                        break;
                    case 3:
                        layout.push<glm::vec3>(name);
                        break;
                    case 4:
                        layout.push<glm::vec4>(name);
                        break;
                }
            }
        }
    }
}
#endif 