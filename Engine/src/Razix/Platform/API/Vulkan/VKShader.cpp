// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKShader.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <SPIRVReflect/common/output_stream.h>
#include <spirv_reflect.h>

#include <glm/glm.hpp>

#include <imgui/imgui.h>

#include "internal/RazixMemory/include/RZMemoryFunctions.h"

namespace Razix {
    namespace Graphics {

        static u32 GetStrideFromVulkanFormat(VkFormat format)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (format) {
                case VK_FORMAT_R8_SINT:
                    return sizeof(int);
                case VK_FORMAT_R32_SFLOAT:
                    return sizeof(FLOAT);
                case VK_FORMAT_R32G32_SFLOAT:
                    return sizeof(glm::vec2);
                case VK_FORMAT_R32G32B32_SFLOAT:
                    return sizeof(glm::vec3);
                case VK_FORMAT_R32G32B32A32_SFLOAT:
                    return sizeof(glm::vec4);
                case VK_FORMAT_R32G32_SINT:
                    return sizeof(glm::ivec2);
                case VK_FORMAT_R32G32B32_SINT:
                    return sizeof(glm::ivec3);
                case VK_FORMAT_R32G32B32A32_SINT:
                    return sizeof(glm::ivec4);
                case VK_FORMAT_R32G32_UINT:
                    return sizeof(glm::uvec2);
                case VK_FORMAT_R32G32B32_UINT:
                    return sizeof(glm::uvec3);
                case VK_FORMAT_R32G32B32A32_UINT:
                    return sizeof(glm::uvec4);    //Need uintvec?
                case VK_FORMAT_R32_UINT:
                    return sizeof(unsigned int);
                default:
                    RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                    return 0;
            }

            return 0;
        }

        static u32 pushBufferLayout(VkFormat format, const std::string& name, RZVertexBufferLayout& layout)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: Add buffer layout for all supported types
            switch (format) {
                case VK_FORMAT_R8_SINT:
                    layout.push<int>(name);
                    break;
                case VK_FORMAT_R32_SFLOAT:
                    layout.push<FLOAT>(name);
                    break;
                case VK_FORMAT_R32G32_SFLOAT:
                    layout.push<glm::vec2>(name);
                    break;
                case VK_FORMAT_R32G32B32_SFLOAT:
                    layout.push<glm::vec3>(name);
                    break;
                case VK_FORMAT_R32G32B32A32_SFLOAT:
                    layout.push<glm::vec4>(name);
                    break;
                default:
                    RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                    return 0;
            }

            return 0;
        }

        static DescriptorType VKToEngineDescriptorType(SpvReflectDescriptorType type)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (type) {
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    return DescriptorType::ImageSamplerCombined;
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    return DescriptorType::UniformBuffer;
                    break;
            }

            // FIXME: Make this return something like NONE and cause a ASSERT_ERROR
            return DescriptorType::UniformBuffer;
        }

        VKShader::VKShader(const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            setShaderFilePath(filePath);

            m_Name = Razix::Utilities::GetFileName(filePath);

            // Read the *.rzsf shader file to get the necessary shader stages and it's corresponding compiled shader file virtual path
            m_ParsedRZSF = RZShader::ParseRZSF(filePath);

            // Cross compile the shaders if necessary to reflect it onto GLSL
            // TODO: Make this shit dynamic!
            CrossCompileShaders(m_ParsedRZSF, ShaderSourceType::SPIRV);

            // Reflect the shaders using SPIR-V Reflect to extract the necessary information about descriptors and inputs to the shaders
            reflectShader();

            // Create the shader modules and the pipeline shader stage create infos that will be bound to the pipeline
            createShaderModules();

            for (const auto& spvSource: m_ParsedRZSF) {
                VK_TAG_OBJECT(bufferName, VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t) m_ShaderCreateInfos[spvSource.first].module);
            }
        }

        VKShader::~VKShader()
        {
        }

        void VKShader::DestroyResource()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the pipeline layout
            vkDestroyPipelineLayout(VKDevice::Get().getDevice(), m_PipelineLayout, nullptr);

            // Destroy the shader modules
            for (const auto& spvSource: m_ParsedRZSF)
                vkDestroyShaderModule(VKDevice::Get().getDevice(), m_ShaderCreateInfos[spvSource.first].module, nullptr);
        }

        void VKShader::Bind() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

        void VKShader::Unbind() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
        }

        void VKShader::CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (srcType != ShaderSourceType::SPIRV)
                return;
        }

        void VKShader::GenerateDescriptorHeaps()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy them and then clear them
            for (size_t i = 0; i < m_SceneParams.userSets.size(); i++)
                m_SceneParams.userSets[i]->Destroy();
            m_SceneParams.userSets.clear();

            // Create the Descriptor Sets for the Shader
            // We skip if they're system sets like FrameData, SceneLightsData and Material which are managed by RHI and Scene
            for (auto& setInfo: m_DescriptorsPerHeap) {
                // Skip creating some sets!
                // We skip the system sets such as FrameData, SceneLightsData and Material Data
                auto& descriptor = setInfo.second[0];

                if (descriptor.name == "FrameData") {
                    m_SceneParams.enableFrameData = true;
                    continue;

                } else if (descriptor.name == "Material") {
                    m_SceneParams.enableMaterials = true;
                    continue;

                } else if (descriptor.name == "SceneLightsData") {
                    m_SceneParams.enableLights = true;
                    continue;
                }
                // TODO: Add support for m_SceneParams.enableBindless
                m_SceneParams.userSets.push_back(RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG(getShaderFilePath())));

                // Add all descriptors of user sets into a named map
                for (auto& descr: setInfo.second)
                    m_BindVars.m_BindMap.insert({descr.name, &descr});
            }
        }

        std::vector<VkPipelineShaderStageCreateInfo> VKShader::getShaderStages()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
            for (std::map<ShaderStage, VkPipelineShaderStageCreateInfo>::iterator it = m_ShaderCreateInfos.begin(); it != m_ShaderCreateInfos.end(); ++it) {
                shaderStages.push_back(it->second);
            }
            return shaderStages;
        }

        void VKShader::reflectShader()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            bool potentiallyBindless = false;

            // Reflect the SPIR-V shader to extract all the necessary information
            for (const auto& spvSource: m_ParsedRZSF) {
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                const void* spvByteCode = reinterpret_cast<void*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                // Generate reflection data for a shader
                SpvReflectShaderModule module;
                SpvReflectResult       result = spvReflectCreateShaderModule(fileSize, spvByteCode, &module);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV shader - ({0})!", virtualPath);

                delete spvByteCode;

                // SPIRV-Reflect uses similar API to Vulkan, first enumerate for the count and then create the container and the use the same functions to fill the container with the enumerated data
                // Also SPIRV-Reflect API enums use same integer codes as Vulkan hence they can be type casted safely and they will work fine, this is guaranteed by the API hence we need not do extra conversions

                // Enumerate and extract shaders input variables
                //u32 var_count = 0;
                //result             = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
                //RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV Input variables - ({0})!", virtualPath);
                //SpvReflectInterfaceVariable** input_vars = (SpvReflectInterfaceVariable**) Razix::Memory::RZMalloc(var_count * sizeof(SpvReflectInterfaceVariable));
                //result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);

                // Vertex Input attributes
                if (spvSource.first == ShaderStage::Vertex) {
                    m_VertexInputStride = 0;

                    //std::cout << "---------------------------------------------" << std::endl;
                    for (sz i = 0; i < module.input_variable_count; i++) {
                        SpvReflectInterfaceVariable inputVar = *module.input_variables[i];

                        if (std::string(inputVar.name) == "gl_VertexIndex" || inputVar.name == "vs_out")
                            break;

                        // Fill the vulkan input variables attribute information
                        VkVertexInputAttributeDescription verextInputattribDesc = {};
                        verextInputattribDesc.binding                           = 0;
                        verextInputattribDesc.location                          = inputVar.location;
                        verextInputattribDesc.format                            = (VkFormat) inputVar.format;
                        verextInputattribDesc.offset                            = m_VertexInputStride;

                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc);

                        m_VertexInputStride += GetStrideFromVulkanFormat((VkFormat) inputVar.format);

                        // Create the buffer layout for Razix engine
                        pushBufferLayout((VkFormat) inputVar.format, inputVar.name, m_BufferLayout);

                        //delete inputVar;
                    }

                    // FIXME: Make this intuitive and don't hard code it
                    // Specializing vertex format for ImGui shaders
                    if (spvSource.second == "Compiled/SPIRV/Shader.Builtin.ImGui.vert.spv" || m_ShaderLibraryID == ShaderBuiltin::ImGui) {
                        struct ImDrawVert
                        {
                            ImVec2 pos;
                            ImVec2 uv;
                            ImU32  col;
                        };

                        m_VertexInputAttributeDescriptions.clear();
                        VkVertexInputAttributeDescription verextInputattribDesc1 = {};
                        verextInputattribDesc1.binding                           = 0;
                        verextInputattribDesc1.location                          = 0;
                        verextInputattribDesc1.format                            = VK_FORMAT_R32G32_SFLOAT;
                        verextInputattribDesc1.offset                            = offsetof(ImDrawVert, pos);

                        VkVertexInputAttributeDescription verextInputattribDesc2 = {};
                        verextInputattribDesc2.binding                           = 0;
                        verextInputattribDesc2.location                          = 1;
                        verextInputattribDesc2.format                            = VK_FORMAT_R32G32_SFLOAT;
                        verextInputattribDesc2.offset                            = offsetof(ImDrawVert, uv);

                        VkVertexInputAttributeDescription verextInputattribDesc3 = {};
                        verextInputattribDesc3.binding                           = 0;
                        verextInputattribDesc3.location                          = 2;
                        verextInputattribDesc3.format                            = VK_FORMAT_R8G8B8A8_UNORM;
                        verextInputattribDesc3.offset                            = offsetof(ImDrawVert, col);

                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc1);
                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc2);
                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc3);

                        m_VertexInputStride = 20;
                    }
                }
                //delete[] input_vars;
                //Razix::Memory::RZFree(input_vars);

                // Uniform Buffers + Samplers
                // Descriptor Bindings : These bindings describe where and what kind of resources are bound to the shaders at various stages, they also store the information of the nature of resource data that is bound
                // Uniform variables usually have members and samplers none
                //u32 descriptors_count = 0;
                //result                     = spvReflectEnumerateDescriptorBindings(&module, &descriptors_count, nullptr);
                //RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect descriptor bindings from SPIRV shader - ({0})", virtualPath);
                //SpvReflectDescriptorBinding* pp_descriptor_bindings = new SpvReflectDescriptorBinding[descriptors_count];
                ////(SpvReflectDescriptorBinding**) Razix::Memory::RZMalloc(var_count * sizeof(SpvReflectDescriptorBinding*));    //malloc(var_count * sizeof(SpvReflectDescriptorBinding*));
                //result                                               = spvReflectEnumerateDescriptorBindings(&module, &descriptors_count, &pp_descriptor_bindings);

                for (u32 i = 0; i < module.descriptor_binding_count; i++) {
                    SpvReflectDescriptorBinding descriptor = module.descriptor_bindings[i];

                    RZDescriptor* rzDescriptor = new RZDescriptor;

                    // First create the descriptor layout bindings, these describe where and what kind of resources are being bound to the shader per descriptor set
                    // Which means each descriptor set (i.e. for a given set ID) it stores a list of binding layouts in a map
                    VkDescriptorSetLayoutBinding setLayoutBindingInfo = {};
                    setLayoutBindingInfo.binding                      = descriptor.binding;
                    setLayoutBindingInfo.descriptorCount              = descriptor.count;    // descriptorCount is the number of descriptors contained in the binding, accessed in a shader as an array, if any (useful for Animation aka JointTransforms)
                    setLayoutBindingInfo.descriptorType               = (VkDescriptorType) descriptor.descriptor_type;
                    setLayoutBindingInfo.stageFlags                   = VKUtilities::ShaderStageToVK(spvSource.first);

                    auto& layou_bindings_in_set = m_VKSetBindingLayouts[descriptor.set];
                    layou_bindings_in_set.push_back(std::move(setLayoutBindingInfo));

                    // -->Also store all this data for the engine as well.
                    DescriptorBindingInfo bindingInfo{};
                    bindingInfo.location.binding = descriptor.binding;
                    bindingInfo.location.set     = descriptor.set;
                    bindingInfo.count            = descriptor.count;
                    bindingInfo.type             = VKToEngineDescriptorType(descriptor.descriptor_type);
                    bindingInfo.stage            = spvSource.first;

                    if (descriptor.count >= 1024)
                        potentiallyBindless = true;

                    rzDescriptor->bindingInfo = bindingInfo;
                    rzDescriptor->name        = descriptor.name;
                    rzDescriptor->offset      = descriptor.block.offset;
                    rzDescriptor->size        = descriptor.block.size;

                    for (sz i = 0; i < descriptor.block.member_count; i++) {
                        RZShaderBufferMemberInfo memberInfo{};
                        memberInfo.fullName = rzDescriptor->name + "." + descriptor.block.members[i].name;
                        memberInfo.name     = descriptor.block.members[i].name;
                        memberInfo.offset   = descriptor.block.members[i].offset;
                        memberInfo.size     = descriptor.block.members[i].size;

                        rzDescriptor->uboMembers.push_back(std::move(memberInfo));
                    }

                    auto& descriptors_in_set = m_DescriptorsPerHeap[descriptor.set];
                    descriptors_in_set.push_back(std::move(*rzDescriptor));

                    delete rzDescriptor;
                }
                //delete[] pp_descriptor_bindings;
                // FIXME: Investigate and delete this properly
                //Razix::Memory::RZFree(pp_descriptor_bindings);

                // Get info about push constants
                //u32 push_constants_count = 0;
                //spvReflectEnumeratePushConstantBlocks(&module, &push_constants_count, nullptr);
                //RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect push constants from shader - ({0})", virtualPath);
                //SpvReflectBlockVariable* pp_push_constant_blocks = new SpvReflectBlockVariable[push_constants_count];
                ////(SpvReflectBlockVariable**) Razix::Memory::RZMalloc(var_count * sizeof(SpvReflectBlockVariable*));
                //spvReflectEnumeratePushConstantBlocks(&module, &push_constants_count, &pp_push_constant_blocks);

                // Create Push constants and store info about it
                for (u32 i = 0; i < module.push_constant_block_count; i++) {
                    SpvReflectBlockVariable* pushConstant = &module.push_constant_blocks[i];
                    //std::cout << "Name      : " << pushConstant->name << std::endl;
                    //std::cout << "Size      : " << pushConstant->size << std::endl;
                    //std::cout << "Offset    : " << pushConstant->offset << std::endl;
                    //RAZIX_CORE_TRACE("Push contant name : {0}", pushConstant->name);

                    RZPushConstant pc{};
                    pc.name                         = pushConstant->name;
                    pc.shaderStage                  = spvSource.first;
                    pc.data                         = nullptr;
                    pc.size                         = pushConstant->size;
                    pc.offset                       = pushConstant->offset;
                    pc.bindingInfo.location.binding = 0;    // Doesn't make sense for PushConstants
                    pc.bindingInfo.location.set     = 0;    // Doesn't make sense for PushConstants
                    pc.bindingInfo.stage            = spvSource.first;
                    pc.bindingInfo.count            = 1;
                    pc.bindingInfo.type             = DescriptorType::UniformBuffer;
                    for (sz i = 0; i < pushConstant->member_count; i++) {
                        auto                     member = pushConstant->members[i];
                        RZShaderBufferMemberInfo mem{};
                        mem.fullName = pc.name + "." + member.name;
                        mem.name     = member.name;
                        mem.offset   = member.offset;
                        mem.size     = member.size;
                        pc.structMembers.push_back(std::move(mem));
                    }
                    m_PushConstants.push_back(std::move(pc));
                }
                //delete[] pp_push_constant_blocks;
                //Razix::Memory::RZFree(pp_push_constant_blocks);
                // Destroy the reflection data when no longer required
                // FIXME: This is causing unnecessary crashes, investigate and resolve!
                spvReflectDestroyShaderModule(&module);
            }

            // Create the Vulkan set layouts for each set ID with the descriptors (bindings) it has
            // Set layout describe how a particular descriptor and it's bindings are layed out
            for (const auto& setLayouts: m_VKSetBindingLayouts) {
                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = static_cast<u32>(setLayouts.second.size());
                layoutInfo.pBindings    = setLayouts.second.data();

                VkDescriptorSetLayout& setLayout = m_PerSetLayouts[setLayouts.first];
                if (VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VKDevice::Get().getDevice(), &layoutInfo, nullptr, &setLayout)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor set layout!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Successfully created descriptor set layout");

                VK_TAG_OBJECT(m_Name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t) setLayout)
            }

            std::vector<VkDescriptorSetLayout> descriptorLayouts;
            for (std::map<u32, VkDescriptorSetLayout>::iterator it = m_PerSetLayouts.begin(); it != m_PerSetLayouts.end(); ++it) {
                descriptorLayouts.push_back(it->second);
            }

            // Create Push constants for vulkan
            for (auto& pushConst: m_PushConstants) {
                VkPushConstantRange pushConstantRange{};
                pushConstantRange.stageFlags = VKUtilities::ShaderStageToVK(pushConst.shaderStage);
                pushConstantRange.offset     = pushConst.offset;
                pushConstantRange.size       = pushConst.size;
                m_VKPushConstants.push_back(pushConstantRange);
            }

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount             = static_cast<u32>(descriptorLayouts.size());
            pipelineLayoutCreateInfo.pSetLayouts                = descriptorLayouts.data();
            pipelineLayoutCreateInfo.pushConstantRangeCount     = u32(m_VKPushConstants.size());
            pipelineLayoutCreateInfo.pPushConstantRanges        = m_VKPushConstants.data();

            if (VK_CHECK_RESULT(vkCreatePipelineLayout(VKDevice::Get().getDevice(), &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_PipelineLayout)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create pipeline layout!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created pipeline layout!");

            VK_TAG_OBJECT(m_Name, VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t) m_PipelineLayout)

            // Replace the Set a index BindingTable_System::SET_IDX_BINDLESS_RESOURCES_START with the bindless set layout
            if (potentiallyBindless) {
                m_PerSetLayouts[BindingTable_System::SET_IDX_BINDLESS_RESOURCES_START] = VKDevice::Get().getBindlessSetLayout();

                std::vector<VkDescriptorSetLayout> descriptorLayoutsBindless;
                for (std::map<u32, VkDescriptorSetLayout>::iterator it = m_PerSetLayouts.begin(); it != m_PerSetLayouts.end(); ++it) {
                    descriptorLayoutsBindless.push_back(it->second);
                }

                pipelineLayoutCreateInfo                        = {};
                pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutCreateInfo.setLayoutCount         = static_cast<u32>(descriptorLayoutsBindless.size());
                pipelineLayoutCreateInfo.pSetLayouts            = descriptorLayoutsBindless.data();
                pipelineLayoutCreateInfo.pushConstantRangeCount = u32(m_VKPushConstants.size());
                pipelineLayoutCreateInfo.pPushConstantRanges    = m_VKPushConstants.data();

                if (VK_CHECK_RESULT(vkCreatePipelineLayout(VKDevice::Get().getDevice(), &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_PipelineLayout)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create bindless pipeline layout!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Successfully created bindless pipeline layout!");
            }

            for (sz i = 0; i < descriptorLayouts.size(); i++)
                vkDestroyDescriptorSetLayout(VKDevice::Get().getDevice(), descriptorLayouts[i], nullptr);

            // Generate the Descriptor Sets
            GenerateDescriptorHeaps();
        }

        void VKShader::createShaderModules()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (const auto& spvSource: m_ParsedRZSF) {
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                const u32* spvByteCode = reinterpret_cast<u32*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                VkShaderModuleCreateInfo shaderModuleCI = {};
                shaderModuleCI.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                shaderModuleCI.codeSize                 = fileSize;
                shaderModuleCI.pCode                    = spvByteCode;

                m_ShaderCreateInfos[spvSource.first].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_ShaderCreateInfos[spvSource.first].pName = "main";    // TODO: Extract this from shader later
                m_ShaderCreateInfos[spvSource.first].stage = VKUtilities::ShaderStageToVK(spvSource.first);

                if (VK_CHECK_RESULT(vkCreateShaderModule(VKDevice::Get().getDevice(), &shaderModuleCI, nullptr, &m_ShaderCreateInfos[spvSource.first].module)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create shader module!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Successfully created shader module");

                delete spvByteCode;
            }
        }
    }    // namespace Graphics
}    // namespace Razix