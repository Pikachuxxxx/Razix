// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZIBL.h"

#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/API/RZCommandBuffer.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZPipeline.h"
#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZUniformBuffer.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"

#include "Razix/Graphics/API/RZRenderContext.h"

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
            glm::lookAt(glm::vec3{0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}),       // +Y
            glm::lookAt(glm::vec3{0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}),     // -Y
            glm::lookAt(glm::vec3{0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}),      // +Z
            glm::lookAt(glm::vec3{0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f})};    // -Z

        RZCubeMap* RZIBL::convertEquirectangularToCubemap(const std::string& hdrFilePath)
        {
            // https://www.reddit.com/r/vulkan/comments/mtx6ar/gl_layer_value_assigned_in_vertex_shader_but/
            // https://www.reddit.com/r/vulkan/comments/xec0vg/multilayered_rendering_to_create_a_cubemap/

            // First create the 2D Equirectangular texture
            uint32_t              width, height, bpp;
            unsigned char*        pixels = Razix::Utilities::LoadImageData(hdrFilePath, &width, &height, &bpp);
            std::vector<uint32_t> pixelData(width * height * 4);
            memcpy(pixelData.data(), pixels, (width * height * bpp));

            RZTexture2D* equirectangularMap = RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("HDR Cube Map Texture") "HDR Cube Map Texture", width, height, pixelData.data(), RZTexture::Format::RGBA8, RZTexture::Wrapping::CLAMP_TO_EDGE);

            std::vector<RZDescriptorSet*> envMapSets;
            std::vector<RZUniformBuffer*> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behaviour is unknown

            // Load the shader
            auto  shader   = RZShaderLibrary::Get().getShader("EnvToCubeMap.rzsf");
            auto& setInfos = shader->getSetsCreateInfos();
            for (int i = 0; i < 6; i++) {
                uboData.view       = kCaptureViews[i];
                uboData.projection = kCubeProjection;
                uboData.layer      = i;

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
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader              = shader;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.attachmentFormats   = {Graphics::RZTexture::Format::RGBA32F};
            RZPipeline* envMapPipeline       = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Envmap Pipeline"));

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            RZCubeMap* cubeMap = RZCubeMap::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Envmap HDR ") "HDR");

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            uint32_t layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RZRenderContext::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Cubemap", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))

                cmdBuffer->UpdateViewport(512, 512);

                RenderingInfo info{};
                info.attachments = {
                    {cubeMap, {true, glm::vec4(0.0f)}}};
                info.extent = {512, 512};
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                RZRenderContext::BeginRendering(cmdBuffer, info);

                envMapPipeline->Bind(cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                for (uint32_t i = 0; i < layerCount; i++) {
                    RZRenderContext::BindDescriptorSets(envMapPipeline, cmdBuffer, &envMapSets[i], 1);
                    RZRenderContext::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RZRenderContext::EndRendering(cmdBuffer);

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            equirectangularMap->Release(true);

            for (size_t i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                UBOs[i]->Destroy();
            }
            envMapPipeline->Destroy();
            cubeMesh->Destroy();

            return cubeMap;
        }

        RZCubeMap* RZIBL::generateIrradianceMap(RZCubeMap* cubeMap)
        {
            RZCubeMap* irradianceMap = RZCubeMap::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Irradiance Map") "Irradiance Map");

            // Load the shader for converting plain cubemap to irradiance map by convolution
            RZShader* cubemapConvolutionShader = RZShaderLibrary::Get().getShader("GenerateIrradianceMap.rzsf");

            // Create the View Proj buffer

            std::vector<RZDescriptorSet*> envMapSets;
            std::vector<RZUniformBuffer*> UBOs;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            // TODO: Disable layout transition when creating Env Map Texture, this causes the Mip 0 to be UNDEFINED, the reason for this weird behaviour is unknown

            // Load the shader
            auto& setInfos = cubemapConvolutionShader->getSetsCreateInfos();
            for (int i = 0; i < 6; i++) {
                uboData.view       = kCaptureViews[i];
                uboData.projection = kCubeProjection;
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
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader              = cubemapConvolutionShader;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.attachmentFormats   = {Graphics::RZTexture::Format::RGBA32F};
            RZPipeline* envMapPipeline       = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Irradiance Pipeline"));

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            uint32_t layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RZRenderContext::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Irradiance cubemap Convolution", glm::vec4(0.8f, 0.4f, 0.3f, 1.0f))

                cmdBuffer->UpdateViewport(512, 512);

                RenderingInfo info{};
                info.attachments = {
                    {irradianceMap, {true, glm::vec4(0.0f)}}};
                info.extent = {512, 512};
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                RZRenderContext::BeginRendering(cmdBuffer, info);

                envMapPipeline->Bind(cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                for (uint32_t i = 0; i < layerCount; i++) {
                    RZRenderContext::BindDescriptorSets(envMapPipeline, cmdBuffer, &envMapSets[i], 1);
                    RZRenderContext::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RZRenderContext::EndRendering(cmdBuffer);

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            for (size_t i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                UBOs[i]->Destroy();
            }
            envMapPipeline->Destroy();
            cubeMesh->Destroy();

            return irradianceMap;
        }

        RZCubeMap* RZIBL::generatePreFilteredMap(RZCubeMap* cubeMap)
        {
            RZCubeMap* preFilteredMap = RZCubeMap::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Pre Filtered Map") "Pre Filtered Map");

            // Load the shader for converting plain cubemap to irradiance map by convolution
            RZShader* cubemapConvolutionShader = RZShaderLibrary::Get().getShader("GeneratePreFilteredMap.rzsf");

            // Create the View Proj buffer

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
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader              = cubemapConvolutionShader;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.attachmentFormats   = {Graphics::RZTexture::Format::RGBA32F};
            RZPipeline* envMapPipeline       = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Pre Filtered Map Pipeline"));

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            uint32_t layerCount = 6;

            // Begin rendering
            auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RZRenderContext::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("PreFiltering cubemap", glm::vec4(0.8f, 0.8f, 0.3f, 1.0f))

                cmdBuffer->UpdateViewport(512, 512);

                RenderingInfo info{};
                info.attachments = {
                    {preFilteredMap, {true, glm::vec4(0.0f)}}};
                info.extent = {512, 512};
                // NOTE: This is very important for layers to work
                info.layerCount = 6;
                RZRenderContext::BeginRendering(cmdBuffer, info);

                envMapPipeline->Bind(cmdBuffer);

                cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                for (uint32_t i = 0; i < layerCount; i++) {
                    RZRenderContext::BindDescriptorSets(envMapPipeline, cmdBuffer, &envMapSets[i], 1);
                    RZRenderContext::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);
                }

                RZRenderContext::EndRendering(cmdBuffer);

                RAZIX_MARK_END()
                RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            for (size_t i = 0; i < envMapSets.size(); i++) {
                envMapSets[i]->Destroy();
                UBOs[i]->Destroy();
            }
            envMapPipeline->Destroy();
            cubeMesh->Destroy();

            return preFilteredMap;
        }
    }    // namespace Graphics
}    // namespace Razix