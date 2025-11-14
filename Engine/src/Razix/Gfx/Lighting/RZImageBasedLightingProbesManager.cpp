// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZImageBasedLightingProbesManager.h"

#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/Utils/RZLoadImage.h"

#include "Razix/Gfx/RZGfxUtil.h"
#include "Razix/Gfx/Resources/RZResourceManager.h"

#define GLM_FORCE_LEFT_HANDED
#include <glm/gtc/matrix_transform.hpp>

namespace Razix {
    namespace Gfx {

        // TODO: Use this via a RootConstant or PushConstant along with the texture bindless id
        struct PushConstant
        {
            int2  cubeFaceSize;
            float roughness;
            u32   mipLevel;
        } data = {};

        rz_gfx_texture_handle ConvertEquirectangularToCubemap(const RZString& hdrFilePath)
        {
            // This is only when we use a VS+PS to render to different layers of a RT (only Vulkan/AGC no HLSL support)
            //  --> https://www.reddit.com/r/vulkan/comments/mtx6ar/gl_layer_value_assigned_in_vertex_shader_but/
            //  --> https://www.reddit.com/r/vulkan/comments/xec0vg/multilayered_rendering_to_create_a_cubemap/
            // We are currently switched back to using VS+GS+PS and in future we will use a CS so no need of this
            // 12/21/2024: we have switched to using CS now, GS are inefficient and won't be good for dynamic CubeMaps

            // First create the 2D Equirectangular texture
            u32  width  = 0;
            u32  height = 0;
            u32  bpp    = 0;    // we only support 4 channels for HDR images
            f32* pixels = LoadImageDataFloat(hdrFilePath, &width, &height, &bpp);
            RAZIX_CORE_ASSERT(pixels != NULL, "[Lighting] Failed to load cubemap data from: {}", hdrFilePath);

            rz_gfx_texture_desc equiMapTextureDesc         = {};
            equiMapTextureDesc.resourceHints               = RZ_GFX_RESOURCE_VIEW_FLAG_SRV;
            equiMapTextureDesc.width                       = width;
            equiMapTextureDesc.height                      = height;
            equiMapTextureDesc.pPixelData                  = pixels;
            equiMapTextureDesc.depth                       = 1;
            equiMapTextureDesc.textureType                 = RZ_GFX_TEXTURE_TYPE_2D;
            equiMapTextureDesc.format                      = RZ_GFX_FORMAT_R32G32B32A32_FLOAT;
            equiMapTextureDesc.mipLevels                   = 1;    // No mips for Equirectangular maps
            rz_gfx_texture_handle equirectangularMapHandle = RZResourceManager::Get().createTexture("HDRCubeMapTexture", equiMapTextureDesc);

            // Since it has both UAV and SRV, backend API will internally specialize the creating a UAV with the type of RWTexture2DArray since
            // an actual RWTextureCube doesn't exist in HLSL and most shading languages
            rz_gfx_texture_desc cubeMapTextureDesc = {};
            cubeMapTextureDesc.resourceHints       = (rz_gfx_resource_view_hints) (RZ_GFX_RESOURCE_VIEW_FLAG_SRV | RZ_GFX_RESOURCE_VIEW_FLAG_UAV);
            cubeMapTextureDesc.width               = CUBEMAP_DIM;
            cubeMapTextureDesc.height              = CUBEMAP_DIM;
            cubeMapTextureDesc.arraySize           = CUBEMAP_LAYERS;
            cubeMapTextureDesc.textureType         = RZ_GFX_TEXTURE_TYPE_CUBE;
            cubeMapTextureDesc.format              = RZ_GFX_FORMAT_R16G16B16A16_FLOAT;
            cubeMapTextureDesc.mipLevels           = rzRHI_GetMipLevelCount(CUBEMAP_DIM, CUBEMAP_DIM);
            rz_gfx_texture_handle cubeMapHandle    = RZResourceManager::Get().createTexture("Texture.Imported.HDR.EnvCubeMap", cubeMapTextureDesc);

            // Create the shader and pipeline
            rz_gfx_shader_handle envMapShaderHandle = {};    // = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::EquirectangularToCubemap);

            rz_gfx_pipeline_desc pipelineDesc     = {};
            pipelineDesc.type                     = RZ_GFX_PIPELINE_TYPE_COMPUTE;
            pipelineDesc.pShader                  = RZResourceManager::Get().getShaderResource(envMapShaderHandle);
            pipelineDesc.pRootSig                 = RZResourceManager::Get().getRootSignatureResource(pipelineDesc.pShader->rootSignature);
            rz_gfx_pipeline_handle envMapPipeline = RZResourceManager::Get().createPipeline("Pipeline.EnvMapConversion", pipelineDesc);

            auto cmdBuffer = Gfx::BeginSingleTimeCommandBuffer(RAZIX_CMD_MARKER_NAME_COLOR("ConvEquiToCubemap"));
            {
                RAZIX_PROFILE_SCOPE("ConvEquiToCubemap");
                RAZIX_MARK_BEGIN(cmdBuffer, "ConvEquiToCubemap", GenerateHashedColor4(RZString("ConvEquiToCubemap").hash()));

                rzRHI_BindPipeline(cmdBuffer, envMapPipeline);

                //rzRHI_BindDescriptorHeaps();
                //rzRHI_BindDescriptorTables(envMapPipeline, cmdBuffer, envMapSet, BindingTable_System::SET_IDX_SYSTEM_START);

                rzRHI_Dispatch(cmdBuffer, CUBEMAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, CUBEMAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, CUBEMAP_LAYERS);

                RAZIX_MARK_END(cmdBuffer);

                Gfx::EndSingleTimeCommandBuffer(cmdBuffer);
            }
            RZResourceManager::Get().destroyTexture(equirectangularMapHandle);

            //Generate mip maps from first mip face
            auto cubeMapTexture = RZResourceManager::Get().getTextureResource(cubeMapHandle);
            //cubeMapTexture->GenerateMipsAndViews();
            cmdBuffer = Gfx::BeginSingleTimeCommandBuffer(RAZIX_CMD_MARKER_NAME_COLOR("GenCubemapMipMaps"));
            {
                RAZIX_PROFILE_SCOPE("GenCubemapMipMaps");
                RAZIX_MARK_BEGIN(cmdBuffer, "GenCubemapMipMaps", GenerateHashedColor4(RZString("GenCubemapMipMaps").hash()));

                rzRHI_GenerateMipmaps(cmdBuffer, cubeMapHandle);

                RAZIX_MARK_END(cmdBuffer);

                Gfx::EndSingleTimeCommandBuffer(cmdBuffer);
            }

            //RZResourceManager::Get().destroyDescriptorTable(envMapSet);
            //RZResourceManager::Get().destroyDescriptorHeap();
            RZResourceManager::Get().destroyPipeline(envMapPipeline);

            return cubeMapHandle;
        }

#if 0
        rz_texture_handle RZImageBasedLightingProbesManager::generateIrradianceMap(rz_texture_handle cubeMap)
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
            rz_texture_handle irradianceMapHandle            = RZResourceManager::Get().createTexture(irradianceMapTextureDesc);

