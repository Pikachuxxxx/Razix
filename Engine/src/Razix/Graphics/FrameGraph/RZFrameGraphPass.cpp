
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

            RZFrameGraphDataPass::RZFrameGraphDataPass(RZShaderHandle shader, RZPipelineHandle pipeline, Razix::SceneDrawGeometryMode geometryMode, Resolution res, bool resize, glm::vec2 extents, u32 layers)
                : m_shader(shader), m_pipeline(pipeline), m_geometryMode(geometryMode), m_resolution(res), m_enableResize(resize), m_extent(extents), m_layers(layers)
            {
            }

            void RZFrameGraphDataPass::operator()(RZPassNode &node, RZPassResourceDirectory &resources)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // Since this a debug color we don't care much but see if we can use the color from the Frame Graph Editor here
                RAZIX_MARK_BEGIN(node.getName(), glm::vec4(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX, 1.0f));

                // Rendering Info, use all the writable resources in the pass node to render onto
                RenderingInfo info{};
                info.resolution = m_resolution;
                info.resize     = m_enableResize;
                info.extent     = m_extent;
                info.layerCount = m_layers;

                auto &writeResourceIDs = node.getOutputResources();
                for (auto &writeResourceID: writeResourceIDs) {
                    // Get the attachment info
                    AttachmentInfo attachmentInfo{};
                    if (writeResourceID.flags != kFlagsNone)
                        attachmentInfo = DecodeAttachmentInfo(writeResourceID.flags);

                    // Distinguish b/w texture and buffer resource
                    if (resources.verifyType<RZFrameGraphTexture>(writeResourceID.id)) {
                        // TODO: Use the layer and mip to set the current mip and array layer of the resource
                        auto writeResourceTextureHandle = resources.get<RZFrameGraphTexture>(writeResourceID.id).getHandle();
                        // Distinguish b/w Depth and color texture
                        auto &textureDesc = resources.getDescriptor<RZFrameGraphTexture>(writeResourceID.id);
                        if (textureDesc.type == TextureType::Texture_Depth)
                            info.depthAttachment = {writeResourceTextureHandle, attachmentInfo};
                        else
                            info.colorAttachments.push_back({writeResourceTextureHandle, attachmentInfo});
                    }
                    // TODO: Support writing to buffers in an API friendly way
                }

                RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                RHI::BindPipeline(m_pipeline, RHI::GetCurrentCommandBuffer());

                auto &shaderBindVars = Graphics::RZResourceManager::Get().getShaderResource(m_shader)->getBindVars();

                // Bind the input resources before drawing the scene
                auto &readResourceIDs = node.getInputResources();
                for (auto &readResourceID: readResourceIDs) {
                    // Get the Binding info
                    DescriptorBindingInfo bindingInfo{};
                    if (readResourceID.flags != kFlagsNone)
                        bindingInfo = DecodeDescriptorBindingInfo(readResourceID.flags);

                    // TODO: Move this shaderBindVars to preRead? here the petty problem is we will have to rely on using the name in T::Desc (not a big issue though) + also how to pass shader handle embed into the struct? too weird
                    // One possible solution is to use Global ShaderBindVars table per set, we pass the shader to a global table which will give it's bind vars and we can update it there

                    // Distinguish b/w texture and buffer resource
                    if (resources.verifyType<RZFrameGraphTexture>(readResourceID.id)) {
                        auto readResourceTextureHandle = resources.get<RZFrameGraphTexture>(readResourceID.id).getHandle();
                        // Get the ResourceNode name, bind will be successful if it matches with descriptor name
                        auto descriptor = shaderBindVars[resources.getResourceName<RZFrameGraphTexture>(readResourceID.id)];
                        if (descriptor)
                            descriptor->texture = readResourceTextureHandle;
                    } else {
                        auto readResourceBufferHandle = resources.get<RZFrameGraphBuffer>(readResourceID.id).getHandle();
                        // Get the ResourceNode name, bind will be successful if it matches with descriptor name
                        auto descriptor = shaderBindVars[resources.getResourceName<RZFrameGraphTexture>(readResourceID.id)];
                        if (descriptor)
                            descriptor->uniformBuffer = readResourceBufferHandle;
                    }
                }

                Graphics::RZResourceManager::Get().getShaderResource(m_shader)->setBindVars(shaderBindVars);

                // TODO: Find a better way to update stuff only once that too on first frame
                // Update the Bind vars only on the first frame
                //static bool FirstFrame = true;
                if (RZFrameGraph::IsFirstFrame())
                    RZResourceManager::Get().getShaderResource(m_shader)->updateBindVarsHeaps();

                // Based on the geometry mode, draw the scene
                RZSceneManager::Get().getCurrentScene()->drawScene(m_pipeline, m_geometryMode);

                RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                RAZIX_MARK_END();
            }

            void RZFrameGraphDataPass::resize(RZPassResourceDirectory &resources, u32 width, u32 height)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // TODO: [High Priority] Resize all "Input" texture resources and only then re-generate the descriptor sets

                // Update/Regenerate the descriptors table on resize
                RZResourceManager::Get().getShaderResource(m_shader)->updateBindVarsHeaps();
            }
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix