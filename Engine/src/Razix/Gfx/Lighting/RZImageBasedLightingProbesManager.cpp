// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImageBasedLightingProbesManager.h"

#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Renderers/RZSystemBinding.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Utilities/RZLoadImage.h"

#define GLM_FORCE_LEFT_HANDED
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include "Razix/Platform/API/Vulkan/VKDevice.h"

namespace Razix {
    namespace Gfx {

#define CUBEMAP_LAYERS            6
#define CUBEMAP_DIM               512
#define IRRADIANCE_MAP_DIM        32
#define DISPATCH_THREAD_GROUP_DIM 32

        // TODO: Use this via a RootConstant or PushConstant along with the texture bindless id
        struct EnvMapGenUBOData
        {
            glm::ivec2 cubeFaceSize = {CUBEMAP_DIM, CUBEMAP_DIM};
            u32        mipLevel     = 0;
        } uboData;

        RZTextureHandle RZImageBasedLightingProbesManager::convertEquirectangularToCubemap(const std::string& hdrFilePath)
        {
            // This is only when we use a VS+PS to render to different layers of a RT
            // We are currently switched back to using VS+GS+PS and in future we will use a CS so no need of this
            // https://www.reddit.com/r/vulkan/comments/mtx6ar/gl_layer_value_assigned_in_vertex_shader_but/
            // https://www.reddit.com/r/vulkan/comments/xec0vg/multilayered_rendering_to_create_a_cubemap/

            // First create the 2D Equirectangular texture
            u32  width, height, bpp;
            f32* pixels = Razix::Utilities::LoadImageDataFloat(hdrFilePath, &width, &height, &bpp);

            RZTextureDesc equiMapTextureDesc = {};
            equiMapTextureDesc.name          = "HDR Cube Map Texture";
            equiMapTextureDesc.width         = width;
            equiMapTextureDesc.height        = height;
            equiMapTextureDesc.data          = pixels;
            equiMapTextureDesc.size          = (width * height * 4 * sizeof(float));
            equiMapTextureDesc.format        = TextureFormat::RGBA32F;
            equiMapTextureDesc.wrapping      = Wrapping::CLAMP_TO_EDGE;
            equiMapTextureDesc.filtering     = {Filtering::Mode::LINEAR, Filtering::Mode::LINEAR};
            equiMapTextureDesc.enableMips    = false;
            equiMapTextureDesc.dataSize      = sizeof(float);

            RZTextureHandle equirectangularMapHandle = RZResourceManager::Get().createTexture(equiMapTextureDesc);

            RZDescriptorSet*      envMapSet = nullptr;
            RZUniformBufferHandle UBO;

            RZTextureDesc cubeMapTextureDesc = {};
            cubeMapTextureDesc.name          = "Texture.Imported.HDR.EnvMap",
            cubeMapTextureDesc.width         = CUBEMAP_DIM;
            cubeMapTextureDesc.height        = CUBEMAP_DIM;
            cubeMapTextureDesc.layers        = CUBEMAP_LAYERS;
            cubeMapTextureDesc.type          = TextureType::Texture_RWCubeMap;
            cubeMapTextureDesc.format        = TextureFormat::RGBA16F;
            cubeMapTextureDesc.filtering     = {Filtering::Mode::LINEAR, Filtering::Mode::LINEAR};
            cubeMapTextureDesc.enableMips    = false;

            RZTextureHandle cubeMapHandle = RZResourceManager::Get().createTexture(cubeMapTextureDesc);
            // Since it has both UAV and SRV, vulkan will internall specialize the creating a UAV with the type of RWTexture2DArray since
            // an actual RWTextureCube doesn't exist in shading languages
            RZResourceManager::Get().getTextureResource(cubeMapHandle)->setResourceViewHints(ResourceViewHint::kSRV | ResourceViewHint::kUAV);

            // Load the shader
            auto shaderHandle = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::EnvToCubemap);
            auto shader       = RZResourceManager::Get().getShaderResource(shaderHandle);
            auto setInfos     = shader->getDescriptorsPerHeapMap();

            RZBufferDesc vplBufferDesc{};
            vplBufferDesc.name = "EnvMapUBOData";
            vplBufferDesc.size = sizeof(EnvMapGenUBOData);
            vplBufferDesc.data = &uboData;

            RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer(vplBufferDesc);

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.name == "HDRTexture")
                        descriptor.texture = equirectangularMapHandle;
                    if (descriptor.name == "HDRSampler")
                        descriptor.texture = equirectangularMapHandle;
                    if (descriptor.name == "CubeMapRT")
                        descriptor.texture = cubeMapHandle;
                    if (descriptor.name == "Constants")
                        descriptor.uniformBuffer = viewProjLayerUBO;
                }
                envMapSet = Gfx::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Env map conversion set"));
            }

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo = {};
            pipelineInfo.name                = "Pipeline.EnvMapGen";
            pipelineInfo.pipelineType        = PipelineType::kCompute;
            pipelineInfo.shader              = shaderHandle;
            RZPipelineHandle envMapPipeline  = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Begin rendering
            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Cubemap", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))

                RHI::BindPipeline(envMapPipeline, cmdBuffer);

                RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSet, BindingTable_System::SET_IDX_SYSTEM_START);
                RHI::Dispatch(cmdBuffer, CUBEMAP_DIM / DISPATCH_THREAD_GROUP_DIM, CUBEMAP_DIM / DISPATCH_THREAD_GROUP_DIM, CUBEMAP_LAYERS);
                RAZIX_MARK_END()
                RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            RZResourceManager::Get().destroyTexture(equirectangularMapHandle);

            // TODO!!: Generate mip maps from first mip face
            if (envMapSet)
                envMapSet->Destroy();
            RZResourceManager::Get().destroyPipeline(envMapPipeline);

            return cubeMapHandle;
        }

        RZTextureHandle RZImageBasedLightingProbesManager::generateIrradianceMap(RZTextureHandle cubeMap)
        {
            RZTextureDesc irradianceMapTextureDesc = {};
            irradianceMapTextureDesc.name          = "Texture.IrradianceMap";
            irradianceMapTextureDesc.width         = IRRADIANCE_MAP_DIM;
            irradianceMapTextureDesc.height        = IRRADIANCE_MAP_DIM;
            irradianceMapTextureDesc.layers        = CUBEMAP_LAYERS;
            irradianceMapTextureDesc.type          = TextureType::Texture_RW2DArray;
            irradianceMapTextureDesc.format        = TextureFormat::RGBA16F;
            irradianceMapTextureDesc.wrapping      = Wrapping::CLAMP_TO_EDGE;
            irradianceMapTextureDesc.filtering     = {Filtering::Mode::LINEAR, Filtering::Mode::LINEAR};
            irradianceMapTextureDesc.enableMips    = false;

            RZTextureHandle irradianceMapHandle = RZResourceManager::Get().createTexture(irradianceMapTextureDesc);

            // Load the shader for converting plain cube map to irradiance map by convolution
            RZShaderHandle cubemapConvolutionShaderHandle = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GenerateIrradianceMap);
            auto           shader                         = RZResourceManager::Get().getShaderResource(cubemapConvolutionShaderHandle);
            auto           setInfos                       = shader->getDescriptorsPerHeapMap();

            // Create the View Projection buffer
            RZDescriptorSet*      envMapSet = nullptr;
            RZUniformBufferHandle UBO;

            RZBufferDesc vplBufferDesc{};
            vplBufferDesc.name = "ViewProjLayerUBOData";
            vplBufferDesc.size = sizeof(EnvMapGenUBOData);
            vplBufferDesc.data = &uboData;

            RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer(vplBufferDesc);

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.name == "EnvCubeMap")
                        descriptor.texture = cubeMap;
                    if (descriptor.name == "IrradianceSampler")
                        descriptor.texture = cubeMap;
                    if (descriptor.name == "IrradianceMap")
                        descriptor.texture = irradianceMapHandle;
                    if (descriptor.name == "Constants")
                        descriptor.uniformBuffer = viewProjLayerUBO;
                }
                envMapSet = Gfx::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Env map conversion set"));
            }

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Pipeline.Irradiance";
            pipelineInfo.pipelineType           = PipelineType::kCompute;
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = cubemapConvolutionShaderHandle;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA32F};
            RZPipelineHandle envMapPipeline     = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Begin rendering
            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer();
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Irradiance cubemap Convolution", glm::vec4(0.8f, 0.4f, 0.3f, 1.0f))

                RHI::BindPipeline(envMapPipeline, cmdBuffer);

                RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSet, BindingTable_System::SET_IDX_SYSTEM_START);

                RHI::Dispatch(cmdBuffer, IRRADIANCE_MAP_DIM / DISPATCH_THREAD_GROUP_DIM, IRRADIANCE_MAP_DIM / DISPATCH_THREAD_GROUP_DIM, CUBEMAP_LAYERS);

                RAZIX_MARK_END()
                RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            if (envMapSet)
                envMapSet->Destroy();
            RZResourceManager::Get().destroyPipeline(envMapPipeline);

            return irradianceMapHandle;
        }