            RZBufferDesc vplBufferDesc             = {};
            vplBufferDesc.name                     = "ViewProjLayerUBOData";
            vplBufferDesc.size                     = sizeof(EnvMapGenUBOData);
            uboData.cubeFaceSize                   = {IRRADIANCE_MAP_DIM, IRRADIANCE_MAP_DIM};
            vplBufferDesc.data                     = &uboData;
            vplBufferDesc.initResourceViewHints    = kCBV;
            RZUniformBufferHandle viewProjLayerUBO = RZResourceManager::Get().createUniformBuffer(vplBufferDesc);

            RZShaderHandle cubemapConvolutionShaderHandle = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GenerateIrradianceMap);
            auto           shader                         = RZResourceManager::Get().getShaderResource(cubemapConvolutionShaderHandle);
            auto           descriptorHeapsMap             = shader->getDescriptorsPerHeapMap();

            RZDescriptorSetHandle envMapSet = {};
            for (auto& heap: descriptorHeapsMap) {
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
                RZDescriptorSetDesc descSetCreateDesc = {};
                descSetCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                descSetCreateDesc.name                = "DescriptorSet.DiffuseIrradianceMap";
                descSetCreateDesc.descriptors         = heap.second;
                envMapSet                             = RZResourceManager::Get().createDescriptorSet(descSetCreateDesc);
            }

