

// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphPass.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"
#include "Razix/Gfx/FrameGraph/RZPassNode.h"

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Core/Markers/RZMarkers.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        RZFrameGraphDataPass::RZFrameGraphDataPass(FrameGraphDataPassDesc desc)
            : m_Desc(desc)
        {
        }

        void RZFrameGraphDataPass::operator()(RZPassNode& node, RZPassResourceDirectory& resources)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

#if 0
            // Since this a debug color we don't care much but see if we can use the color from the Frame Graph Editor here
            RAZIX_MARK_BEGIN(node.getName(), float4(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX, 1.0f));
            // Rendering Info, use all the writable resources in the pass node to render onto
            RenderingInfo info{};
            info.resolution = m_Desc.resolution;
            info.extent     = m_Desc.extent;
            info.layerCount = m_Desc.layers;

            auto& writeResourceIDs = node.getOutputResources();
            for (auto& writeResourceID: writeResourceIDs) {
                // Get the attachment info
                RenderTargetAttachmentInfo attachmentInfo{};
                if (writeResourceID.flags != kFlagsNone)
                    attachmentInfo = DecodeAttachmentInfo(writeResourceID.flags);

                // Distinguish b/w texture and buffer resource
                if (resources.verifyType<RZFrameGraphTexture>(writeResourceID.id)) {
                    // TODO: Use the layer and mip to set the current mip and array layer of the resource
                    auto writeResourceTextureHandle = resources.get<RZFrameGraphTexture>(writeResourceID.id).getHandle();
                    // Distinguish b/w Depth and color texture
                    auto& textureDesc = resources.getDescriptor<RZFrameGraphTexture>(writeResourceID.id);
                    if (textureDesc.type == TextureType::kDepth)
                        info.depthAttachment = {writeResourceTextureHandle, attachmentInfo};
                    else
                        info.colorAttachments.push_back({writeResourceTextureHandle, attachmentInfo});
                }
                // TODO: Support writing to buffers in an API friendly way
            }

            RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

            RHI::BindPipeline(m_Desc.pipeline, RHI::GetCurrentCommandBuffer());

            auto& shaderBindVars = Gfx::RZResourceManager::Get().getShaderResource(m_Desc.shader)->getBindVars();

            // Bind the input resources before drawing the scene
            auto& readResourceIDs = node.getInputResources();
            for (auto& readResourceID: readResourceIDs) {
                // Get the Binding info
                DescriptorBindingInfo bindingInfo{};
                if (readResourceID.flags != kFlagsNone)
                    bindingInfo = DecodeDescriptorBindingInfo(readResourceID.flags);

                // TODO: Move this shaderBindVars to preRead? here the petty problem is we will have to rely on using the name in T::Desc (not a big issue though) + also how to pass shader handle embed into the struct? too weird
                // One possible solution is to use Global ShaderBindVars table per set, we pass the shader to a global table which will give it's bind vars and we can update it there

                // Distinguish b/w texture and buffer resource
                // Get the ResourceNode name, bind will be successful if it matches with descriptor name
                if (resources.verifyType<RZFrameGraphTexture>(readResourceID.id)) {
                    auto readResourceTextureHandle = resources.get<RZFrameGraphTexture>(readResourceID.id).getHandle();
                    auto descriptor                = shaderBindVars[resources.getResourceName<RZFrameGraphTexture>(readResourceID.id)];
                    if (descriptor)
                        descriptor->texture = readResourceTextureHandle;
                }
                if (resources.verifyType<RZFrameGraphSampler>(readResourceID.id)) {
                    auto readResourceTextureHandle = resources.get<RZFrameGraphSampler>(readResourceID.id).getHandle();
                    auto descriptor                = shaderBindVars[resources.getResourceName<RZFrameGraphSampler>(readResourceID.id)];
                    if (descriptor)
                        descriptor->sampler = readResourceTextureHandle;
                } else {
                    auto readResourceBufferHandle = resources.get<RZFrameGraphBuffer>(readResourceID.id).getHandle();
                    auto descriptor               = shaderBindVars[resources.getResourceName<RZFrameGraphTexture>(readResourceID.id)];
                    if (descriptor)
                        descriptor->uniformBuffer = readResourceBufferHandle;
                }
            }

            Gfx::RZResourceManager::Get().getShaderResource(m_Desc.shader)->setBindVars(shaderBindVars);

            // TODO: Find a better way to update stuff only once that too on first frame
            // Update the Bind vars only on the first frame
            //static bool FirstFrame = true;
            if (RZFrameGraph::IsFirstFrame())
                RZResourceManager::Get().getShaderResource(m_Desc.shader)->updateBindVarsHeaps();

            // Based on the geometry mode, draw the scene
            RZSceneManager::Get().getCurrentScene()->drawScene(m_Desc.pipeline, m_Desc.geometryMode);

            RHI::EndRendering(RHI::GetCurrentCommandBuffer());
            RAZIX_MARK_END();
#endif
        }

        void RZFrameGraphDataPass::resize(RZPassResourceDirectory& resources, u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // TODO: [High Priority] Resize all "Input" texture resources and only then re-generate the descriptor sets

            // Update/Regenerate the descriptors table on resize
            //RZResourceManager::Get().getShaderResource(m_Desc.shader)->updateBindVarsHeaps();
        }
    }    // namespace Gfx
}    // namespace Razix