// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCascadedShadowsRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZCascadedShadowsRenderer::Init()
        {
            // Init the resources (API & Frame Graph) needed for Cascaded Shadow Mapping
            // Import the ShadowMapData as a Buffer into the FrameGraph (used to upload to GPU later)
            m_CascadedMatricesUBO = RZResourceManager::Get().createUniformBuffer({"Cascades UBO", sizeof(CasdacesUBOData), nullptr});
        }

        void RZCascadedShadowsRenderer::Begin(RZScene* scene)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZCascadedShadowsRenderer::Draw(RZCommandBuffer* cmdBuffer)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZCascadedShadowsRenderer::End()
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZCascadedShadowsRenderer::Resize(u32 width, u32 height)
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void RZCascadedShadowsRenderer::Destroy()
        {
            RZResourceManager::Get().destroyUniformBuffer(m_CascadedMatricesUBO);
            for (u32 i = 0; i < kNumCascades; i++) {
                RZResourceManager::Get().destroyUniformBuffer(cascadeGPUResources[i].ViewProjLayerUBO);
                for (auto& set: cascadeGPUResources[i].CascadeVPSet)
                    set->Destroy();
                RZResourceManager::Get().destroyPipeline(cascadeGPUResources[i].CascadePassPipeline);
            }
        }

        //--------------------------------------------------------------------------

        void RZCascadedShadowsRenderer::addPass(FrameGraph::RZFrameGraph& framegraph,  Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto& shadowMapData = framegraph.getBlackboard().add<ShadowMapData>();
            // Import the cascades buffer into the FG
            RZBufferDesc desc{};
            desc.name                      = "Cascades UBO";
            desc.size                      = sizeof(CasdacesUBOData);
            shadowMapData.viewProjMatrices = framegraph.import <FrameGraph::RZFrameGraphBuffer>("Cascade Matrices", CAST_TO_FG_BUF_DESC desc, {m_CascadedMatricesUBO});

            // Build Cascades
            m_Cascades = buildCascades(scene->getSceneCamera(), glm::vec3(1.0f), kNumCascades, 0.94f, kShadowMapSize);

            FrameGraph::RZFrameGraphResource cascaseShadowMaps{-1};
            for (u32 i = 0; i < m_Cascades.size(); i++) {
                //const glm::mat4& lightViewProj = cascades[i].viewProjMatrix;
                cascaseShadowMaps = addCascadePass(framegraph, cascaseShadowMaps, scene, i);
            }
            shadowMapData.cascadedShadowMaps = cascaseShadowMaps;

            // Since the above texture passes are cascaded we do an extra pass to constantly update the data into a buffer after all the cascade calculations are done whilst filling the TextureArray2D
            framegraph.addCallbackPass(
                "Upload Cascade Matrices (post CSM calculation)",
                [&](auto&, FrameGraph::RZPassResourceBuilder& builder) {
                    shadowMapData.viewProjMatrices = builder.write(shadowMapData.viewProjMatrices);
                },
                [=](const auto&, FrameGraph::RZPassResourceDirectory& resources) {
                    CasdacesUBOData data{};
                    for (u32 i{0}; i < m_Cascades.size(); ++i) {
                        data.splitDepth[i]       = m_Cascades[i].splitDepth;
                        data.viewProjMatrices[i] = m_Cascades[i].viewProjMatrix;
                    }
                    //m_CascadedMatricesUBO->SetData(sizeof(CasdacesUBOData), &data);
                });
        }

        void RZCascadedShadowsRenderer::updateCascades(RZScene* scene)
        {
            auto dirLight = scene->GetComponentsOfType<LightComponent>();
            m_Cascades    = buildCascades(scene->getSceneCamera(), -dirLight[0].light.getPosition(), kNumCascades, 0.95f, kShadowMapSize);
        }

        //--------------------------------------------------------------------------

        std::vector<Cascade> RZCascadedShadowsRenderer::buildCascades(RZSceneCamera camera, glm::vec3 dirLightDirection, u32 numCascades, f32 lambda, u32 shadowMapSize)
        {
            // [Reference] https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/

            const f32 clipRange = camera.getPerspectiveFarClip() - camera.getPerspectiveNearClip();
            // Get the cascade splits
            const auto cascadeSplits  = buildCascadeSplits(numCascades, lambda, camera.getPerspectiveNearClip(), clipRange);
            const auto invViewProjRaw = camera.getProjectionRaw() * camera.getViewMatrix();
            const auto invViewProj    = (camera.getProjection() * camera.getViewMatrix());

            auto lastSplitDist = 0.0f;

            std::vector<Cascade> cascades(numCascades);
            for (u32 i{0}; i < cascades.size(); ++i) {
                const auto splitDist = cascadeSplits[i];

                cascades[i] = {
                    (camera.getPerspectiveNearClip() + splitDist * clipRange) * -1.0f,
                    buildDirLightMatrix(invViewProjRaw, dirLightDirection, shadowMapSize, splitDist, lastSplitDist),
                };
                lastSplitDist = splitDist;
            }
            return cascades;
        }

        std::vector<f32> RZCascadedShadowsRenderer::buildCascadeSplits(u32 numCascades, f32 lambda, f32 nearPlane, f32 clipRange)
        {
            constexpr auto kMinDistance = 0.0f, kMaxDistance = 1.0f;

            const auto minZ = nearPlane + kMinDistance * clipRange;
            const auto maxZ = nearPlane + kMaxDistance * clipRange;

            const auto range = maxZ - minZ;
            const auto ratio = maxZ / minZ;

            std::vector<f32> cascadeSplits(numCascades);
            // Calculate split depths based on view camera frustum
            // Based on method presented in:
            // https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
            for (u32 i{0}; i < cascadeSplits.size(); ++i) {
                const auto p       = static_cast<f32>(i + 1) / cascadeSplits.size();
                const auto log     = minZ * std::pow(ratio, p);
                const auto uniform = minZ + range * p;
                const auto d       = lambda * (log - uniform) + uniform;
                cascadeSplits[i]   = (d - nearPlane) / clipRange;
            }
            return cascadeSplits;
        }

        //--------------------------------------------------------------------------

        FrustumCorners RZCascadedShadowsRenderer::buildFrustumCorners(const glm::mat4& inversedViewProj, f32 splitDist, f32 lastSplitDist)
        {
            // clang-format off
            FrustumCorners frustumCorners{
                // Near
                glm::vec3{-1.0f,  1.0f, -1.0f}, // TL
                glm::vec3{ 1.0f,  1.0f, -1.0f}, // TR
                glm::vec3{ 1.0f, -1.0f, -1.0f}, // BR
                glm::vec3{-1.0f, -1.0f, -1.0f}, // BL
                // Far
                glm::vec3{-1.0f,  1.0f,  1.0f}, // TL
                glm::vec3{ 1.0f,  1.0f,  1.0f}, // TR
                glm::vec3{ 1.0f, -1.0f,  1.0f}, // BR
                glm::vec3{-1.0f, -1.0f,  1.0f}  // BL
            };
            // clang-format on

            // Project frustum corners into world space
            for (auto& p: frustumCorners) {
                const auto temp = inversedViewProj * glm::vec4{p, 1.0f};
                p               = glm::vec3{temp} / temp.w;
            }
            for (u32 i{0}; i < 4; ++i) {
                const auto cornerRay     = frustumCorners[i + 4] - frustumCorners[i];
                const auto farCornerRay  = cornerRay * splitDist;
                frustumCorners[i + 4]    = frustumCorners[i] + farCornerRay;
                const auto nearCornerRay = cornerRay * lastSplitDist;
                frustumCorners[i]        = frustumCorners[i] + nearCornerRay;
            }
            return frustumCorners;
        }

        auto RZCascadedShadowsRenderer::measureFrustum(const FrustumCorners& frustumCorners)
        {
            glm::vec3 center{0.0f};
            for (const auto& p: frustumCorners)
                center += p;
            center /= frustumCorners.size();

            auto radius = 0.0f;
            for (const auto& p: frustumCorners) {
                const auto distance = glm::length(p - center);
                radius              = glm::max(radius, distance);
            }
            radius = glm::ceil(radius * 16.0f) / 16.0f;
            return std::make_tuple(center, radius);
        }

        void RZCascadedShadowsRenderer::eliminateShimmering(glm::mat4& projection, const glm::mat4& view, u32 shadowMapSize)
        {
            auto shadowOrigin = projection * view * glm::vec4{glm::vec3{0.0f}, 1.0f};
            shadowOrigin *= (static_cast<f32>(shadowMapSize) / 2.0f);

            const auto roundedOrigin = glm::round(shadowOrigin);
            auto       roundOffset   = roundedOrigin - shadowOrigin;
            roundOffset              = roundOffset * 2.0f / static_cast<f32>(shadowMapSize);
            roundOffset.z            = 0.0f;
            roundOffset.w            = 0.0f;
            projection[3] += roundOffset;
        }

        glm::mat4 RZCascadedShadowsRenderer::buildDirLightMatrix(const glm::mat4& inversedViewProj, const glm::vec3& lightDirection, u32 shadowMapSize, f32 splitDist, f32 lastSplitDist)
        {
            const auto frustumCorners   = buildFrustumCorners(inversedViewProj, splitDist, lastSplitDist);
            const auto [center, radius] = measureFrustum(frustumCorners);

            const auto maxExtents = glm::vec3{radius};
            const auto minExtents = -maxExtents;

            const auto eye        = center - glm::normalize(lightDirection) * -minExtents.z;
            const auto view       = glm::lookAt(eye, center, {0.0f, 1.0f, 0.0f});
            auto       projection = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, (maxExtents - minExtents).z);
            //auto projection = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, minExtents.z, maxExtents.z);

            eliminateShimmering(projection, view, shadowMapSize);
            //if (Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::VULKAN)
            //    projection[1][1] *= -1;
            return projection * view;
        }

        FrameGraph::RZFrameGraphResource RZCascadedShadowsRenderer::addCascadePass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZFrameGraphResource cascadeShadowMap, Razix::RZScene* scene, u32 cascadeIdx)
        {
            const auto name = "CSM #" + std::to_string(cascadeIdx);

            struct CascadeSubPassData
            {
                FrameGraph::RZFrameGraphResource cascadeOuput;
            };

            struct ModelViewProjLayerUBOData
            {
                alignas(16) glm::mat4 viewProj = glm::mat4(1.0f);
                alignas(4) int layer           = 0;
            };

            for (u32 j = 0; j < RAZIX_MAX_SWAP_IMAGES_COUNT; j++) {
                auto cmdBuffer = RZCommandBuffer::Create();
                cmdBuffer->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("CSM cmd buffer [cascade :" + std::to_string(cascadeIdx) + "]"));
                cascadeGPUResources[cascadeIdx].CmdBuffers.push_back(cmdBuffer);
            }

            // Load the shader
            auto shader   = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::CSM);
            auto setInfos = RZResourceManager::Get().getShaderResource(shader)->getDescriptorsPerHeapMap();

            cascadeGPUResources[cascadeIdx].ViewProjLayerUBO = RZResourceManager::Get().createUniformBuffer({"Cascaded Depth pass VPLayerUBO", sizeof(ModelViewProjLayerUBOData), nullptr});

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UniformBuffer)
                        descriptor.uniformBuffer = cascadeGPUResources[cascadeIdx].ViewProjLayerUBO;
                }
                cascadeGPUResources[cascadeIdx].CascadeVPSet.push_back(Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Cascaded Depth pass set")));
            }

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                                   = "Cascade Pass Pipeline";
            pipelineInfo.cullMode                               = Graphics::CullMode::None;
            pipelineInfo.drawType                               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                                 = shader;
            pipelineInfo.transparencyEnabled                    = false;
            pipelineInfo.depthBiasEnabled                       = false;
            pipelineInfo.depthFormat                            = {Graphics::TextureFormat::DEPTH32F};
            cascadeGPUResources[cascadeIdx].CascadePassPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& pass = framegraph.addCallbackPass<CascadeSubPassData>(
                name,
                [&](CascadeSubPassData& data, FrameGraph::RZPassResourceBuilder& builder) {
                builder.setAsStandAlonePass();
                if (cascadeIdx == 0) {
                    cascadeShadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("CascadedShadowMap Array", {.name = "CascadedShadowMapsArray", .width = kShadowMapSize, .height = kShadowMapSize, .layers = kNumCascades, .type = TextureType::Texture_Depth, .format = TextureFormat::DEPTH32F});
                }
                data.cascadeOuput = builder.write(cascadeShadowMap); },
                [=](const CascadeSubPassData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    /**
                     * Since the resource is cloned and we maintain different version of it, the same resource is handled to us every time we ask it
                     */

                    auto cmdBuf = cascadeGPUResources[cascadeIdx].CmdBuffers[Graphics::RHI::GetSwapchain()->getCurrentImageIndex()];

                    // Begin Command Buffer Recording
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("CSM Pass" + std::to_string(cascadeIdx), glm::vec4(0.45, 0.23, 0.56f, 1.0f));

                    // Update the desc sets data
                    constexpr f32             kFarPlane{1.0f};
                    ModelViewProjLayerUBOData uboData;
                    uboData.layer    = cascadeIdx;
                    uboData.viewProj = m_Cascades[cascadeIdx].viewProjMatrix;

                    // Begin Rendering
                    RenderingInfo info{};
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.cascadeOuput).getHandle(), {!cascadeIdx ? true : false, ClearColorPresets::DepthOneToZero}};
                    info.extent          = {kShadowMapSize, kShadowMapSize};
                    /////////////////////////////////
                    // !!! VERY IMPORTANT !!!
                    info.layerCount = kNumCascades;
                    /////////////////////////////////
                    info.resize = false;
                    RHI::BeginRendering(cmdBuf, info);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "CSM Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(cmdBuf, &checkpointData);

                    // Bind pipeline
                    RHI::BindPipeline(cascadeGPUResources[cascadeIdx].CascadePassPipeline, cmdBuf);

                    // Bind Sets
                    RHI::BindUserDescriptorSets(cascadeGPUResources[cascadeIdx].CascadePassPipeline, cmdBuf, cascadeGPUResources[cascadeIdx].CascadeVPSet);

                    // Draw calls
                    // Get the meshes and the models from the Scene and render them

                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetGlobalTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = RZResourceManager::Get().getShaderResource(shader)->getPushConstants()[0];

                        struct PCD
                        {
                            glm::mat4 mat;
                        } pcData;
                        pcData.mat       = transform;
                        modelMatrix.data = &pcData;
                        modelMatrix.size = sizeof(PCD);

                        // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
                        Graphics::RHI::BindPushConstant(cascadeGPUResources[cascadeIdx].CascadePassPipeline, cmdBuf, modelMatrix);
                        //-----------------------------
                        auto buffer = RZResourceManager::Get().getUniformBufferResource(cascadeGPUResources[cascadeIdx].ViewProjLayerUBO);
                        buffer->SetData(sizeof(ModelViewProjLayerUBOData), &uboData);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuf);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuf);

                        Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
                    }
                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////

                    // End Rendering
                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();

                    // End Command Buffer Recording
                    RHI::Submit(cmdBuf);

                    // Submit the work for execution + synchronization
                    // Signal a passDoneSemaphore only on the last cascade pass
                    RHI::SubmitWork({}, {/*PassDoneSemaphore*/});
                });

            return pass.cascadeOuput;
        }
    }    // namespace Graphics
}    // namespace Razix
