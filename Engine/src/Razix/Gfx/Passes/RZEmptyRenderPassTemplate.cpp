// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZEmptyRenderPassTemplate.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Core/Utils/RZColorUtilities.h"

namespace Razix {
    namespace Gfx {

        void RZEmptyRenderPassTemplate::addPass(RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            framegraph.addCallbackPass(
                "",
                [&](auto& data, RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();
                },
                [=](const auto& data, RZPassResourceDirectory& resources) {
#if 0
                    
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
                    RAZIX_MARK_BEGIN("", Utilities::GenerateHashedColor4(45u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RenderingInfo info{
                        .resolution       = Resolution::kWindow,
                        .colorAttachments = {{rt, {true, ClearColorPresets::OpaqueBlack}}},
                        .resize           = true};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(shader)->getBindVars();

                        RZResourceManager::Get().getShaderResource(shader)->updateBindVarsHeaps();
                    }

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
#endif
                });

            framegraph.getBlackboard().setFinalOutputName("ColorGradedSceneHDR");
        }

        void RZEmptyRenderPassTemplate::destroy()
        {
        }
    }    // namespace Gfx
}    // namespace Razix
