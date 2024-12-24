// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImageBasedLightingProbesManager.h"

#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZSampler.h"
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

#define CUBEMAP_LAYERS                6
#define CUBEMAP_DIM                   1024
#define IRRADIANCE_MAP_DIM            32
#define PREFILTERED_MAP_DIM           128
#define IBL_DISPATCH_THREAD_GROUP_DIM 32

        // TODO: Use this via a RootConstant or PushConstant along with the texture bindless id
        struct EnvMapGenUBOData
        {
            glm::ivec2 cubeFaceSize = {-1, -1};
            u32        mipLevel     = 0;
        } uboData;

        // - [ ] Fix this file to use descriptor handles, this is the first stage of integration and proceed to other parts of engine from here

        RZTextureHandle RZImageBasedLightingProbesManager::convertEquirectangularToCubemap(const std::string& hdrFilePath)
        {
            // This is only when we use a VS+PS to render to different layers of a RT (only Vulkan/AGC no HLSL support)
            //  --> https://www.reddit.com/r/vulkan/comments/mtx6ar/gl_layer_value_assigned_in_vertex_shader_but/
            //  --> https://www.reddit.com/r/vulkan/comments/xec0vg/multilayered_rendering_to_create_a_cubemap/
            // We are currently switched back to using VS+GS+PS and in future we will use a CS so no need of this
            // 12/21/2024: we have switched to using CS now, GS are inefficient and won't be good for dynamic CubeMaps

            // First create the 2D Equirectangular texture
            u32  width, height, bpp;
            f32* pixels = Razix::Utilities::LoadImageDataFloat(hdrFilePath, &width, &height, &bpp);

            RZTextureDesc equiMapTextureDesc         = {};
            equiMapTextureDesc.name                  = "HDR Equirectangular Texture";
            equiMapTextureDesc.width                 = width;
            equiMapTextureDesc.height                = height;
            equiMapTextureDesc.data                  = pixels;
            equiMapTextureDesc.size                  = (width * height * 4 * sizeof(float));
            equiMapTextureDesc.format                = TextureFormat::RGBA32F;
            equiMapTextureDesc.enableMips            = false;
            equiMapTextureDesc.dataSize              = sizeof(float);    // HDR mode
            RZTextureHandle equirectangularMapHandle = RZResourceManager::Get().createTexture(equiMapTextureDesc);

            // Since it has both UAV and SRV, vulkan will internally specialize the creating a UAV with the type of RWTexture2DArray since
            // an actual RWTextureCube doesn't exist in shading languages
            RZTextureDesc cubeMapTextureDesc         = {};
            cubeMapTextureDesc.name                  = "Texture.Imported.HDR.EnvCubeMap",
            cubeMapTextureDesc.width                 = CUBEMAP_DIM;
            cubeMapTextureDesc.height                = CUBEMAP_DIM;
            cubeMapTextureDesc.layers                = CUBEMAP_LAYERS;
            cubeMapTextureDesc.type                  = TextureType::kRWCubeMap;
            cubeMapTextureDesc.format                = TextureFormat::RGBA16F;
            cubeMapTextureDesc.initResourceViewHints = ResourceViewHint::kSRV | ResourceViewHint::kUAV;
            cubeMapTextureDesc.enableMips            = true;
            RZTextureHandle cubeMapHandle            = RZResourceManager::Get().createTexture(cubeMapTextureDesc);

            RZBufferDesc vplBufferDesc             = {};
            vplBufferDesc.name                     = "EnvMapUBOData";
            vplBufferDesc.size                     = sizeof(EnvMapGenUBOData);
            uboData.cubeFaceSize                   = {CUBEMAP_DIM, CUBEMAP_DIM};
            vplBufferDesc.data                     = &uboData;
            RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer(vplBufferDesc);

            // Load the shader
            auto shaderHandle       = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::EnvToCubemap);
            auto shader             = RZResourceManager::Get().getShaderResource(shaderHandle);
            auto descriptorHeapsMap = shader->getDescriptorsPerHeapMap();

            RZDescriptorSetHandle envMapSet = {};
            for (auto& heap: descriptorHeapsMap) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: heap.second) {
                    if (descriptor.name == "HDRTexture")
                        descriptor.texture = equirectangularMapHandle;
                    if (descriptor.name == "HDRSampler")
                        descriptor.sampler = Gfx::g_SamplerPresets[(u32) SamplerPresets::kDefaultGeneric];
                    if (descriptor.name == "CubeMapRT")
                        descriptor.texture = cubeMapHandle;
                    if (descriptor.name == "Constants")
                        descriptor.uniformBuffer = viewProjLayerUBO;
                }
                RZDescriptorSetDesc descSetCreateDesc = {};
                descSetCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;    // TODO: FUCK! Damn the samplers separate into different heap on shaders and CPU side
                descSetCreateDesc.name                = "DescriptorSet.EquirectangularToCubeMap";
                descSetCreateDesc.descriptors         = heap.second;
                envMapSet                             = RZResourceManager::Get().createDescriptorSet(descSetCreateDesc);
            }

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo = {};
            pipelineInfo.name                = "Pipeline.EnvMapGen";
            pipelineInfo.pipelineType        = PipelineType::kCompute;
            pipelineInfo.shader              = shaderHandle;
            RZPipelineHandle envMapPipeline  = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Begin rendering
            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("Environment Cubemap generation", glm::vec4(0.25f));
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Cubemap", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))

                RHI::BindPipeline(envMapPipeline, cmdBuffer);

                RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSet, BindingTable_System::SET_IDX_SYSTEM_START);
                RHI::Dispatch(cmdBuffer, CUBEMAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, CUBEMAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, CUBEMAP_LAYERS);
                RAZIX_MARK_END()
                RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            RZResourceManager::Get().destroyTexture(equirectangularMapHandle);

            //Generate mip maps from first mip face
            auto cubeMapTexture = RZResourceManager::Get().getTextureResource(cubeMapHandle);
            cubeMapTexture->GenerateMipsAndViews();

            RZResourceManager::Get().destroyDescriptorSet(envMapSet);
            RZResourceManager::Get().destroyPipeline(envMapPipeline);
            RZResourceManager::Get().destroyUniformBuffer(viewProjLayerUBO);

            return cubeMapHandle;
        }

        RZTextureHandle RZImageBasedLightingProbesManager::generateIrradianceMap(RZTextureHandle cubeMap)
        {
            RZTextureDesc irradianceMapTextureDesc         = {};
            irradianceMapTextureDesc.name                  = "Texture.IrradianceMap";
            irradianceMapTextureDesc.width                 = IRRADIANCE_MAP_DIM;
            irradianceMapTextureDesc.height                = IRRADIANCE_MAP_DIM;
            irradianceMapTextureDesc.layers                = CUBEMAP_LAYERS;
            irradianceMapTextureDesc.type                  = TextureType::kRWCubeMap;
            irradianceMapTextureDesc.format                = TextureFormat::RGBA16F;
            irradianceMapTextureDesc.enableMips            = false;
            irradianceMapTextureDesc.initResourceViewHints = ResourceViewHint::kSRV | ResourceViewHint::kUAV;

            RZTextureHandle irradianceMapHandle = RZResourceManager::Get().createTexture(irradianceMapTextureDesc);

            // Load the shader for converting plain cube map to irradiance map by convolution
            RZShaderHandle cubemapConvolutionShaderHandle = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GenerateIrradianceMap);
            auto           shader                         = RZResourceManager::Get().getShaderResource(cubemapConvolutionShaderHandle);
            auto           descriptorHeapsMap             = shader->getDescriptorsPerHeapMap();

            // Create the View Projection buffer
            RZDescriptorSet* envMapSet = nullptr;

            RZBufferDesc vplBufferDesc{};
            vplBufferDesc.name   = "ViewProjLayerUBOData";
            vplBufferDesc.size   = sizeof(EnvMapGenUBOData);
            uboData.cubeFaceSize = {IRRADIANCE_MAP_DIM, IRRADIANCE_MAP_DIM};
            vplBufferDesc.data   = &uboData;

            RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer(vplBufferDesc);

            for (auto& heap: descriptorHeapsMap) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: heap.second) {
                    if (descriptor.name == "EnvCubeMap")
                        descriptor.texture = cubeMap;
                    if (descriptor.name == "EnvCubeSampler")
                        descriptor.sampler = Gfx::g_SamplerPresets[(u32) SamplerPresets::kDefaultGeneric];
                    if (descriptor.name == "IrradianceMap")
                        descriptor.texture = irradianceMapHandle;
                    if (descriptor.name == "Constants")
                        descriptor.uniformBuffer = viewProjLayerUBO;
                }
                envMapSet = Gfx::RZDescriptorSet::Create(heap.second RZ_DEBUG_NAME_TAG_STR_E_ARG("generateIrradianceMap set"));
            }

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name               = "Pipeline.Irradiance";
            pipelineInfo.pipelineType       = PipelineType::kCompute;
            pipelineInfo.shader             = cubemapConvolutionShaderHandle;
            RZPipelineHandle envMapPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Begin rendering
            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("Irradiance Cubemap generation", glm::vec4(0.5f));
            {
                RHI::SetCmdBuffer(cmdBuffer);

                RAZIX_MARK_BEGIN("Irradiance cubemap Convolution", glm::vec4(0.8f, 0.4f, 0.3f, 1.0f))

                RHI::BindPipeline(envMapPipeline, cmdBuffer);

                RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSet, BindingTable_System::SET_IDX_SYSTEM_START);

                RHI::Dispatch(cmdBuffer, IRRADIANCE_MAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, IRRADIANCE_MAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, CUBEMAP_LAYERS);

                RAZIX_MARK_END()
                RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            if (envMapSet)
                envMapSet->Destroy();
            RZResourceManager::Get().destroyPipeline(envMapPipeline);
            RZResourceManager::Get().destroyUniformBuffer(viewProjLayerUBO);

            return irradianceMapHandle;
        }

        RZTextureHandle RZImageBasedLightingProbesManager::generatePreFilteredMap(RZTextureHandle cubeMap)
        {
            RZTextureDesc preFilteredMapTextureDesc{};
            preFilteredMapTextureDesc.name                  = "Texture.PreFilteredMap";
            preFilteredMapTextureDesc.width                 = PREFILTERED_MAP_DIM;
            preFilteredMapTextureDesc.height                = PREFILTERED_MAP_DIM;
            preFilteredMapTextureDesc.layers                = 6;
            preFilteredMapTextureDesc.type                  = TextureType::kRWCubeMap;
            preFilteredMapTextureDesc.format                = TextureFormat::RGBA16F;
            preFilteredMapTextureDesc.initResourceViewHints = ResourceViewHint::kSRV | ResourceViewHint::kUAV;
            preFilteredMapTextureDesc.enableMips            = true;
            RZTextureHandle preFilteredMapHandle            = RZResourceManager::Get().createTexture(preFilteredMapTextureDesc);
            RZTexture*      preFilteredMap                  = RZResourceManager::Get().getPool<RZTexture>().get(preFilteredMapHandle);

            // Load the shader for converting plain cube map to irradiance map by convolution
            RZShaderHandle cubemapConvolutionShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GeneratePreFilteredMap);
            auto           shader                   = RZResourceManager::Get().getShaderResource(cubemapConvolutionShader);
            auto           descriptorHeapsMap       = shader->getDescriptorsPerHeapMap();

            // Create the View Projection buffer
            std::vector<RZDescriptorSet*> PrefilteredMapPerMipHeaps = {};

            u32 MaxMipLevels = RZTexture::CalculateMipMapCount(PREFILTERED_MAP_DIM, PREFILTERED_MAP_DIM);

            for (u32 mipLevel = 0; mipLevel < MaxMipLevels; mipLevel++) {
                // Create a heap with different SRV/UAV attached
                preFilteredMap->setCurrentMipLevel(mipLevel);

                for (auto& heap: descriptorHeapsMap) {
                    // Fill the descriptors with buffers and textures
                    for (auto& descriptor: heap.second) {
                        if (descriptor.name == "EnvCubeMap")
                            descriptor.texture = cubeMap;
                        if (descriptor.name == "EnvCubeSampler")
                            descriptor.sampler = Gfx::g_SamplerPresets[(u32) SamplerPresets::kDefaultGeneric];
                        if (descriptor.name == "PreFilteredMap")
                            descriptor.texture = preFilteredMapHandle;
                    }
                    PrefilteredMapPerMipHeaps.push_back(Gfx::RZDescriptorSet::Create(heap.second RZ_DEBUG_NAME_TAG_STR_E_ARG("generatePreFiltered set" + std::to_string(mipLevel))));
                }
            }
            preFilteredMap->setCurrentMipLevel(0);

            // Create the Pipeline
            Gfx::RZPipelineDesc pipelineInfo = {};
            pipelineInfo.name                = "Pipeline.PreFilteredMap";
            pipelineInfo.pipelineType        = PipelineType::kCompute;
            pipelineInfo.shader              = cubemapConvolutionShader;
            RZPipelineHandle envMapPipeline  = RZResourceManager::Get().createPipeline(pipelineInfo);

            struct PushConstant
            {
                glm::ivec2 cubeFaceSize;
                float      roughness;
                u32        mipLevel;
            } data = {};

