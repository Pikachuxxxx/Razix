#pragma once

namespace Razix {
    namespace Graphics {

        /**
         * Root Signature describes the parameters passed to the various stages of the rendering pipeline
         * 
         * Abstracts over ID3D12RootSignature and vkPipelineLayout (either using API provided here or
         * from shader reflection to automatically generate) 
         */
        class RZRootSignature
        {
        };

    }    // namespace Graphics
}    // namespace Razix