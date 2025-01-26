// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKShader.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/AssetSystem/RZAssetFileSpec.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include "internal/RazixMemory/include/RZMemoryFunctions.h"

#include <SPIRVReflect/common/output_stream.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <spirv_reflect.h>

namespace Razix {
    namespace Gfx {

        VKShader::VKShader(const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Desc = desc;

            m_Desc.name = Razix::Utilities::GetFileName(desc.filePath);

            // Read the *.rzsf shader file to get the necessary shader stages and it's corresponding compiled shader file virtual path
            m_ParsedRZSF = RZShader::ParseRZSF(desc.filePath);

            // Reflect the shaders using SPIR-V Reflect to extract the necessary information about descriptors and inputs to the shaders
            reflectShader();

            createLayoutHandles();

            // Create the shader modules and the pipeline shader stage create infos that will be bound to the pipeline
            createShaderModules();
        }

        RAZIX_CLEANUP_RESOURCE_IMPL(VKShader)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            destroyUserDescriptorSets();

            // Destroy the pipeline layout
            vkDestroyPipelineLayout(VKDevice::Get().getDevice(), m_PipelineLayout, nullptr);

            // Destroy the shader modules
            for (const auto& spvSource: m_ParsedRZSF)
                vkDestroyShaderModule(VKDevice::Get().getDevice(), m_ShaderCreateInfos[spvSource.first].module, nullptr);
        }

        void VKShader::GenerateUserDescriptorHeaps()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            destroyUserDescriptorSets();

