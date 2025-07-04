// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZBloomPass.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/Materials/RZMaterial.h"

#include "Razix/Gfx/Renderers/RZWorldRenderer.h"

#include "Razix/Gfx/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Gfx/RHI/API/RZTexture.h"
#include "Razix/Gfx/RHI/API/RZUniformBuffer.h"
#include "Razix/Gfx/RHI/API/RZVertexBuffer.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Gfx/Passes/Data/GlobalData.h"

namespace Razix {
    namespace Gfx {

#if 0
void RZBloomPass::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            SceneData forwardSceneData = framegraph.getBlackboard().get<SceneData>();

            auto& bloomMipData = framegraph.getBlackboard().add<BloomPassData>();

            // Create GPU resources
            auto upsamplingShader   = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);
            auto downsamplingShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);

            RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Bloom Upscale Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA16F};

            pipelineInfo.shader = upsamplingShader;
            m_UpsamplePipeline  = RZResourceManager::Get().createPipeline(pipelineInfo);

            pipelineInfo.name    = "Bloom Downsample Pipeline";
            pipelineInfo.shader  = downsamplingShader;
            m_DownsamplePipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            // Init the mesh
            m_ScreenQuadMesh = Graphics::MeshFactory::CreatePrimitive(Razix::Gfx::MeshPrimitive::ScreenQuad);

            // Do a bunch of Down Sampling and Up Samplings on the scene Texture
            // Start with down sampling, use the Source texture as the first mip
            BloomMip sourceMip{-1};
            sourceMip.mip  = forwardSceneData.SceneHDR;
            sourceMip.size = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
            for (u32 i = 0; i < NUM_BLOOM_MIPS; ++i)
                sourceMip = downsample(framegraph, sourceMip, scene, i);

            for (u32 i = 0; i < NUM_BLOOM_MIPS; ++i)
                sourceMip = upsample(framegraph, sourceMip, scene, i, settings);

            bloomMipData.bloomTexture = sourceMip.mip;

            mixScene(framegraph, scene, settings);
        }

        void RZBloomPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD;
        }

        BloomMip RZBloomPass::upsample(RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene, u32 mipindex, RZRendererSettings* settings)
        {
            const auto name = "Bloom Upsample pass #" + std::to_string(mipindex);

            BloomMip mip;

            struct BloomSubPassData
            {
                RZFrameGraphResource bloomMip;
            };

            for (u32 j = 0; j < RAZIX_MAX_SWAP_IMAGES_COUNT; j++) {
                auto cmdBuffer = RZDrawCommandBuffer::Create();
                cmdBuffer->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Bloom Upsample cmd buffer [mip idx :" + std::to_string(mipindex) + "]"));
                upsamplebBloomGpuResources[mipindex].cmdBuffers.push_back(cmdBuffer);
            }

            auto upsamplingShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);
            auto setInfos         = RZResourceManager::Get().getShaderResource(upsamplingShader)->getDescriptorsPerHeapMap();
            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined)
                        descriptor.texture = RZMaterial::GetDefaultTexture();
                }
                upsamplebBloomGpuResources[mipindex].bloomDescSet.push_back(Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Bloom Upsample Set")));
            }

            auto& pass = framegraph.addCallbackPass<BloomSubPassData>(
                name.c_str(),
                [&](BloomSubPassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc bloomMipDesc{
                        .name   = "Bloom Mip",
                        .width  = static_cast<u32>(bloomSourceMip.size.x),
                        .height = static_cast<u32>(bloomSourceMip.size.y),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA16F,
                    };

                    data.bloomMip = builder.create<RZFrameGraphTexture>("Bloom Mip", CAST_TO_FG_TEX_DESC bloomMipDesc);

                    // Read the mip from the previous pass
                    builder.read(bloomSourceMip.mip);
                    data.bloomMip = builder.write(data.bloomMip);
                },
                [=](const BloomSubPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    /**
                     * Since the resource is cloned and we maintain different version of it, the same resource is handled to us every time we ask it
                     */

                    auto cmdBuf = upsamplebBloomGpuResources[mipindex].cmdBuffers[Graphics::RHI::GetSwapchain()->getCurrentFrameIndex()];

                    // Begin Command Buffer Recording
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Bloom Upsample Pass" + std::to_string(mipindex), float4(0.25, 0.23, 0.86f, 1.0f));

                    // Update the Descriptor Set with the new texture once
                    static bool updatedRT     = false;
                    static i32  mipIdxTracker = 0;
                    if (!updatedRT) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default))->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                descriptor.texture = resources.get<RZFrameGraphTexture>(bloomSourceMip.mip).getHandle();
                            }
                            upsamplebBloomGpuResources[mipindex].bloomDescSet[0]->UpdateSet(setInfo.second);
                        }
                        ++mipIdxTracker;
                        if (mipIdxTracker >= NUM_BLOOM_MIPS)
                            updatedRT = true;
                    }

                    // Begin Rendering
                    RenderingInfo info{};
                    info.colorAttachments = {{resources.get<RZFrameGraphTexture>(data.bloomMip).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.extent           = {bloomSourceMip.size.x, bloomSourceMip.size.y};
                    info.resize           = false;
                    RHI::BeginRendering(cmdBuf, info);

                    RHI::BindPipeline(m_UpsamplePipeline, cmdBuf);

                    // Bind the descriptor sets
                    Graphics::RHI::BindUserDescriptorSets(m_UpsamplePipeline, cmdBuf, upsamplebBloomGpuResources[mipindex].bloomDescSet);

                    //-----------------------------
                    // Get the shader from the Mesh Material later
                    // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                    auto& upsampleData = RZResourceManager::Get().getShaderResource(Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default))->getPushConstants()[0];

                    struct PCD
                    {
                        f32 filterRadius;
                    } pcData{};
                    pcData.filterRadius = settings->bloomConfig.radius;
                    upsampleData.data   = &pcData;
                    upsampleData.size   = sizeof(PCD);

                    // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                    Graphics::RHI::BindPushConstant(m_UpsamplePipeline, cmdBuf, upsampleData);
                    //-----------------------------

                    m_ScreenQuadMesh->Draw(cmdBuf);

                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();
                    RHI::Submit(cmdBuf);
                    Graphics::RHI::SubmitWork({}, {});
                });

            mip.mip  = pass.bloomMip;
            mip.size = float2(bloomSourceMip.size.x, bloomSourceMip.size.y) * 2.0f;
            return mip;
        }

        BloomMip RZBloomPass::downsample(RZFrameGraph& framegraph, BloomMip bloomSourceMip, Razix::RZScene* scene, u32 mipindex)
        {
            const auto name = "Bloom Downsample pass #" + std::to_string(mipindex);

            BloomMip mip;

            struct BloomSubPassData
            {
                RZFrameGraphResource bloomMip;
            };

            for (u32 j = 0; j < RAZIX_MAX_SWAP_IMAGES_COUNT; j++) {
                auto cmdBuffer = RZDrawCommandBuffer::Create();
                cmdBuffer->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Bloom Downsample cmd buffer [mip idx :" + std::to_string(mipindex) + "]"));
                downsamplebBloomGpuResources[mipindex].cmdBuffers.push_back(cmdBuffer);
            }

            auto downsamplingShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);
            auto setInfos           = RZResourceManager::Get().getShaderResource(downsamplingShader)->getDescriptorsPerHeapMap();
            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined)
                        descriptor.texture = RZMaterial::GetDefaultTexture();
                }
                downsamplebBloomGpuResources[mipindex].bloomDescSet.push_back(Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Bloom Downsample Set")));
            }

            auto& pass = framegraph.addCallbackPass<BloomSubPassData>(
                name.c_str(),
                [&](BloomSubPassData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc bloomMipDesc{
                        .name   = "Bloom Mip",
                        .width  = static_cast<u32>(bloomSourceMip.size.x),
                        .height = static_cast<u32>(bloomSourceMip.size.y),
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA16F,
                    };

                    data.bloomMip = builder.create<RZFrameGraphTexture>("Bloom Mip", CAST_TO_FG_TEX_DESC bloomMipDesc);

                    // Read the mip from the previous pass
                    builder.read(bloomSourceMip.mip);
                    data.bloomMip = builder.write(data.bloomMip);
                },
                [=](const BloomSubPassData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    /**
                     * Since the resource is cloned and we maintain different version of it, the same resource is handled to us every time we ask it
                     */

                    auto cmdBuf = downsamplebBloomGpuResources[mipindex].cmdBuffers[Graphics::RHI::GetSwapchain()->getCurrentFrameIndex()];

                    // Begin Command Buffer Recording
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Bloom Downsample Pass" + std::to_string(mipindex), float4(0.85, 0.23, 0.56f, 1.0f));

                    // Update the Descriptor Set with the new texture once
                    static bool updatedRT     = false;
                    static i32  mipIdxTracker = 0;
                    if (!updatedRT) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default))->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                descriptor.texture = resources.get<RZFrameGraphTexture>(bloomSourceMip.mip).getHandle();
                            }
                            downsamplebBloomGpuResources[mipindex].bloomDescSet[0]->UpdateSet(setInfo.second);
                        }
                        ++mipIdxTracker;
                        if (mipIdxTracker >= NUM_BLOOM_MIPS)
                            updatedRT = true;
                    }

                    // Begin Rendering
                    RenderingInfo info{};
                    info.colorAttachments = {{resources.get<RZFrameGraphTexture>(data.bloomMip).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.extent           = {bloomSourceMip.size.x, bloomSourceMip.size.y};
                    info.resize           = false;
                    RHI::BeginRendering(cmdBuf, info);

                    RHI::BindPipeline(m_DownsamplePipeline, cmdBuf);

                    // Bind the descriptor sets
                    Graphics::RHI::BindUserDescriptorSets(m_DownsamplePipeline, cmdBuf, downsamplebBloomGpuResources[mipindex].bloomDescSet);

                    //-----------------------------
                    // Get the shader from the Mesh Material later
                    // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                    auto& downsampleData = RZResourceManager::Get().getShaderResource(Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default))->getPushConstants()[0];

                    struct PCD
                    {
                        float2 resoulution{};
                    } pcData{};
                    pcData.resoulution  = info.extent;
                    downsampleData.data = &pcData;
                    downsampleData.size = sizeof(PCD);

                    // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                    Graphics::RHI::BindPushConstant(m_DownsamplePipeline, cmdBuf, downsampleData);
                    //-----------------------------

                    m_ScreenQuadMesh->Draw(cmdBuf);

                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();
                    RHI::Submit(cmdBuf);
                    Graphics::RHI::SubmitWork({}, {});
                });

            mip.mip  = pass.bloomMip;
            mip.size = float2(bloomSourceMip.size.x, bloomSourceMip.size.y) * 0.5f;
            return mip;
        }

        void RZBloomPass::mixScene(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            for (u32 j = 0; j < RAZIX_MAX_SWAP_IMAGES_COUNT; j++) {
                auto cmdBuffer = RZDrawCommandBuffer::Create();
                cmdBuffer->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Bloom Scene Mix cmd buffer"));
                bloomSceneMixGpuResources.cmdBuffers.push_back(cmdBuffer);
            }

            auto downsamplingShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);
            auto setInfos           = RZResourceManager::Get().getShaderResource(downsamplingShader)->getDescriptorsPerHeapMap();
            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::ImageSamplerCombined)
                        descriptor.texture = RZMaterial::GetDefaultTexture();
                }
                bloomSceneMixGpuResources.bloomDescSet.push_back(Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Bloom mix Set")));
            }

            auto bloomMixShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default);

            RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Bloom Mix Tonemapper Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = false;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA8};
            pipelineInfo.shader                 = bloomMixShader;
            m_HDRBloomMixPipeline               = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto&     bloomData        = framegraph.getBlackboard().get<BloomPassData>();
            SceneData forwardSceneData = framegraph.getBlackboard().get<SceneData>();

            struct TonemapData
            {
                RZFrameGraphResource ldrOutput;
            };

            framegraph.addCallbackPass<TonemapData>(
                "Bloom Mix Tonemapping pass",
                [&](TonemapData& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(bloomData.bloomTexture);
                    builder.read(forwardSceneData.SceneHDR);
                    builder.read(forwardSceneData.sceneLDR);

                    //data.ldrOutput = builder.write(data.ldrOutput);
                },
                [=](const TonemapData& data, RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto cmdBuf = bloomSceneMixGpuResources.cmdBuffers[RHI::GetSwapchain()->getCurrentFrameIndex()];

                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Bloom Mix Tonemap", float4(0.05, 0.83, 0.66f, 1.0f));

                    // Update the Descriptor Set with the new texture once
                    static bool updatedRT = false;
                    if (!updatedRT) {
                        auto setInfos = RZResourceManager::Get().getShaderResource(Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default))->getDescriptorsPerHeapMap();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                if (descriptor.bindingInfo.location.binding == 0)
                                    descriptor.texture = resources.get<RZFrameGraphTexture>(forwardSceneData.SceneHDR).getHandle();
                                else if (descriptor.bindingInfo.location.binding == 1)
                                    descriptor.texture = resources.get<RZFrameGraphTexture>(bloomData.bloomTexture).getHandle();
                            }
                            bloomSceneMixGpuResources.bloomDescSet[0]->UpdateSet(setInfo.second);
                        }
                        updatedRT = true;
                    }

                    // Begin Rendering
                    RenderingInfo info{
                        .extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()},
                        .colorAttachments = {{resources.get<RZFrameGraphTexture>(forwardSceneData.sceneLDR).getHandle(), {true, ClearColorPresets::OpaqueBlack}}},
                        .resize           = true};
                    RHI::BeginRendering(cmdBuf, info);

                    RHI::BindPipeline(m_HDRBloomMixPipeline, cmdBuf);

                    // Bind the descriptor sets
                    Graphics::RHI::BindUserDescriptorSets(m_HDRBloomMixPipeline, cmdBuf, bloomSceneMixGpuResources.bloomDescSet);

                    //-----------------------------
                    // Get the shader from the Mesh Material later
                    // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                    auto& bloomData = RZResourceManager::Get().getShaderResource(Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Default))->getPushConstants()[0];

                    struct PCD
                    {
                        float bloomStrength;
                        u32   toneMapMode;
                    } pcData{};
                    pcData.bloomStrength = RZEngine::Get().getWorldSettings().bloomConfig.strength;
                    pcData.toneMapMode   = settings->tonemapMode;
                    bloomData.data       = &pcData;
                    bloomData.size       = sizeof(PCD);

                    // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                    Graphics::RHI::BindPushConstant(m_HDRBloomMixPipeline, cmdBuf, bloomData);
                    //-----------------------------

                    m_ScreenQuadMesh->Draw(cmdBuf);

                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();
                    RHI::Submit(cmdBuf);
                    Graphics::RHI::SubmitWork({}, {});
                });
        }
#endif
    }    // namespace Gfx
}    // namespace Razix