#define TEST_MIP_GEN_UPTO_MIPS 3

            // Begin rendering
            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("PreFiltering CubeMap", glm::vec4(0.6f, 0.8f, 0.3f, 1.0f));
            {
                for (u32 mipLevel = 0, mipSize = PREFILTERED_MAP_DIM; mipLevel < MaxMipLevels; mipLevel++, mipSize /= 2) {
                    // so we bind something fun here, we bind RW2DArraysViews but at different mip levels
                    // But Razix cannot create these views, it can currently make only 2D views....
                    // So we extend the ResourceHints to be more robust with some API functions to make it possible
                    // But until we come across another scenario we specialize this case, create them in backend for RWCubeMapArray type
                    // call it something like VkImageView m_PerMipArrayViews[NUM_MIPS] and bind it as needed

                    RHI::BindPipeline(envMapPipeline, cmdBuffer);
                    RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, PrefilteredMapPerMipHeaps[mipLevel], BindingTable_System::SET_IDX_SYSTEM_START);
                    // This example will use push constants
                    data.roughness    = (float) mipLevel / (float) (MaxMipLevels - 1);
                    data.mipLevel     = mipLevel;
                    data.cubeFaceSize = {mipSize, mipSize};
                    RZPushConstant pc = {};
                    pc.shaderStage    = ShaderStage::kCompute;
                    pc.data           = &data;
                    pc.size           = sizeof(PushConstant);
                    RHI::BindPushConstant(envMapPipeline, cmdBuffer, pc);
                    RHI::Dispatch(cmdBuffer, mipSize, mipSize, CUBEMAP_LAYERS);
                }
                RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            for (auto& set: PrefilteredMapPerMipHeaps) {
                if (set)
                    set->Destroy();
            }
            RZResourceManager::Get().destroyPipeline(envMapPipeline);

            return preFilteredMapHandle;
        }
    }    // namespace Gfx
}    // namespace Razix
