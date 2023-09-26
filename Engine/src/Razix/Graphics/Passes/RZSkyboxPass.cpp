// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSkyboxPass.h"

#include "Razix/Core/RZApplication.h"
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

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZSkyboxPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto skyboxShader           = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Skybox);
            auto proceduralSkyboxShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::ProceduralSkybox);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Skybox.Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::Front;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = skyboxShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = false;
            pipelineInfo.depthOp                = CompareOp::LessOrEqual;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            pipelineInfo.name    = "ProceduralSkybox.Pipeline";
            pipelineInfo.shader  = proceduralSkyboxShader;
            m_ProceduralPipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            m_SkyboxCube = MeshFactory::CreateCube();

            auto& frameDataBlock  = blackboard.get<FrameData>();
            auto& lightProbesData = blackboard.get<GlobalLightProbeData>();
            auto& sceneData       = blackboard.get<SceneData>();
            auto& volumetricData  = blackboard.get<VolumetricCloudsData>();

            framegraph.addCallbackPass(
                "Pass.Builtin.Code.Skybox",
                [&](auto& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(frameDataBlock.frameData);
                    builder.read(lightProbesData.environmentMap);
                    builder.read(lightProbesData.diffuseIrradianceMap);
                    builder.read(lightProbesData.specularPreFilteredMap);
                    builder.read(sceneData.outputHDR);
                    builder.read(sceneData.depth);
                    builder.read(volumetricData.noiseTexture);

                    sceneData.outputHDR = builder.write(sceneData.outputHDR);
                    sceneData.depth     = builder.write(sceneData.depth);
                },
                [=](const auto& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Skybox pass", glm::vec4(0.33f, 0.45f, 1.0f, 1.0f));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle(), {false, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle(), {false, ClearColorPresets::DepthOneToZero}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(cmdBuffer, info);

                    // Set the Descriptor Set once rendering starts
                    static bool updatedSets = false;
                    if (!updatedSets) {
                        auto envMap = resources.get<FrameGraph::RZFrameGraphTexture>(lightProbesData.environmentMap).getHandle();

                        RZDescriptor lightProbes_descriptor{};
                        lightProbes_descriptor.bindingInfo.location.binding = 0;
                        lightProbes_descriptor.bindingInfo.type             = DescriptorType::ImageSamplerCombined;
                        lightProbes_descriptor.bindingInfo.stage            = ShaderStage::Pixel;
                        lightProbes_descriptor.texture                      = envMap;

                        m_LightProbesDescriptorSet = RZDescriptorSet::Create({lightProbes_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Env Map - Skybox"));

                        auto noiseTexture = resources.get<FrameGraph::RZFrameGraphTexture>(volumetricData.noiseTexture).getHandle();

                        RZDescriptor volumetric_descriptor{};
                        volumetric_descriptor.bindingInfo.location.binding = 0;
                        volumetric_descriptor.bindingInfo.type             = DescriptorType::ImageSamplerCombined;
                        volumetric_descriptor.bindingInfo.stage            = ShaderStage::Pixel;
                        volumetric_descriptor.texture                      = noiseTexture;

                        m_VolumetricDescriptorSet = RZDescriptorSet::Create({volumetric_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Volumetric"));

                        updatedSets = true;
                    }

                    if (!m_UseProceduralSkybox) {
                        Graphics::RHI::BindPipeline(m_Pipeline, cmdBuffer);
                        Graphics::RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);
                        Graphics::RHI::BindDescriptorSet(m_Pipeline, cmdBuffer, m_LightProbesDescriptorSet, BindingTable_System::SET_IDX_MATERIAL_DATA);
                        //RHI::EnableBindlessTextures(m_Pipeline, cmdBuffer);
                    } else {
                        Graphics::RHI::BindPipeline(m_ProceduralPipeline, cmdBuffer);
                        Graphics::RHI::BindDescriptorSet(m_ProceduralPipeline, cmdBuffer, RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);
                        Graphics::RHI::BindDescriptorSet(m_ProceduralPipeline, cmdBuffer, m_VolumetricDescriptorSet, BindingTable_System::SET_IDX_MATERIAL_DATA);
                        //RHI::EnableBindlessTextures(m_ProceduralPipeline, cmdBuffer);
                    }

                    m_SkyboxCube->getIndexBuffer()->Bind(cmdBuffer);
                    m_SkyboxCube->getVertexBuffer()->Bind(cmdBuffer);

                    if (!m_UseProceduralSkybox) {
                        //u32            envMapIdx = resources.get<FrameGraph::RZFrameGraphTexture>(lightProbesData.environmentMap).getHandle().getIndex();
                        //RZPushConstant pc;
                        //pc.data        = &envMapIdx;
                        //pc.size        = sizeof(u32);
                        //pc.shaderStage = ShaderStage::PIXEL;
                        //
                        //RHI::BindPushConstant(m_Pipeline, cmdBuffer, pc);
                    } else {
                        // Since no skybox, we update the directional light direction
                        auto lights = scene->GetComponentsOfType<LightComponent>();
                        // We use the first found directional light
                        // TODO: Cache this
                        RZLight dirLight;
                        for (auto& lc: lights) {
                            if (lc.light.getType() == LightType::DIRECTIONAL)
                                dirLight = lc.light;
                            break;
                        }
                        struct PCData
                        {
                            glm::vec3 worldSpaceLightPos;
                            //u32       noiseTextureIdx;
                        } data{};
                        // FIXME: Use direction
                        data.worldSpaceLightPos = dirLight.getPosition();
                        //data.noiseTextureIdx    = resources.get<FrameGraph::RZFrameGraphTexture>(volumetricData.noiseTexture).getHandle().getIndex();
                        RZPushConstant pc;
                        pc.data        = &data;
                        pc.size        = sizeof(PCData);
                        pc.shaderStage = ShaderStage::Pixel;

                        RHI::BindPushConstant(m_ProceduralPipeline, cmdBuffer, pc);
                    }

                    RHI::DrawIndexed(cmdBuffer, m_SkyboxCube->getIndexBuffer()->getCount());

                    RHI::EndRendering(cmdBuffer);
                    RAZIX_MARK_END();
                });
        }

        void RZSkyboxPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            RZResourceManager::Get().destroyPipeline(m_ProceduralPipeline);
            m_SkyboxCube->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix