// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphPass.h"

#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"
#include "Razix/Graphics/FrameGraph/RZPassNode.h"

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

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GBufferData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/RZSceneManager.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            RZFrameGraphDataPass::RZFrameGraphDataPass(RZShaderHandle shader, RZPipelineHandle pipeline, Razix::SceneDrawParams sceneDrawParams, Resolution res, bool resize)
                : shader(shader), pipeline(pipeline), params(sceneDrawParams), resolution(resolution), enableResize(resize)
            {
            }

            void RZFrameGraphDataPass::operator()(RZPassNode &node, RZPassResourceDirectory &resources)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                RAZIX_MARK_BEGIN(node.getName(), glm::vec4(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX, 1.0f));

                // TODO: Rendering Info, use the writable resources in the pass node
                RenderingInfo info{};
                info.resolution = Resolution::kWindow;
                //info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                //info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                //info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                info.resize = true;

                RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                // TODO: Set the Descriptor Set once rendering starts, using the readable resource in the PassNode
                static bool updatedSets = false;
                if (!updatedSets) {
                    // Create Descriptor Sets here, use the readable resources in the pass node
                    updatedSets = true;
                }

                // TODO: TBD on what to and how to bind the PassNode m_Read resource, use the flags too if needed
                //RHI::BindDescriptorSet(pipeline, RHI::GetCurrentCommandBuffer(), );

                RHI::BindPipeline(pipeline, RHI::GetCurrentCommandBuffer());

                RZSceneManager::Get().getCurrentScene()->drawScene(pipeline, params);

                RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                RAZIX_MARK_END();
            }

            void RZFrameGraphDataPass::resize(RZPassResourceDirectory &resources, u32 width, u32 height)
            {
                // Update the descriptors table

            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix