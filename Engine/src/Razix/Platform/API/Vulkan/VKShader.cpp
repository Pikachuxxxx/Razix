#include "rzxpch.h"
#include "VKShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <SPIRVReflect/common/output_stream.h>
#include <glm/glm.hpp>

#include <imgui/imgui.h>

namespace Razix {
    namespace Graphics {

        static uint32_t GetStrideFromVulkanFormat(VkFormat format)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (format) {
                case VK_FORMAT_R8_SINT:
                    return sizeof(int);
                case VK_FORMAT_R32_SFLOAT:
                    return sizeof(float);
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
                    return sizeof(glm::uvec4); //Need uintvec?
                case  VK_FORMAT_R32_UINT:
                    return sizeof(unsigned int);
                default:
                    RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                    return 0;
            }

            return 0;
        }


        static uint32_t pushBufferLayout(VkFormat format, const std::string& name, RZVertexBufferLayout& layout)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: Add buffer layout for all supported types
            switch (format) {
                case VK_FORMAT_R8_SINT:
                    layout.push<int>(name);
                    break;
                case VK_FORMAT_R32_SFLOAT:
                    layout.push<float>(name);
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
                    return DescriptorType::IMAGE_SAMPLER;
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    return DescriptorType::UNIFORM_BUFFER;
                    break;
            }
        }

        VKShader::VKShader(const std::string& filePath)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_ShaderFilePath = filePath;

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
        }

        VKShader::~VKShader() { }

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

        void VKShader::Destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Destroy the pipeline layout
            vkDestroyPipelineLayout(VKDevice::Get().getDevice(), m_PipelineLayout, nullptr);

            // Destroy the shader modules
            for (const auto& spvSource : m_ParsedRZSF)
                vkDestroyShaderModule(VKDevice::Get().getDevice(), m_ShaderCreateInfos[spvSource.first].module, nullptr);
        }

        std::vector<VkPipelineShaderStageCreateInfo> VKShader::getShaderStages()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
            for (std::unordered_map<ShaderStage, VkPipelineShaderStageCreateInfo>::iterator it = m_ShaderCreateInfos.begin(); it != m_ShaderCreateInfos.end(); ++it) {
                shaderStages.push_back(it->second);
            }
            return shaderStages;
        }

        void VKShader::reflectShader()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Reflect the SPIR-V shader to extract all the necessary information
            for (const auto& spvSource : m_ParsedRZSF) {

                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                const void* spvByteCode= reinterpret_cast<void*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                // Generate reflection data for a shader
                SpvReflectShaderModule module;
                SpvReflectResult result = spvReflectCreateShaderModule(fileSize, spvByteCode, &module);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV shader - ({0})!", virtualPath);

                // SPIRV-Reflect uses similar API to Vulkan, first enumerate for the count and then create the container and the use the same functions to fill the container with the enumerated data
                // Also SPIRV-Reflect API enums use same integer codes as Vulkan hence they can be type casted safely and they will work fine, this is guaranteed by the API hence we need not do extra conversions

                // Enumerate and extract shaders input variables
                uint32_t var_count = 0;
                result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV Input variables - ({0})!", virtualPath);
                SpvReflectInterfaceVariable** input_vars = (SpvReflectInterfaceVariable**) malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
                result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);

                // Vertex Input attributes
                if (spvSource.first == ShaderStage::VERTEX) {

                    m_VertexInputStride = 0;

                    //std::cout << "---------------------------------------------" << std::endl;
                    for (size_t i = 0; i < var_count; i++) {
                        SpvReflectInterfaceVariable* inputVar = input_vars[i];

                        if (std::string(inputVar->name) == "gl_VertexIndex")
                            break;

                        // Fill the vulkan input variables attribute information
                        VkVertexInputAttributeDescription verextInputattribDesc = {};
                        verextInputattribDesc.binding   = 0;
                        verextInputattribDesc.location  = inputVar->location;
                        verextInputattribDesc.format    = (VkFormat) inputVar->format;
                        verextInputattribDesc.offset    = m_VertexInputStride;

                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc);

                        m_VertexInputStride += GetStrideFromVulkanFormat((VkFormat) inputVar->format);

                        // Create the buffer layout for Razix engine
                        pushBufferLayout((VkFormat) inputVar->format, inputVar->name, m_BufferLayout);
                    }

                    if (spvSource.second == "Compiled/SPIRV/imgui.vert.spv") {

                        struct ImDrawVert
                        {
                            ImVec2  pos;
                            ImVec2  uv;
                            ImU32   col;
                        };

                        m_VertexInputAttributeDescriptions.clear();
                        VkVertexInputAttributeDescription verextInputattribDesc1 = {};
                        verextInputattribDesc1.binding = 0;
                        verextInputattribDesc1.location = 0;
                        verextInputattribDesc1.format = VK_FORMAT_R32G32_SFLOAT;
                        verextInputattribDesc1.offset = offsetof(ImDrawVert, pos);

                        VkVertexInputAttributeDescription verextInputattribDesc2 = {};
                        verextInputattribDesc2.binding = 0;
                        verextInputattribDesc2.location = 1;
                        verextInputattribDesc2.format = VK_FORMAT_R32G32_SFLOAT;
                        verextInputattribDesc2.offset = offsetof(ImDrawVert, uv);

                        VkVertexInputAttributeDescription verextInputattribDesc3 = {};
                        verextInputattribDesc3.binding = 0;
                        verextInputattribDesc3.location = 2;
                        verextInputattribDesc3.format = VK_FORMAT_R8G8B8A8_UNORM;
                        verextInputattribDesc3.offset = offsetof(ImDrawVert, col);


                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc1);
                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc2);
                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc3);

                        m_VertexInputStride = 20;
                    }
                }

                // Uniform Buffers + Samplers
                // Descriptor Bindings : These bindings describe where and what kind of resources are bound to the shaders at various stages, they also store the information of the nature of resource data that is bound
                // Uniform variables usually have members and samplers none
                // TODO: Use spvReflectEnumerateDescriptorSets instead to fill in the vulkan descriptor sets quickly
                uint32_t descriptors_count = 0;
                result = spvReflectEnumerateDescriptorBindings(&module, &descriptors_count, nullptr);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect descriptor bindings from SPIRV shader - ({0})", virtualPath);
                SpvReflectDescriptorBinding** pp_descriptor_bindings = (SpvReflectDescriptorBinding**) malloc(var_count * sizeof(SpvReflectDescriptorBinding*));
                result = spvReflectEnumerateDescriptorBindings(&module, &descriptors_count, pp_descriptor_bindings);

                for (uint32_t i = 0; i < descriptors_count; i++) {

                    std::cout << "---------------------------------------------" << std::endl;
                    DescriptorSetInfo* setInfo = new DescriptorSetInfo;
                    setInfo->setID = -1;
                    bool oldSet = false;

                    SpvReflectDescriptorBinding* descriptor = pp_descriptor_bindings[i];

                    for (size_t i = 0; i < m_DescriptorSetInfos.size(); i++) {
                        if (m_DescriptorSetInfos[i].setID == descriptor->set) {
                            setInfo = &m_DescriptorSetInfos[i];
                            oldSet = true;
                        }
                    }

                    RZDescriptor rzDescriptor;

  
                    std::cout << "SPIRV ID                  : " << descriptor->spirv_id << std::endl;
                    std::cout << "UBO Name                  : " << descriptor->name << std::endl;
                    std::cout << "Binding                   : " << descriptor->binding << std::endl;
                    std::cout << "Set                       : " << descriptor->set << std::endl;
                    std::cout << "Count                     : " << descriptor->count << std::endl;
                    std::cout << "Descriptor Type           : " << ToStringDescriptorType(descriptor->descriptor_type) << std::endl;
                    std::cout << "Member Count              : " << descriptor->block.member_count << std::endl;
                    //
                    //for (size_t i = 0; i < descriptor->block.member_count; i++) {
                    //
                    //    std::cout << "\t Member Name : " << descriptor->block.members[i].name << std::endl;
                    //    std::cout << "\t Member offset : " << descriptor->block.members[i].offset << std::endl;
                    //    std::cout << "\t Member size : " << descriptor->block.members[i].size << std::endl;
                    //}
                    //std::cout << "---------------------------------------------" << std::endl;

                    // First create the descriptor layout bindings, these describe where and what kind of resources are being bound to the shader per descriptor set
                    // Which means each descriptor set (i.e. for a given set ID) it stores a list of binding layouts in a map
                    VkDescriptorSetLayoutBinding setLayoutBindingInfo = {};
                    setLayoutBindingInfo.binding = descriptor->binding;
                    setLayoutBindingInfo.descriptorCount = 1; // descriptorCount is the number of descriptors contained in the binding, accessed in a shader as an array, if any (useful for Animation aka JointTransforms)
                    setLayoutBindingInfo.descriptorType = (VkDescriptorType)descriptor->descriptor_type;
                    setLayoutBindingInfo.stageFlags = VKUtilities::ShaderStageToVK(spvSource.first);

                    m_VKSetBindingLayouts[descriptor->set].push_back(setLayoutBindingInfo);

                    // -->Also store all this data for the engine as well.
                    RZDescriptorLayoutBinding bindingInfo;
                    bindingInfo.binding = descriptor->binding;
                    bindingInfo.count   = 1;
                    bindingInfo.name    = descriptor->name;
                    bindingInfo.type    = VKToEngineDescriptorType(descriptor->descriptor_type);
                    bindingInfo.stage   = spvSource.first;

                    rzDescriptor.bindingInfo = bindingInfo;
                    rzDescriptor.name = descriptor->name;
                    rzDescriptor.offset = descriptor->block.offset;
                    rzDescriptor.size = descriptor->block.size;

                    for (size_t i = 0; i < descriptor->block.member_count; i++) {
                        RZShaderBufferMemberInfo memberInfo;
                        memberInfo.fullName = rzDescriptor.name + "." + descriptor->block.members[i].name;
                        memberInfo.name = descriptor->block.members[i].name;
                        memberInfo.offset = descriptor->block.members[i].offset;
                        memberInfo.size = descriptor->block.members[i].size;

                        rzDescriptor.uboMembers.push_back(memberInfo);
                    }

                    setInfo->setID = descriptor->set;
                    setInfo->descriptors.push_back(rzDescriptor);

                    if (!oldSet && setInfo->setID != -1)
                        m_DescriptorSetInfos.push_back(*setInfo);
                }
                //if(!oldSet && setInfo->setID != -1)
                //    m_DescriptorSetInfos.push_back(*setInfo);

                // Get info about push constants
                uint32_t push_constants_count = 0;
                spvReflectEnumeratePushConstantBlocks(&module, &push_constants_count, nullptr);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect push constants from shader - ({0})", virtualPath);
                SpvReflectBlockVariable** pp_push_constant_blocks =(SpvReflectBlockVariable**) malloc(var_count * sizeof(SpvReflectBlockVariable*));
                spvReflectEnumeratePushConstantBlocks(&module, &push_constants_count, pp_push_constant_blocks);

                // Create Push constants and store info about it
                for (uint32_t i = 0; i < push_constants_count; i++) {
                    SpvReflectBlockVariable* pushConstant = pp_push_constant_blocks[i];
                    //std::cout << "Name      : " << pushConstant->name << std::endl;
                    //std::cout << "Size      : " << pushConstant->size << std::endl;
                    //std::cout << "Offset    : " << pushConstant->offset << std::endl;

                    m_PushConstants.push_back(RZPushConstant(pushConstant->name, spvSource.first, nullptr, pushConstant->size, pushConstant->offset));
                }
                // Destroy the reflection data when no longer required
                spvReflectDestroyShaderModule(&module);
            }

            // Create the Vulkan set layouts for each set ID with the descriptors (bindings) it has
            // Set layout describe how a particular descriptor and it's bindings are layed out
            for (const auto& setLayouts : m_VKSetBindingLayouts) {
                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = setLayouts.second.size();
                layoutInfo.pBindings = setLayouts.second.data();

                VkDescriptorSetLayout& setLayout = m_PerSetLayouts[setLayouts.first];
                if (VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VKDevice::Get().getDevice(), &layoutInfo, nullptr, &setLayout)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor set layout!");
                else RAZIX_CORE_TRACE("[Vulkan] Successfully created descriptor set layout");
            }

            std::vector<VkDescriptorSetLayout> descriptorLayouts;
            for (std::unordered_map<uint32_t, VkDescriptorSetLayout>::iterator it = m_PerSetLayouts.begin(); it != m_PerSetLayouts.end(); ++it) {
                descriptorLayouts.push_back(it->second);
            }

            // Create Push constants for vulkan
            for (auto& pushConst : m_PushConstants) {
                VkPushConstantRange pushConstantRange{};
                pushConstantRange.stageFlags = VKUtilities::ShaderStageToVK(pushConst.shaderStage);
                pushConstantRange.offset = pushConst.offset;
                pushConstantRange.size = pushConst.size;
                m_VKPushConstants.push_back(pushConstantRange);
            }

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
            pipelineLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();

            pipelineLayoutCreateInfo.pushConstantRangeCount = uint32_t(m_VKPushConstants.size());
            pipelineLayoutCreateInfo.pPushConstantRanges = m_VKPushConstants.data();

            if (VK_CHECK_RESULT(vkCreatePipelineLayout(VKDevice::Get().getDevice(), &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_PipelineLayout)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create pipeline layout!");
            else RAZIX_CORE_TRACE("[Vulkan] Successfully created pipeline layout!");

            for (size_t i = 0; i < descriptorLayouts.size(); i++) {
                vkDestroyDescriptorSetLayout(VKDevice::Get().getDevice(), descriptorLayouts[i], nullptr);
            }
        }

        void VKShader::createShaderModules()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (const auto& spvSource : m_ParsedRZSF) {

                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                const uint32_t* spvByteCode = reinterpret_cast<uint32_t*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                VkShaderModuleCreateInfo shaderModuleCI = {};
                shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                shaderModuleCI.codeSize = fileSize;
                shaderModuleCI.pCode = spvByteCode;

                m_ShaderCreateInfos[spvSource.first].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_ShaderCreateInfos[spvSource.first].pName = "main"; // TODO: Extract this from shader later
                m_ShaderCreateInfos[spvSource.first].stage = VKUtilities::ShaderStageToVK(spvSource.first);

                if(VK_CHECK_RESULT(vkCreateShaderModule(VKDevice::Get().getDevice(), &shaderModuleCI, nullptr, &m_ShaderCreateInfos[spvSource.first].module)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create shader module!");
                else RAZIX_CORE_TRACE("[Vulkan] Successfully created shader module");
            }
        }
    }
}