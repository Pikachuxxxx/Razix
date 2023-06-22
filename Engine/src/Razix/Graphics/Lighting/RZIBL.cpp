// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZIBL.h"

#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

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

        RZCubeMap* RZIBL::convertEquirectangularToCubemap(const std::string& hdrFilePath)
        {
            // https://www.reddit.com/r/vulkan/comments/mtx6ar/gl_layer_value_assigned_in_vertex_shader_but/
            // https://www.reddit.com/r/vulkan/comments/xec0vg/multilayered_rendering_to_create_a_cubemap/

            // First create the 2D Equirectangular texture
            u32            width, height, bpp;
            unsigned char* pixels = Razix::Utilities::LoadImageData(hdrFilePath, &width, &height, &bpp);

            RZTexture2D* equirectangularMap = RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("HDR Cube Map Texture") "HDR Cube Map Texture", width, height, pixels, RZTextureProperties::Format::RGBA8, RZTextureProperties::Wrapping::CLAMP_TO_EDGE);

            std::vector<RZDescriptorSet*> envMapSets;
            std::vector<RZUniformBuffer*> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behavior is unknown

            // Load the shader
            auto  shader   = RZShaderLibrary::Get().getShader("EnvToCubeMap.rzsf");
            auto& setInfos = shader->getSetsCreateInfos();
            for (int i = 0; i < 6; i++) {
                uboData.view             = kCaptureViews[i];
                uboData.projection       = kCubeProjection;
                uboData.projection[1][1] = -1;
                uboData.layer            = i;

                RZUniformBuffer* viewProjLayerUBO = RZUniformBuffer::Create(sizeof(ViewProjLayerUBOData), &uboData RZ_DEBUG_NAME_TAG_STR_E_ARG("ViewProjLayerUBOData : #" + std::to_string(i)));
                UBOs.push_back(viewProjLayerUBO);

                for (auto& setInfo: setInfos) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: setInfo.second) {
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                            descriptor.texture = equirectangularMap;
                        if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Env map conversion set : #" + std::to_string(i)), false);
                    envMapSets.push_back(set);
                }
            }

            // Create the Pipeline
            Graphics::PipelineDesc pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = shader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTextureProperties::Format::RGBA32F};
            RZPipeline* envMapPipeline          = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Envmap Pipeline"));

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            RZCubeMap* cubeMap = RZCubeMap::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Envmap HDR ") "HDR", 512, 512);

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            u32 layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Cubemap", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))

                cmdBuffer->UpdateViewport(512, 512);

                RenderingInfo info{};
                info.colorAttachments = {
                    {cubeMap, {true, glm::vec4(0.0f)}}};
                info.extent = {512, 512};
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                RHI::BeginRendering(cmdBuffer, info);

                envMapPipeline->Bind(cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                for (u32 i = 0; i < layerCount; i++) {
                    RHI::BindDescriptorSets(envMapPipeline, cmdBuffer, &envMapSets[i], 1);
                    RHI::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RHI::EndRendering(cmdBuffer);
                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            equirectangularMap->Release(true);

            for (sz i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                UBOs[i]->Destroy();
            }
            envMapPipeline->Destroy();
            cubeMesh->Destroy();

            cubeMap->setMipLevel(0);

            return cubeMap;
        }

        RZCubeMap* RZIBL::generateIrradianceMap(RZCubeMap* cubeMap)
        {
            RZCubeMap* irradianceMap = RZCubeMap::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Irradiance Map") "Irradiance Map", 32, 32);

            // Load the shader for converting plain cubemap to irradiance map by convolution
            RZShader* cubemapConvolutionShader = RZShaderLibrary::Get().getShader("GenerateIrradianceMap.rzsf");

            // Create the View Projection buffer

            std::vector<RZDescriptorSet*> envMapSets;
            std::vector<RZUniformBuffer*> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behavior is unknown

            // Load the shader
            auto& setInfos = cubemapConvolutionShader->getSetsCreateInfos();
            for (int i = 0; i < 6; i++) {
                uboData.view       = kCaptureViews[i];
                uboData.projection = kCubeProjection;
                uboData.projection[1][1] = -1;
                uboData.layer      = i;

                RZUniformBuffer* viewProjLayerUBO = RZUniformBuffer::Create(sizeof(ViewProjLayerUBOData), &uboData RZ_DEBUG_NAME_TAG_STR_E_ARG("ViewProjLayerUBOData : #" + std::to_string(i)));
                UBOs.push_back(viewProjLayerUBO);

                for (auto& setInfo: setInfos) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: setInfo.second) {
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                            descriptor.texture = cubeMap;
                        if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Irradiance map conversion set : #" + std::to_string(i)), false);
                    envMapSets.push_back(set);
                }
            }

            // Create the Pipeline
            Graphics::PipelineDesc pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = cubemapConvolutionShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTextureProperties::Format::RGBA32F};
            RZPipeline* envMapPipeline          = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Irradiance Pipeline"));

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            u32 layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Irradiance cubemap Convolution", glm::vec4(0.8f, 0.4f, 0.3f, 1.0f))

                cmdBuffer->UpdateViewport(32, 32);

                RenderingInfo info{};
                info.colorAttachments = {
                    {irradianceMap, {true, glm::vec4(0.0f)}}};
                info.extent = {32, 32};
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                RHI::BeginRendering(cmdBuffer, info);

                envMapPipeline->Bind(cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                for (u32 i = 0; i < layerCount; i++) {
                    RHI::BindDescriptorSets(envMapPipeline, cmdBuffer, &envMapSets[i], 1);
                    RHI::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RHI::EndRendering(cmdBuffer);

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            for (sz i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                UBOs[i]->Destroy();
            }
            envMapPipeline->Destroy();
            cubeMesh->Destroy();

            return irradianceMap;
        }

        RZCubeMap* RZIBL::generatePreFilteredMap(RZCubeMap* cubeMap)
        {
            u32        dim            = 128;
            RZCubeMap* preFilteredMap = RZCubeMap::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Pre Filtered Map") "Pre Filtered Map", dim, dim, true);

            // Load the shader for converting plain cubemap to irradiance map by convolution
            RZShader* cubemapConvolutionShader = RZShaderLibrary::Get().getShader("GeneratePreFilteredMap.rzsf");

            // Create the View Projection buffer

            std::vector<RZDescriptorSet*> envMapSets;
            std::vector<RZUniformBuffer*> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behavior is unknown

            // Load the shader
            auto& setInfos = cubemapConvolutionShader->getSetsCreateInfos();
            for (int i = 0; i < 6; i++) {
                uboData.view       = kCaptureViews[i];
                uboData.projection = kCubeProjection;
                uboData.projection[1][1] = -1;
                uboData.layer      = i;

                RZUniformBuffer* viewProjLayerUBO = RZUniformBuffer::Create(sizeof(ViewProjLayerUBOData), &uboData RZ_DEBUG_NAME_TAG_STR_E_ARG("ViewProjLayerUBOData : #" + std::to_string(i)));
                UBOs.push_back(viewProjLayerUBO);

                for (auto& setInfo: setInfos) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: setInfo.second) {
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                            descriptor.texture = cubeMap;
                        if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Pre Filtered Map conversion set : #" + std::to_string(i)), false);
                    envMapSets.push_back(set);
                }
            }

            // Create the Pipeline
            Graphics::PipelineDesc pipelineInfo{};
            pipelineInfo.cullMode               = Graphics::CullMode::NONE;
            pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                 = cubemapConvolutionShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::RZTextureProperties::Format::RGBA32F};
            RZPipeline* envMapPipeline          = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Pre Filtered Map Pipeline"));

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            u32 layerCount   = 6;
            u32 maxMipLevels = 5;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("PreFiltering cubemap", glm::vec4(0.8f, 0.8f, 0.3f, 1.0f))

                for (u32 mip = 0; mip < maxMipLevels; mip++) {
                    cmdBuffer->UpdateViewport(dim, dim);

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {preFilteredMap, {true, glm::vec4(0.0f)}}};
                    info.extent = {dim, dim};
                    // NOTE: This is very important for layers to work
                    info.layerCount = 6;

                    preFilteredMap->setMipLevel(mip);
                    RHI::BeginRendering(cmdBuffer, info);

                    envMapPipeline->Bind(cmdBuffer);

                    cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                    cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                    for (u32 i = 0; i < layerCount; i++) {
                        RHI::BindDescriptorSets(envMapPipeline, cmdBuffer, &envMapSets[i], 1);

                        float roughness = (float) mip / (float) (maxMipLevels - 1);
                        struct PCData
                        {
                            float roughness;
                        } data;
                        data.roughness = roughness;
                        RZPushConstant pc;
                        pc.shaderStage = ShaderStage::PIXEL;
                        pc.data = &data;
                        pc.size = sizeof(PCData);

                        RHI::BindPushConstant(envMapPipeline, cmdBuffer, pc);

                        RHI::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                    }

                    dim = dim / 2;
                }
                RHI::EndRendering(cmdBuffer);

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            preFilteredMap->setMipLevel(0);

            for (sz i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                UBOs[i]->Destroy();
            }
            envMapPipeline->Destroy();
            cubeMesh->Destroy();

            return preFilteredMap;
        }
    }    // namespace Graphics
}    // namespace Razix