#if 0
        RZTextureHandle RZImageBasedLightingProbesManager::generatePreFilteredMap(RZTextureHandle cubeMap)
        {
            u32 dim = 128;

            RZTextureDesc preFilteredMapTextureDesc{};
            preFilteredMapTextureDesc.name       = "Texture.PreFilteredMap";
            preFilteredMapTextureDesc.width      = dim;
            preFilteredMapTextureDesc.height     = dim;
            preFilteredMapTextureDesc.layers     = 6;
            preFilteredMapTextureDesc.type       = TextureType::Texture_CubeMap;
            preFilteredMapTextureDesc.format     = TextureFormat::RGBA32F;
            preFilteredMapTextureDesc.wrapping   = Wrapping::CLAMP_TO_EDGE;
            preFilteredMapTextureDesc.filtering  = {Filtering::Mode::LINEAR, Filtering::Mode::LINEAR};
            preFilteredMapTextureDesc.enableMips = true;

            RZTextureHandle preFilteredMapHandle = RZResourceManager::Get().createTexture(preFilteredMapTextureDesc);

            RZTexture* preFilteredMap = RZResourceManager::Get().getPool<RZTexture>().get(preFilteredMapHandle);

            // Load the shader for converting plain cube map to irradiance map by convolution
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

                RZBufferDesc vplBufferDesc{};
                vplBufferDesc.name = "ViewProjLayerUBOData : #" + std::to_string(i);
                vplBufferDesc.size = sizeof(ViewProjLayerUBOData);
                vplBufferDesc.data = &uboData;

                RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer(vplBufferDesc);
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
                        if (descriptor.bindingInfo.type == Gfx::DescriptorType::kImageSamplerCombined)
                            descriptor.texture = cubeMap;
                        if (descriptor.bindingInfo.type == DescriptorType::kUniformBuffer)
                            descriptor.uniformBuffer = viewProjLayerUBO;
                    }
                    auto set = Gfx::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Pre Filtered Map conversion set : #" + std::to_string(i)));
                    envMapSets.push_back(set);
                }
            }

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Pipeline.PreFilteredMap";
            pipelineInfo.cullMode               = Gfx::CullMode::None;
            pipelineInfo.drawType               = Gfx::DrawType::Triangle;
            pipelineInfo.shader                 = cubemapConvolutionShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Gfx::TextureFormat::RGBA32F};
            RZPipelineHandle envMapPipeline     = RZResourceManager::Get().createPipeline(pipelineInfo);

            RZMesh* cubeMesh = MeshFactory::CreatePrimitive(MeshPrimitive::Cube);

            u32 layerCount   = 6;
            u32 maxMipLevels = RZTexture::calculateMipMapCount(dim, dim);

            // Begin rendering
            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer();
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

                    // AOS Deprecated
                    //RZ_GET_RAW_RESOURCE(VertexBuffer, cubeMesh->getVertexBufferHandle())->Bind(cmdBuffer);
                    //RZ_GET_RAW_RESOURCE(IndexBuffer, cubeMesh->getIndexBufferHandle())->Bind(cmdBuffer);

                    cubeMesh->bindVBsAndIB(cmdBuffer);

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
                        pc.shaderStage = ShaderStage::kPixel;
                        pc.data        = &data;
                        pc.size        = sizeof(PCData);

                        RHI::BindPushConstant(envMapPipeline, cmdBuffer, pc);

                        RHI::DrawIndexed(cmdBuffer, RZ_GET_RAW_RESOURCE(IndexBuffer, cubeMesh->getIndexBufferHandle())->getCount(), 1, 0, 0, 0);
                    }
                    RHI::EndRendering(cmdBuffer);
                }

                RAZIX_MARK_END()
                RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
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

#endif
    }    // namespace Gfx
}    // namespace Razix
