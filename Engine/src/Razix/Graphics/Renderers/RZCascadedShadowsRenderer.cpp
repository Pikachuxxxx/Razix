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
#include "Razix/Graphics/RZModel.h"
#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZCascadedShadowsRenderer::Init()
        {
            // Init the resources (API & Frame Graph) needed for Cascaded Shadow Mapping
            // Import the ShadowMapData as a Buffer into the FrameGraph (used to upload to GPU later)
            m_CascadedMatricesUBO = Graphics::RZUniformBuffer::Create(sizeof(CasdacesUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("Cascades UBO"));
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

            m_CascadedMatricesUBO->Destroy();
            for (u32 i = 0; i < kNumCascades; i++) {
                cascadeGPUResources[i].ViewProjLayerUBO->Destroy();
                for (auto& set: cascadeGPUResources[i].CascadeVPSet)
                    set->Destroy();
                cascadeGPUResources[i].CascadePassPipeline->Destroy();
            }
        }

        //--------------------------------------------------------------------------

        void RZCascadedShadowsRenderer::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto& shadowMapData = blackboard.add<ShadowMapData>();
            // Import the cascades buffer into the FG
            shadowMapData.viewProjMatrices = framegraph.import <FrameGraph::RZFrameGraphBuffer>("Cascade Matrices", {"Cascades UBO", sizeof(CasdacesUBOData)}, {m_CascadedMatricesUBO});

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
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, auto&) {
                    shadowMapData.viewProjMatrices = builder.write(shadowMapData.viewProjMatrices);
                },
                [=](const auto&, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    CasdacesUBOData data{};
                    for (u32 i{0}; i < m_Cascades.size(); ++i) {
                        data.splitDepth[i]       = m_Cascades[i].splitDepth;
                        data.viewProjMatrices[i] = m_Cascades[i].viewProjMatrix;
                    }
                    m_CascadedMatricesUBO->SetData(sizeof(CasdacesUBOData), &data);
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
            auto  shader   = RZShaderLibrary::Get().getShader("cascaded_shadow_maps.rzsf");
            auto& setInfos = shader->getSetsCreateInfos();

            cascadeGPUResources[cascadeIdx].ViewProjLayerUBO = RZUniformBuffer::Create(sizeof(ModelViewProjLayerUBOData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("Cascaded Depth pass VPLayerUBO"));

            for (auto& setInfo: setInfos) {
                // Fill the descriptors with buffers and textures
                for (auto& descriptor: setInfo.second) {
                    if (descriptor.bindingInfo.type == DescriptorType::UNIFORM_BUFFER)
                        descriptor.uniformBuffer = cascadeGPUResources[cascadeIdx].ViewProjLayerUBO;
                }
                cascadeGPUResources[cascadeIdx].CascadeVPSet.push_back(Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Cascaded Depth pass set")));
            }

            // Create the Pipeline
            Graphics::PipelineInfo pipelineInfo{};
            pipelineInfo.cullMode                               = Graphics::CullMode::NONE;
            pipelineInfo.drawType                               = Graphics::DrawType::TRIANGLE;
            pipelineInfo.shader                                 = shader;
            pipelineInfo.transparencyEnabled                    = false;
            pipelineInfo.depthBiasEnabled                       = false;
            pipelineInfo.depthFormat                            = {Graphics::RZTexture::Format::DEPTH32F};
            cascadeGPUResources[cascadeIdx].CascadePassPipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Cascade Pass Pipeline"));

            auto& pass = framegraph.addCallbackPass<CascadeSubPassData>(
                name,
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, CascadeSubPassData& data) { 
                        builder.setAsStandAlonePass();
                    if (cascadeIdx == 0) {
                        cascadeShadowMap = builder.create<FrameGraph::RZFrameGraphTexture>("CascadedShadowMap sArray", {FrameGraph::TextureType::Texture_Depth, "CascadedShadowMapsArray", {kShadowMapSize, kShadowMapSize}, RZTexture::Format::DEPTH32F, kNumCascades});
                    }
                    data.cascadeOuput = builder.write(cascadeShadowMap); },
                [=](const CascadeSubPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    /**
                     * Since the resource is cloned and we maintain different version of it, the same resource is handled to us every time we ask it
                     */

                    auto cmdBuf = cascadeGPUResources[cascadeIdx].CmdBuffers[Graphics::RHI::getSwapchain()->getCurrentImageIndex()];

                    // Begin Command Buffer Recording
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("CSM Pass" + std::to_string(cascadeIdx), glm::vec4(0.45, 0.23, 0.56f, 1.0f));

                    // Update Viewport and Scissor Rect
                    cmdBuf->UpdateViewport(kShadowMapSize, kShadowMapSize);

                    // Update the desc sets data
                    constexpr f32             kFarPlane{1.0f};
                    ModelViewProjLayerUBOData uboData;
                    uboData.layer    = cascadeIdx;
                    uboData.viewProj = m_Cascades[cascadeIdx].viewProjMatrix;

                    // Begin Rendering
                    RenderingInfo info{};
                    info.depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.cascadeOuput).getHandle(), {!cascadeIdx ? true : false, glm::vec4(kFarPlane)}};
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
                    cascadeGPUResources[cascadeIdx].CascadePassPipeline->Bind(cmdBuf);

                    // Bind Sets
                    RHI::BindDescriptorSets(cascadeGPUResources[cascadeIdx].CascadePassPipeline, cmdBuf, cascadeGPUResources[cascadeIdx].CascadeVPSet);

                    // Draw calls
                    // Get the meshes and the models from the Scene and render them

                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////
                    auto group = scene->getRegistry().group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

                        auto& meshes = model.getMeshes();

                        glm::mat4 transform = trans.GetTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = shader->getPushConstants()[0];

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

                        cascadeGPUResources[cascadeIdx].ViewProjLayerUBO->SetData(sizeof(ModelViewProjLayerUBOData), &uboData);

                        // Bind IBO and VBO
                        for (auto& mesh: meshes) {
                            mesh->getVertexBuffer()->Bind(cmdBuf);
                            mesh->getIndexBuffer()->Bind(cmdBuf);

                            Graphics::RHI::DrawIndexed(Graphics::RHI::getCurrentCommandBuffer(), mesh->getIndexCount());
                        }
                    }
                    // MODELS ///////////////////////////////////////////////////////////////////////////////////////////

                    // MESHES ///////////////////////////////////////////////////////////////////////////////////////////
                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetTransform();

                        //-----------------------------
                        // Get the shader from the Mesh Material later
                        // FIXME: We are using 0 to get the first push constant that is the ....... to be continued coz im lazy
                        auto& modelMatrix = shader->getPushConstants()[0];

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

                        cascadeGPUResources[cascadeIdx].ViewProjLayerUBO->SetData(sizeof(ModelViewProjLayerUBOData), &uboData);

                        mrc.Mesh->getVertexBuffer()->Bind(cmdBuf);
                        mrc.Mesh->getIndexBuffer()->Bind(cmdBuf);

                        Graphics::RHI::DrawIndexed(Graphics::RHI::getCurrentCommandBuffer(), mrc.Mesh->getIndexCount());
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
