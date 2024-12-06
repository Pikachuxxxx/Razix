// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLShader.h"

#include "Razix/Core/OS/RZFileSystem.h"
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
            setShaderFilePath(filePath);

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
        }

        void OpenGLShader::Bind() const
        {
            GL_CALL(glUseProgram(m_ProgramID));
        }

        void OpenGLShader::Unbind() const
        {
            GL_CALL(glUseProgram(0));
        }

        void OpenGLShader::CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType)
        {
            if (srcType == ShaderSourceType::GLSL)
                return;
            else if (srcType == ShaderSourceType::SPIRV) {
                // TODO: Change the decorations for push constants and UBO and set IDs and recompile into proper glsl
            }
        }

        void OpenGLShader::init()
        {
            u32 vertex_shader, pixel_shader;
            // TODO: Add support for geometry, tessellation and compute shaders
            //, geom_shader, compute_shader;
            GLint  success;
            GLchar infoLog[512];

            for (auto& source: m_ParsedRZSF) {
                // Read SPIR-V from disk or similar and store it into a 32-bit byte code
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + source.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                u32* spvSource = reinterpret_cast<u32*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                std::vector<u32> spv(spvSource, spvSource + fileSize / sizeof(u32));

                RAZIX_CORE_TRACE("Loading compiled shader : {0}", virtualPath);

                spirv_cross::CompilerGLSL glsl(std::move(spv));

                // The SPIR-V is now parsed, and we can perform reflection on it
                spirv_cross::ShaderResources resources = glsl.get_shader_resources();

                // Get the input variables info to create the buffer layout for glsl only in the vertex shader
                if (source.first == ShaderStage::Vertex) {
                    u32 stride = 0;
                    for (const spirv_cross::Resource& resource: resources.stage_inputs) {
                        const spirv_cross::SPIRType& InputType = glsl.get_type(resource.type_id);

                        RAZIX_CORE_TRACE("Input variable name : {0}, type : {1}", resource.name, InputType.vecsize);
                        // Push the input vertex layout
                        pushTypeToBuffer(InputType, m_Layout, resource.name);
                        // TODO: Add Stride
                    }
                }

                //---------------------------------------------------------------------------------------------------------------------------------------
                // reflecting the sampled images in the shader
                for (auto& resource: resources.sampled_images) {
                    // Get the shader and binding info for sampler and get the type of the sampler
                    u32 set     = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
                    u32 binding = glsl.get_decoration(resource.id, spv::DecorationBinding);

                    RAZIX_CORE_TRACE("Image Sampler binding : {0}, set : {1}", binding, set);

                    // Modify the decoration to prepare it for GLSL.
                    glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);

                    // Some arbitrary remapping if we want
                    //glsl.unset_decoration(resource.id, spv::DecorationBinding);

                    DescriptorBindingInfo bindingLayout = {};
                    bindingLayout.location.binding      = binding;
                    bindingLayout.count                 = 1;
                    bindingLayout.stage                 = source.first == ShaderStage::Vertex ? ShaderStage::Vertex : (source.first == ShaderStage::Pixel ? ShaderStage::Pixel : ShaderStage::NONE);
                    bindingLayout.type                  = DescriptorType::ImageSamplerCombined;

                    RZDescriptor rzDescriptor;
                    rzDescriptor.bindingInfo = bindingLayout;
                    rzDescriptor.name        = resource.name;

                    // Find the set first and then it's descriptors vector to append to
                    auto& descriptors_in_set = m_DescriptorsPerHeap[set];
                    descriptors_in_set.push_back(rzDescriptor);
                }

                // Changing some parameters of the spv so that it's compatible with OpenGL glsl
                for (auto const& image: resources.separate_images) {
                    auto set{glsl.get_decoration(image.id, spv::Decoration::DecorationDescriptorSet)};
                    glsl.set_decoration(image.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
                }
                for (auto const& input: resources.subpass_inputs) {
                    auto set{glsl.get_decoration(input.id, spv::Decoration::DecorationDescriptorSet)};
                    glsl.set_decoration(input.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
                }

                //---------------------------------------------------------------------------------------------------------------------------------------
                // Reflecting the Uniform buffer data
                for (auto& uniform_buffer: resources.uniform_buffers) {
                    auto set{glsl.get_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet)};
                    glsl.set_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);

                    u32   binding    = glsl.get_decoration(uniform_buffer.id, spv::DecorationBinding);
                    auto& bufferType = glsl.get_type(uniform_buffer.type_id);

                    auto bufferSize  = glsl.get_declared_struct_size(bufferType);
                    u32  memberCount = (u32) bufferType.member_types.size();

                    DescriptorBindingInfo bindingLayout = {};
                    bindingLayout.location.binding               = binding;
                    bindingLayout.count                 = 1;
                    bindingLayout.stage                 = source.first == ShaderStage::Vertex ? ShaderStage::Vertex : (source.first == ShaderStage::Pixel ? ShaderStage::Pixel : ShaderStage::NONE);
                    bindingLayout.type                  = DescriptorType::UniformBuffer;

                    RAZIX_CORE_WARN("id : {0}, type_id {1}, base_type_id : {2}, name : {3}", glsl.get_name(uniform_buffer.id), glsl.get_name(uniform_buffer.type_id), glsl.get_name(uniform_buffer.base_type_id), uniform_buffer.name);

                    glsl.set_decoration(uniform_buffer.id, spv::Decoration::DecorationBinding, binding + set);

                    RZDescriptor rzDescriptor;
                    rzDescriptor.bindingInfo = bindingLayout;
                    rzDescriptor.typeName    = glsl.get_name(uniform_buffer.base_type_id);
                    rzDescriptor.name        = glsl.get_name(uniform_buffer.id);
                    rzDescriptor.offset      = 0;    // TODO: Research on how to extract this info, although 0 should work for most cases
                    rzDescriptor.size        = static_cast<u32>(bufferSize);

                    RAZIX_CORE_TRACE("Uniform buffer info | binding : {0}, set : {1}, bufferSize : {2}, memberCount : {3}, name : {4}", binding, set, bufferSize, memberCount, glsl.get_name(uniform_buffer.id));

                    // Get the member info
                    for (u32 i = 0; i < memberCount; i++) {
                        auto& type       = glsl.get_type(bufferType.member_types[i]);
                        auto& memberName = glsl.get_member_name(bufferType.self, i);
                        auto  size       = glsl.get_declared_struct_member_size(bufferType, i);
                        auto  offset     = glsl.type_struct_member_offset(bufferType, i);

                        std::string uniformName = glsl.get_name(uniform_buffer.id) + "." + memberName;

                        RZShaderBufferMemberInfo memberInfo;

                        memberInfo.size   = (u32) size;
                        memberInfo.offset = offset;
                        //memberInfo.type     = spirvtype(type); // TODO: Add utility function for conversion
                        memberInfo.fullName = uniformName;
                        memberInfo.name     = memberName;

                        rzDescriptor.uboMembers.push_back(memberInfo);

                        RAZIX_CORE_TRACE("\t UBO member Info | name : {0}, offset : {1}, size : {2}", uniformName, offset, size);
                    }

                    // Find the set first and then it's descriptors vector to append to
                    auto& descriptors_in_set = m_DescriptorsPerHeap[set];
                    descriptors_in_set.push_back(rzDescriptor);
                }

                //---------------------------------------------------------------------------------------------------------------------------------------
                // Push constants
                for (auto& push_constant: resources.push_constant_buffers) {
                    u32 set     = glsl.get_decoration(push_constant.id, spv::DecorationDescriptorSet);
                    u32 binding = glsl.get_decoration(push_constant.id, spv::DecorationBinding);

                    auto& type = glsl.get_type(push_constant.type_id);
                    auto& name = glsl.get_name(push_constant.id);

                    auto ranges = glsl.get_active_buffer_ranges(push_constant.id);

                    u32 size = 0;
                    for (auto& range: ranges) {
                        size += u32(range.range);
                    }

                    auto& bufferType  = glsl.get_type(push_constant.base_type_id);
                    auto  bufferSize  = glsl.get_declared_struct_size(bufferType);
                    int   memberCount = (int) bufferType.member_types.size();

                    //glsl.unset_decoration(u.id, spv::DecorationBinding);
                    // FIXME: This isn't working
                    glsl.set_decoration(push_constant.id, spv::Decoration::DecorationBinding, binding + set);
                    //glsl.set_decoration(u.id, spv::Decort)
                    //m_PushConstants.push_back({size, file.first});
                    //m_PushConstants.back().data = new u8[size];

                    DescriptorBindingInfo bindingLayout = {};
                    bindingLayout.location.binding               = binding;
                    bindingLayout.count                 = 1;
                    bindingLayout.stage                 = source.first == ShaderStage::Vertex ? ShaderStage::Vertex : (source.first == ShaderStage::Pixel ? ShaderStage::Pixel : ShaderStage::NONE);
                    bindingLayout.type                  = DescriptorType::UniformBuffer;

                    RAZIX_CORE_WARN("id : {0}, type_id {1}, base_type_id : {2}, name : {3}", glsl.get_name(push_constant.id), glsl.get_name(push_constant.type_id), glsl.get_name(push_constant.base_type_id), push_constant.name);

                    //RZDescriptor rzDescriptor;
                    //rzDescriptor.bindingInfo = bindingLayout;
                    //rzDescriptor.typeName    = glsl.get_name(push_constant.base_type_id);
                    //rzDescriptor.name        = glsl.get_name(push_constant.id);
                    //rzDescriptor.offset      = 0;    // TODO: Research on how to extract this info, although 0 should work for most cases
                    //rzDescriptor.size        = bufferSize;

                    RZPushConstant pc{};
                    pc.typeName    = glsl.get_name(push_constant.base_type_id);
                    pc.name        = glsl.get_name(push_constant.id);
                    pc.shaderStage = bindingLayout.stage;
                    pc.data        = nullptr;
                    pc.size        = static_cast<u32>(bufferSize);
                    pc.offset      = 0;    // TODO: Research on how to extract this info, although 0 should work for most cases
                    pc.bindingInfo = bindingLayout;

                    for (int i = 0; i < memberCount; i++) {
                        auto& type       = glsl.get_type(bufferType.member_types[i]);
                        auto& memberName = glsl.get_member_name(bufferType.self, i);
                        auto  size       = glsl.get_declared_struct_member_size(bufferType, i);
                        auto  offset     = glsl.type_struct_member_offset(bufferType, i);

                        std::string uniformName = push_constant.name + "." + memberName;

                        RZShaderBufferMemberInfo memberInfo;

                        memberInfo.size   = (u32) size;
                        memberInfo.offset = offset;
                        // TODO: Add utility function for conversion
                        //memberInfo.type     = spirvtype(type);
                        memberInfo.fullName = uniformName;
                        memberInfo.name     = memberName;

                        pc.structMembers.push_back(memberInfo);

                        RAZIX_CORE_TRACE("\t PushConstant member Info | name : {0}, offset : {1}, size : {2}", uniformName, offset, size);
                    }

                    m_PushConstants.push_back(pc);

                    // Find the set first and then it's descriptors vector to append to
                    //auto& descriptors_in_set = m_DescriptorSetsCreateInfos[set];
                    //descriptors_in_set.push_back(rzDescriptor);
                }
                //---------------------------------------------------------------------------------------------------------------------------------------
                spirv_cross::CompilerGLSL::Options options;
                options.version                               = 440;
                options.es                                    = false;
                options.vulkan_semantics                      = false;
                options.separate_shader_objects               = false;
                options.enable_420pack_extension              = false;
                options.emit_push_constant_as_uniform_buffer  = true;
                options.emit_uniform_buffer_as_plain_uniforms = false;
                glsl.set_common_options(options);

                // Compile to GLSL, ready to give to GL driver.
                std::string glslSource = glsl.compile();
                RAZIX_CORE_TRACE("//---------------------------------------------------------------------------------------------------------------------------------------//");
                RAZIX_CORE_TRACE("SPIRV-->GLSL (non-vulkan) Source ==> {0} : {1} \n \t", source.first, glslSource);
                RAZIX_CORE_TRACE("//---------------------------------------------------------------------------------------------------------------------------------------//");

                // Now compile this
                if (source.first == ShaderStage::Vertex) {
                    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

                    const GLchar* ShaderCode = glslSource.c_str();
                    glShaderSource(vertex_shader, 1, &ShaderCode, NULL);
                    glCompileShader(vertex_shader);
                    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

                    if (!success) {
                        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);

                        RAZIX_CORE_ERROR("[OPENGL] [Shader Compilation Failed :: VEXTEX_SHADER] Info : {0} ", infoLog);
                    }

                } else if (source.first == ShaderStage::Pixel) {
                    pixel_shader = glCreateShader(GL_FRAGMENT_SHADER);

                    const GLchar* ShaderCode = glslSource.c_str();
                    glShaderSource(pixel_shader, 1, &ShaderCode, NULL);
                    glCompileShader(pixel_shader);
                    glGetShaderiv(pixel_shader, GL_COMPILE_STATUS, &success);

                    if (!success) {
                        glGetShaderInfoLog(pixel_shader, 512, NULL, infoLog);

                        RAZIX_CORE_ERROR("[OPENGL] [Shader Compilation Failed :: PIXEL_SHADER] Info : {0} ", infoLog);
                    }
                }
            }

            // Link the vertex and pixel shaders into a single shader program
            m_ProgramID = glCreateProgram();
            glAttachShader(m_ProgramID, vertex_shader);
            glAttachShader(m_ProgramID, pixel_shader);
            glLinkProgram(m_ProgramID);

            glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
                RAZIX_CORE_ERROR("[OPENGL] [Shader Program LINKING_FAILED] Info : {0}", infoLog);
            }

            // Delete the shaders as they're linked into our program now and no longer necessary
            glDeleteShader(vertex_shader);
            glDeleteShader(pixel_shader);

            // Add bindings to shader
        }

        void OpenGLShader::pushTypeToBuffer(const spirv_cross::SPIRType type, Graphics::RZVertexBufferLayout& layout, const std::string& name)
        {
            if (type.basetype == spirv_cross::SPIRType::Float) {
                switch (type.vecsize) {
                    case 1:
                        layout.push<f32>(name);
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

        void OpenGLShader::DestroyResource()
        {
            GL_CALL(glDeleteProgram(m_ProgramID));
        }

        void OpenGLShader::GenerateDescriptorHeaps()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }    // namespace Graphics
}    // namespace Razix
#endif