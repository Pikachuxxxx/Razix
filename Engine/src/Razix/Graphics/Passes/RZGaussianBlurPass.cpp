// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGaussianBlurPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
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

        void RZGaussianBlurPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
        }

        void RZGaussianBlurPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }

        FrameGraph::RZFrameGraphResource RZGaussianBlurPass::addBlurPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings& settings)
        {
#if 0
            // Set the Descriptor Set once rendering starts
            if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                auto& shaderBindVars = RZResourceManager::Get().getShaderResource(blurShader)->getBindVars();

                RZDescriptor* descriptor = nullptr;

                // Bind the GBuffer textures gBuffer0:Normal and SceneDepth
                descriptor = shaderBindVars["inputTexture"];
                if (descriptor)
                    descriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.GBuffer0).getHandle();

                RZResourceManager::Get().getShaderResource(ssaoShader)->updateBindVarsHeaps();
            }
#endif
            return FrameGraph::RZFrameGraphResource{-1};
        }

    }    // namespace Graphics
}    // namespace Razix