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
            // First create the 2D Equirectangular texture
            uint32_t              width, height, bpp;
            unsigned char*        pixels = Razix::Utilities::LoadImageData(hdrFilePath, &width, &height, &bpp);
            std::vector<uint32_t> pixelData(width * height * 4);
            memcpy(pixelData.data(), pixels, (width * height * bpp));

            RZTexture2D* equirectangularMap = RZTexture2D::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("HDR Cube Map Texture") "HDR Cube Map Texture", width, height, pixelData.data(), RZTexture::Format::RGBA);

            std::vector<RZDescriptorSet*> envMapSet;

            struct ViewProjLayerUBOData
            {
                alignas(16) glm::mat4 view       = glm::mat4(1.0f);
                alignas(16) glm::mat4 projection = glm::mat4(1.0f);
                alignas(4) int layer             = 0;
            } uboData;

            RZUniformBuffer* viewProjLayerUBO = RZUniformBuffer::Create(sizeof(ViewProjLayerUBOData), &uboData RZ_DEBUG_NAME_TAG_STR_E_ARG("ViewProjLayerUBOData"));

            // Load the shader
            auto  shader   = RZShaderLibrary::Get().getShader("EnvToCubeMap.rzsf");
            auto& setInfos = shader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                        descriptor.texture = RZMaterial::GetDefaultTexture();    //equirectangularMap;
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = viewProjLayerUBO;
                }
                auto set = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Env map conversion set"));
                envMapSet.push_back(set);
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
            //RZTexture* rt = RZRenderTexture::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Envmap HDR RT") 512, 512, RZTexture::Format::RGBA32F);

            vkDeviceWaitIdle(VKDevice::Get().getDevice());

            // Begin rendering
            {
                uint32_t layerCount = 6;
                for (uint32_t i = 0; i < layerCount; i++) {
                    // Update the UBO data
                    uboData.view       = kCaptureViews[i];
                    uboData.projection = kCubeProjection;
                    uboData.layer      = i;
                    viewProjLayerUBO->SetData(sizeof(ViewProjLayerUBOData), &uboData);

                    auto cmdBuffer = RZCommandBuffer::BeginSingleTimeCommandBuffer();
                    {
                        RZRenderContext::SetCmdBuffer(cmdBuffer);

                        RAZIX_MARK_BEGIN("Cubemap", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))

                        cmdBuffer->UpdateViewport(512, 512);

                        RenderingInfo info{};
                        info.attachments = {
                            {cubeMap, {true, glm::vec4(0.0f)}}};
                        info.extent = {512, 512};
                        RZRenderContext::BeginRendering(cmdBuffer, info);

                        envMapPipeline->Bind(cmdBuffer);

                        RZRenderContext::BindDescriptorSets(envMapPipeline, cmdBuffer, envMapSet);

                        cubeMesh->getVertexBuffer()->Bind(cmdBuffer);
                        cubeMesh->getIndexBuffer()->Bind(cmdBuffer);

                        RZRenderContext::DrawIndexed(cmdBuffer, cubeMesh->getIndexBuffer()->getCount(), 1, 0, 0, 0);

                        RZRenderContext::EndRendering(cmdBuffer);

                        RAZIX_MARK_END()
                        RZCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
                    }
                }
            }
            equirectangularMap->Release(true);
            envMapSet[0]->Destroy();
            envMapPipeline->Destroy();
            cubeMap->Release(true);
            //rt->Release(true);
            cubeMesh->Destroy();
            viewProjLayerUBO->Destroy();

            return nullptr;
        }

        RZCubeMap* RZIBL::generateIrradianceMap(RZCubeMap* cubeMap)
        {
            return nullptr;
        }

        RZCubeMap* RZIBL::generatePreFilteredMap(RZCubeMap* cubeMap)
        {
            return nullptr;
        }

    }    // namespace Graphics
}    // namespace Razix