            // Create the Descriptor Sets for the Shader
            // We skip if they're system sets like FrameData, SceneLightsData and Material which are managed by RHI and Scene
            for (auto& heap: m_DescriptorsPerHeap) {
                // Skip creating some sets!
                // We skip the system sets such as FrameData, SceneLightsData and Material Data
                auto& descriptor = heap.second[0];

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
                RZDescriptorSetDesc descSetCreateDesc = {};
                descSetCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                descSetCreateDesc.name                = "DescriptorSet." + getShaderFilePath();
                descSetCreateDesc.descriptors         = heap.second;
                descSetCreateDesc.setIdx              = BindingTable_System::SET_IDX_SYSTEM_START;
                auto setHandle                        = RZResourceManager::Get().createDescriptorSet(descSetCreateDesc);

                m_SceneParams.userSets.push_back(setHandle);

                // Add all descriptors of user sets into a named map
                for (auto& descr: heap.second)
                    m_BindVars.m_BindMap.insert({descr.name, &descr});
            }
        }

        //-----------------------------------------------------------------------------------

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

            // Reflect the SPIR-V shader to extract all the necessary information
            for (const auto& spvSource: m_ParsedRZSF) {
                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                const u8* spvByteCode = reinterpret_cast<u8*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                // Generate reflection data for a shader
                SpvReflectShaderModule module;
                SpvReflectResult       result = spvReflectCreateShaderModule(fileSize, spvByteCode, &module);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV shader - (%s)!", virtualPath);

                delete spvByteCode;

                // SPIRV-Reflect uses similar API to Vulkan, first enumerate for the count and then create the container and the use the same functions to fill the container with the enumerated data
                // Also SPIRV-Reflect API enums use same integer codes as Vulkan hence they can be type casted safely and they will work fine, this is guaranteed by the API hence we need not do extra conversions

                // Vertex Input attributes
                if (spvSource.first == ShaderStage::kVertex) {
                    m_VertexInputStride = 0;

                    //std::cout << "---------------------------------------------" << std::endl;
                    for (sz i = 0; i < module.input_variable_count; i++) {
                        SpvReflectInterfaceVariable inputVar = *module.input_variables[i];

                        if (inputVar.semantic == NULL && inputVar.name == NULL)
                            break;

                        if (inputVar.semantic && std::string(inputVar.semantic) == "SV_VertexID")
                            break;

                        if (inputVar.name && (std::string(inputVar.name) == "gl_VertexIndex" || std::string(inputVar.name) == "vs_out"))
                            break;

#if RAZIX_ASSET_VERSION == RAZIX_ASSET_VERSION_V1

                        // Fill the vulkan input variables attribute information
                        VkVertexInputAttributeDescription verextInputattribDesc = {};
                        verextInputattribDesc.binding                           = 0;
                        verextInputattribDesc.location                          = inputVar.location;
                        verextInputattribDesc.format                            = (VkFormat) inputVar.format;
                        verextInputattribDesc.offset                            = m_VertexInputStride;

                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc);

                        m_VertexInputStride += VKUtilities::GetStrideFromVulkanFormat((VkFormat) inputVar.format);

                        VKUtilities::PushBufferLayout((VkFormat) inputVar.format, inputVar.name, m_BufferLayout);
#else
                        // SOA so we will have a attrib per binding+location pair and also multiple VkVertexInputBindingDescriptions

                        VkVertexInputAttributeDescription verextInputattribDesc = {};
                        verextInputattribDesc.binding                           = inputVar.location;    // Since we will be doing SOA instead of AOS
                        verextInputattribDesc.location                          = inputVar.location;
                        verextInputattribDesc.format                            = (VkFormat) inputVar.format;
                        verextInputattribDesc.offset                            = 0;
                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc);

                        VkVertexInputBindingDescription bindingDesc = {};
                        bindingDesc.binding                         = verextInputattribDesc.binding;
                        bindingDesc.stride                          = VKUtilities::GetStrideFromVulkanFormat((VkFormat) inputVar.format);
                        bindingDesc.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;
                        m_VertexInputBindingDescriptions.push_back(bindingDesc);

                        // TODO: Not creating the buffer layout for Razix engine, but in case of SOA we will have many, each for a buffer, they are added manually for now, not reflected.
#endif
                        //delete inputVar;
                    }

                    // FIXME: Make this intuitive and don't hard code it
                    // Specializing vertex format for ImGui shaders
                    if (spvSource.second == "Compiled/SPIRV/Shader.Builtin.ImGui.vert.spv" || m_Desc.libraryID == ShaderBuiltin::ImGui) {
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

                for (u32 i = 0; i < module.descriptor_binding_count; i++) {
                    SpvReflectDescriptorBinding descriptor = module.descriptor_bindings[i];

                    RZDescriptor rzDescriptor = {};

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
                    bindingInfo.type             = VKUtilities::VKToEngineDescriptorType(descriptor.descriptor_type);
                    bindingInfo.stage            = spvSource.first;

                    if (descriptor.count >= 1024)
                        m_PotentiallyBindless = true;

                    rzDescriptor.bindingInfo = bindingInfo;
                    rzDescriptor.name        = descriptor.name;
                    rzDescriptor.offset      = descriptor.block.offset;
                    rzDescriptor.size        = descriptor.block.size;

#if ENABLE_THIS_IF_YOU_NEED_TO_INTERACT_WITH_UBO_MEMBERS
                    for (sz i = 0; i < descriptor.block.member_count; i++) {
                        RZShaderBufferMemberInfo memberInfo{};
                        memberInfo.fullName = rzDescriptor.name + "." + descriptor.block.members[i].name;
                        memberInfo.name     = descriptor.block.members[i].name;
                        memberInfo.offset   = descriptor.block.members[i].offset;
                        memberInfo.size     = descriptor.block.members[i].size;

                        rzDescriptor.uboMembers.push_back(std::move(memberInfo));
                    }
#endif

                    auto& descriptors_in_set = m_DescriptorsPerHeap[descriptor.set];
                    descriptors_in_set.push_back(rzDescriptor);
                }

                // Create Push constants and store info about it
                u32 numPushConstants = module.push_constant_block_count;
                RAZIX_CORE_ASSERT(numPushConstants <= 1, "[Vulkan] [Shader Reflection] Found more than one push constant in a single shader stage, this is unsupported bahaviour by the engine! Please use only one for each stage");

                for (u32 i = 0; i < numPushConstants; i++) {
                    SpvReflectBlockVariable* pushConstant = &module.push_constant_blocks[0];
                    RZPushConstant           pc{};
                    pc.name                         = pushConstant->name;
                    pc.shaderStage                  = spvSource.first;
                    pc.data                         = nullptr;
                    pc.size                         = pushConstant->size;
                    pc.offset                       = pushConstant->offset;
                    pc.bindingInfo.location.binding = 0;    // Doesn't make sense for PushConstants
                    pc.bindingInfo.location.set     = 0;    // Doesn't make sense for PushConstants
                    pc.bindingInfo.stage            = spvSource.first;
                    pc.bindingInfo.count            = 1;
                    pc.bindingInfo.type             = DescriptorType::kPushConstant;
                    for (sz i = 0; i < pushConstant->member_count; i++) {
                        auto                     member = pushConstant->members[i];
                        RZShaderBufferMemberInfo mem{};
                        mem.fullName = pc.name + "." + member.name;
                        mem.name     = member.name;
                        mem.offset   = member.offset;
                        mem.size     = member.size;
                        pc.structMembers.push_back(std::move(mem));
                    }
                    m_PushConstants.push_back(pc);
                }

                spvReflectDestroyShaderModule(&module);
            }

            // Generate the Descriptor Sets
            GenerateUserDescriptorHeaps();
        }

        void VKShader::createLayoutHandles()
        {
            createSetLayoutHandles();

            createPipelineLayoutHandles();

            if (m_PotentiallyBindless)
                createBindlessPipelineLayoutHandles();
        }

        void VKShader::createSetLayoutHandles()
        {
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

                VK_TAG_OBJECT(m_Desc.name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t) setLayout)
            }
        }

        void VKShader::createPipelineLayoutHandles()
        {
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

            VK_TAG_OBJECT(m_Desc.name, VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t) m_PipelineLayout)

            for (sz i = 0; i < descriptorLayouts.size(); i++)
                vkDestroyDescriptorSetLayout(VKDevice::Get().getDevice(), descriptorLayouts[i], nullptr);
        }

        void VKShader::createBindlessPipelineLayoutHandles()
        {
            // Replace the Set a index BindingTable_System::SET_IDX_BINDLESS_RESOURCES_START with the bindless set layout

            m_PerSetLayouts[BindingTable_System::SET_IDX_BINDLESS_RESOURCES_START] = VKDevice::Get().getBindlessSetLayout();

            std::vector<VkDescriptorSetLayout> descriptorLayoutsBindless;
            for (std::map<u32, VkDescriptorSetLayout>::iterator it = m_PerSetLayouts.begin(); it != m_PerSetLayouts.end(); ++it) {
                descriptorLayoutsBindless.push_back(it->second);
            }

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount             = static_cast<u32>(descriptorLayoutsBindless.size());
            pipelineLayoutCreateInfo.pSetLayouts                = descriptorLayoutsBindless.data();
            pipelineLayoutCreateInfo.pushConstantRangeCount     = u32(m_VKPushConstants.size());
            pipelineLayoutCreateInfo.pPushConstantRanges        = m_VKPushConstants.data();

            if (VK_CHECK_RESULT(vkCreatePipelineLayout(VKDevice::Get().getDevice(), &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &m_PipelineLayout)))
                RAZIX_CORE_ERROR("[Vulkan] Failed to create bindless pipeline layout!");
            else
                RAZIX_CORE_TRACE("[Vulkan] Successfully created bindless pipeline layout!");
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
                m_ShaderCreateInfos[spvSource.first].pName = g_ShaderStageEntryPointNameMap[spvSource.first];
                m_ShaderCreateInfos[spvSource.first].stage = VKUtilities::ShaderStageToVK(spvSource.first);

                if (VK_CHECK_RESULT(vkCreateShaderModule(VKDevice::Get().getDevice(), &shaderModuleCI, nullptr, &m_ShaderCreateInfos[spvSource.first].module)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create shader module!");
                else
                    RAZIX_CORE_TRACE("[Vulkan] Successfully created shader module");

                // Name tag
                VK_TAG_OBJECT(m_Desc.filePath, VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t) m_ShaderCreateInfos[spvSource.first].module);

                delete spvByteCode;
            }
        }

        void VKShader::destroyUserDescriptorSets()
        {
            for (size_t i = 0; i < m_SceneParams.userSets.size(); i++) {
                auto& setHandle = m_SceneParams.userSets[i];
                RZResourceManager::Get().destroyDescriptorSet(setHandle);
            }
            m_SceneParams.userSets.clear();
        }
    }    // namespace Gfx
}    // namespace Razix