            Gfx::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name               = "Pipeline.Irradiance";
            pipelineInfo.pipelineType       = PipelineType::kCompute;
            pipelineInfo.shader             = cubemapConvolutionShaderHandle;
            RZPipelineHandle envMapPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("Irradiance Cubemap generation", float4(0.5f));
            {
                RHI::BindPipeline(envMapPipeline, cmdBuffer);
                RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, envMapSet, BindingTable_System::SET_IDX_SYSTEM_START);
                RHI::Dispatch(cmdBuffer, IRRADIANCE_MAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, IRRADIANCE_MAP_DIM / IBL_DISPATCH_THREAD_GROUP_DIM, CUBEMAP_LAYERS);
            }
            RZDrawCommandBuffer::EndSingleTimeCommandBuffer(cmdBuffer);

            RZResourceManager::Get().destroyUniformBuffer(viewProjLayerUBO);
            RZResourceManager::Get().destroyDescriptorSet(envMapSet);
            RZResourceManager::Get().destroyPipeline(envMapPipeline);

            return irradianceMapHandle;
        }

        rz_texture_handle RZImageBasedLightingProbesManager::generatePreFilteredMap(rz_texture_handle cubeMap)
        {
            RZTextureDesc preFilteredMapTextureDesc{};
            preFilteredMapTextureDesc.name                  = "Texture.PreFilteredMap";
            preFilteredMapTextureDesc.width                 = PREFILTERED_MAP_DIM;
            preFilteredMapTextureDesc.height                = PREFILTERED_MAP_DIM;
            preFilteredMapTextureDesc.layers                = CUBEMAP_LAYERS;
            preFilteredMapTextureDesc.type                  = TextureType::kRWCubeMap;
            preFilteredMapTextureDesc.format                = TextureFormat::RGBA16F;
            preFilteredMapTextureDesc.initResourceViewHints = ResourceViewHint::kSRV | ResourceViewHint::kUAV;
            preFilteredMapTextureDesc.enableMips            = true;
            rz_texture_handle preFilteredMapHandle            = RZResourceManager::Get().createTexture(preFilteredMapTextureDesc);
            RZTexture*      preFilteredMap                  = RZResourceManager::Get().getPool<RZTexture>().get(preFilteredMapHandle);

            RZShaderHandle cubemapConvolutionShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GeneratePreFilteredMap);
            auto           shader                   = RZResourceManager::Get().getShaderResource(cubemapConvolutionShader);
            auto           descriptorHeapsMap       = shader->getDescriptorsPerHeapMap();

            const u32 MaxMipLevels = RZTexture::CalculateMipMapCount(PREFILTERED_MAP_DIM, PREFILTERED_MAP_DIM);

            RZDynamicArray<RZDescriptorSetHandle> PrefilteredMapPerMipHeaps = {};
            for (u32 mipLevel = 0; mipLevel < MaxMipLevels; mipLevel++) {
                // Create a heap with different SRV/UAV attached
                preFilteredMap->setCurrentMipLevel(mipLevel);

                for (auto& heap: descriptorHeapsMap) {
                    for (auto& descriptor: heap.second) {
                        if (descriptor.name == "EnvCubeMap")
                            descriptor.texture = cubeMap;
                        if (descriptor.name == "EnvCubeSampler")
                            descriptor.sampler = Gfx::g_SamplerPresets[(u32) SamplerPresets::kDefaultGeneric];
                        if (descriptor.name == "PreFilteredMap")
                            descriptor.texture = preFilteredMapHandle;
                    }
                    RZDescriptorSetDesc descSetCreateDesc = {};
                    descSetCreateDesc.heapType            = DescriptorHeapType::kCbvUavSrvHeap;
                    descSetCreateDesc.name                = "DescriptorSet.SpecularPreFiltered";
                    descSetCreateDesc.descriptors         = heap.second;
                    auto preFiltSet                       = RZResourceManager::Get().createDescriptorSet(descSetCreateDesc);
                    PrefilteredMapPerMipHeaps.push_back(preFiltSet);
                }
            }
            preFilteredMap->setCurrentMipLevel(0);

            Gfx::RZPipelineDesc pipelineInfo = {};
            pipelineInfo.name                = "Pipeline.PreFilteredMap";
            pipelineInfo.pipelineType        = PipelineType::kCompute;
            pipelineInfo.shader              = cubemapConvolutionShader;
            RZPipelineHandle envMapPipeline  = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto cmdBuffer = RZDrawCommandBuffer::BeginSingleTimeCommandBuffer("PreFiltering CubeMap", float4(0.6f, 0.8f, 0.3f, 1.0f));
            {
                for (u32 mipLevel = 0, mipSize = PREFILTERED_MAP_DIM; mipLevel < MaxMipLevels; mipLevel++, mipSize /= 2) {
                    // so we bind something fun here, we bind RW2DArraysViews but at different mip levels
                    // But Razix cannot create these views, it can currently make only 2D views....
                    // So we extend the ResourceHints to be more robust with some API functions to make it possible
                    // But until we come across another scenario we specialize this case, create them in backend for RWCubeMapArray type
                    // call it something like VkImageView m_PerMipArrayViews[NUM_MIPS] and bind it as needed

                    RHI::BindPipeline(envMapPipeline, cmdBuffer);
                    RHI::BindDescriptorSet(envMapPipeline, cmdBuffer, PrefilteredMapPerMipHeaps[mipLevel], BindingTable_System::SET_IDX_SYSTEM_START);
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

            for (auto& set: PrefilteredMapPerMipHeaps)
                if (set.isValid())
                    RZResourceManager::Get().destroyDescriptorSet(set);
            RZResourceManager::Get().destroyPipeline(envMapPipeline);

            return preFilteredMapHandle;
        }
#endif
    }    // namespace Gfx
}    // namespace Razix
