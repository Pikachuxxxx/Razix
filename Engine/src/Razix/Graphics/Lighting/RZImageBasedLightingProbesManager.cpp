// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImageBasedLightingProbesManager.h"

#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Utilities/LoadImage.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include "Razix/Platform/API/Vulkan/VKDevice.h"

namespace Razix {
    namespace Graphics {

        static const auto kCubeProjection =
            glm::perspective(glm::radians(90.0f), 1.0f, 0.0f, 1.0f);

        static const glm::mat4 kCaptureViews[]{
            glm::lookAt(glm::vec3{0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}),      // +X
            glm::lookAt(glm::vec3{0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}),     // -X
            glm::lookAt(glm::vec3{0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}),     // +Y
            glm::lookAt(glm::vec3{0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}),       // -Y
            glm::lookAt(glm::vec3{0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}),      // +Z
            glm::lookAt(glm::vec3{0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f})};    // -Z

        RZTextureHandle RZImageBasedLightingProbesManager::convertEquirectangularToCubemap(const std::string& hdrFilePath)
        {
            // https://www.reddit.com/r/vulkan/comments/mtx6ar/gl_layer_value_assigned_in_vertex_shader_but/
            // https://www.reddit.com/r/vulkan/comments/xec0vg/multilayered_rendering_to_create_a_cubemap/

            // First create the 2D Equirectangular texture
            u32  width, height, bpp;
            f32* pixels = Razix::Utilities::LoadImageDataFloat(hdrFilePath, &width, &height, &bpp);

            RZTextureHandle equirectangularMapHandle = RZResourceManager::Get().createTexture({.name = "HDR Cube Map Texture", .width = width, .height = height, .data = pixels, .format = TextureFormat::RGBA32F, .wrapping = Wrapping::CLAMP_TO_EDGE, .enableMips = false,  .dataSize = sizeof(float)});

            std::vector<RZDescriptorSet*>      envMapSets;
            std::vector<RZUniformBufferHandle> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // FIXME: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behavior is unknown, yet!

            // Load the shader
            auto shaderHandle = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::EnvToCubemap);
            auto shader       = RZResourceManager::Get().getShaderResource(shaderHandle);
            auto setInfos     = shader->getDescriptorsPerHeapMap();
            for (int i = 0; i < 6; i++) {
                uboData.view             = kCaptureViews[i];
                uboData.projection       = kCubeProjection;
                uboData.projection[1][1] = -1;
                uboData.layer            = i;

                RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer({.name = "ViewProjLayerUBOData : #" + std::to_string(i), .size = sizeof(ViewProjLayerUBOData), .data = &uboData});

                UBOs.push_back(viewProjLayerUBO);

                for (auto& setInfo: setInfos) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: setInfo.second) {
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::ImageSamplerCombined)
                            descriptor.texture = equirectangularMapHandle;
                        if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Env map conversion set : #" + std::to_string(i)), false);
                    envMapSets.push_back(set);
                }
            }

            u32 dim = 512;

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Env map pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = shaderHandle;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            RZPipelineHandle envMapPipeline     = RZResourceManager::Get().createPipeline(pipelineInfo);

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            RZTextureHandle cubeMapHandle = RZResourceManager::Get().createTexture({.name = "HDR Env Map",
                .width                                                                    = dim,
                .height                                                                   = dim,
                .layers                                                                   = 6,
                .type                                                                     = TextureType::Texture_CubeMap,
                .format                                                                   = TextureFormat::RGBA32F,
                .filtering                                                                = {Filtering::Mode::LINEAR, Filtering::Mode::LINEAR},
                .enableMips                                                               = false});

            u32 layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Cubemap", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))

                RenderingInfo info{};
                info.resolution       = Resolution::kCustom;
                info.colorAttachments = {
                    {cubeMapHandle, {true, ClearColorPresets::TransparentBlack}}};
                info.extent = {dim, dim};
                //------------------------------------------------
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                //------------------------------------------------
                RHI::BeginRendering(cmdBuffer, info);

                RHI::BindPipeline(envMapPipeline, cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                // Bind the Bindless Env map texture 2d

                u32            idx = equirectangularMapHandle.getIndex();
                RZPushConstant pc;
                pc.size        = sizeof(u32);
                pc.shaderStage = ShaderStage::Pixel;
                pc.data        = &idx;

                //RHI::BindPushConstant(envMapPipeline, cmdBuffer, pc);

                for (u32 i = 0; i < layerCount; i++) {
                    RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSets[i], BindingTable_System::SET_IDX_SYSTEM_START);
                    //RHI::EnableBindlessTextures(envMapPipeline, cmdBuffer);
                    RHI::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RHI::EndRendering(cmdBuffer);
                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            RZResourceManager::Get().destroyTexture(equirectangularMapHandle);

            // Generate mip maps from first mip face
            //RZTexture* envCubeMap = RZResourceManager::Get().getPool<RZTexture>().get(cubeMapHandle);
            //envCubeMap->GenerateMips();

            for (sz i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                RZResourceManager::Get().destroyUniformBuffer(UBOs[i]);
            }
            RZResourceManager::Get().destroyPipeline(envMapPipeline);
            cubeMesh->Destroy();

            return cubeMapHandle;
        }

        RZTextureHandle RZImageBasedLightingProbesManager::generateIrradianceMap(RZTextureHandle cubeMap)
        {
            u32 dim = 32;

            RZTextureHandle irradianceMapHandle = RZResourceManager::Get().createTexture({.name = "Irradiance Map",
                .width                                                                          = dim,
                .height                                                                         = dim,
                .layers                                                                         = 6,
                .type                                                                           = TextureType::Texture_CubeMap,
                .format                                                                         = TextureFormat::RGBA32F,
                .enableMips                                                                     = false});

            // Load the shader for converting plain cubemap to irradiance map by convolution
            RZShaderHandle cubemapConvolutionShaderHandle = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GenerateIrradianceMap);
            auto           shader                         = RZResourceManager::Get().getShaderResource(cubemapConvolutionShaderHandle);

            // Create the View Projection buffer

            std::vector<RZDescriptorSet*>      envMapSets;
            std::vector<RZUniformBufferHandle> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behavior is unknown

            // Load the shader
            auto setInfos = shader->getDescriptorsPerHeapMap();
            for (int i = 0; i < 6; i++) {
                uboData.view             = kCaptureViews[i];
                uboData.projection       = kCubeProjection;
                uboData.projection[1][1] = -1;
                uboData.layer            = i;

                RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer({.name = "ViewProjLayerUBOData : #" + std::to_string(i), .size = sizeof(ViewProjLayerUBOData), .data = &uboData});
                UBOs.push_back(viewProjLayerUBO);

                for (auto& setInfo: setInfos) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: setInfo.second) {
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::ImageSamplerCombined)
                            descriptor.texture = cubeMap;
                        if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Irradiance map conversion set : #" + std::to_string(i)), false);
                    envMapSets.push_back(set);
                }
            }

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Irradiance Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = cubemapConvolutionShaderHandle;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            RZPipelineHandle envMapPipeline     = RZResourceManager::Get().createPipeline(pipelineInfo);

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            u32 layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Irradiance cubemap Convolution", glm::vec4(0.8f, 0.4f, 0.3f, 1.0f))

                RenderingInfo info{};
                info.resolution       = Resolution::kCustom;
                info.colorAttachments = {
                    {irradianceMapHandle, {true, ClearColorPresets::TransparentBlack}}};
                info.extent = {dim, dim};
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                RHI::BeginRendering(cmdBuffer, info);

                RHI::BindPipeline(envMapPipeline, cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                //u32            idx = cubeMap.getIndex();
                //RZPushConstant pc;
                //pc.size        = sizeof(u32);
                //pc.shaderStage = ShaderStage::PIXEL;
                //pc.data        = &idx;

                //RHI::BindPushConstant(envMapPipeline, cmdBuffer, pc);

                for (u32 i = 0; i < layerCount; i++) {
                    RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSets[i], BindingTable_System::SET_IDX_SYSTEM_START);
                    //RHI::EnableBindlessTextures(envMapPipeline, cmdBuffer);
                    RHI::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RHI::EndRendering(cmdBuffer);

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            for (sz i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                RZResourceManager::Get().destroyUniformBuffer(UBOs[i]);
            }
            RZResourceManager::Get().destroyPipeline(envMapPipeline);
            cubeMesh->Destroy();

            return irradianceMapHandle;
        }

        RZTextureHandle RZImageBasedLightingProbesManager::generatePreFilteredMap(RZTextureHandle cubeMap)
        {
            u32 dim = 128;

            RZTextureHandle preFilteredMapHandle = RZResourceManager::Get().createTexture({.name = "Pre Filtered Map",
                .width                                                                           = dim,
                .height                                                                          = dim,
                .layers                                                                          = 6,
                .type                                                                            = TextureType::Texture_CubeMap,
                .format                                                                          = TextureFormat::RGBA32F,
                //.filtering                                                                       = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                .enableMips = true});

            RZTexture* preFilteredMap = RZResourceManager::Get().getPool<RZTexture>().get(preFilteredMapHandle);

            // Load the shader for converting plain cubemap to irradiance map by convolution
            RZShaderHandle cubemapConvolutionShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GeneratePreFilteredMap);
            auto           shader                   = RZResourceManager::Get().getShaderResource(cubemapConvolutionShader);

            // Create the View Projection buffer

            std::vector<RZDescriptorSet*>      envMapSets;
            std::vector<RZUniformBufferHandle> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behavior is unknown

            // Load the shader
            auto setInfos = shader->getDescriptorsPerHeapMap();
            for (int i = 0; i < 6; i++) {
                uboData.view             = kCaptureViews[i];
                uboData.projection       = kCubeProjection;
                uboData.projection[1][1] = -1;
                uboData.layer            = i;

                RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer({.name = "ViewProjLayerUBOData : #" + std::to_string(i), .size = sizeof(ViewProjLayerUBOData), .data = &uboData});
                UBOs.push_back(viewProjLayerUBO);

                //for (auto& setInfo: setInfos) {
                //    // Fill the descriptors with buffers and textures
                //    for (auto& descriptor: setInfo.second) {
                //        if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                //            descriptor.uniformBuffer = viewProjLayerUBO;
                //    }
                //    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Pre Filtered Map conversion set : #" + std::to_string(i)), false);
                //    envMapSets.push_back(set);
                //}

                for (auto& setInfo: setInfos) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: setInfo.second) {
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::ImageSamplerCombined)
                            descriptor.texture = cubeMap;
                        if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Pre Filtered Map conversion set : #" + std::to_string(i)), false);
                    envMapSets.push_back(set);
                }
            }

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Pre Filtered Map Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = cubemapConvolutionShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            RZPipelineHandle envMapPipeline     = RZResourceManager::Get().createPipeline(pipelineInfo);

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            u32 layerCount   = 6;
            u32 maxMipLevels = RZTexture::calculateMipMapCount(dim, dim);

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("PreFiltering cubemap", glm::vec4(0.8f, 0.8f, 0.3f, 1.0f))

                for (u32 mip = 0; mip < maxMipLevels; mip++) {
                    u32 mipWidth  = static_cast<u32>(dim * std::pow(0.5, mip));
                    u32 mipHeight = static_cast<u32>(dim * std::pow(0.5, mip));

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {
                        {preFilteredMapHandle, {true, ClearColorPresets::TransparentBlack}}};
                    info.extent = {mipWidth, mipHeight};
                    // NOTE: This is very important for layers to work
                    info.layerCount = 6;

                    preFilteredMap->setCurrentMipLevel(mip);
                    RHI::BeginRendering(cmdBuffer, info);

                    RHI::BindPipeline(envMapPipeline, cmdBuffer);

                    cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                    cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                    for (u32 i = 0; i < layerCount; i++) {
                        RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSets[i], BindingTable_System::SET_IDX_SYSTEM_START);
                        //RHI::EnableBindlessTextures(envMapPipeline, cmdBuffer);

                        float roughness = (float) mip / (float) (maxMipLevels - 1);
                        struct PCData
                        {
                            float roughness = 0.0f;
                        } data{};
                        data.roughness = roughness;
                        RZPushConstant pc;
                        pc.shaderStage = ShaderStage::Pixel;
                        pc.data        = &data;
                        pc.size        = sizeof(PCData);

                        RHI::BindPushConstant(envMapPipeline, cmdBuffer, pc);

                        RHI::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                    }
                    RHI::EndRendering(cmdBuffer);
                }

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            preFilteredMap->setCurrentMipLevel(0);

            for (sz i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                RZResourceManager::Get().destroyUniformBuffer(UBOs[i]);
            }
            RZResourceManager::Get().destroyPipeline(envMapPipeline);
            cubeMesh->Destroy();

            return preFilteredMapHandle;
        }
    }    // namespace Graphics
}    // namespace Razix
