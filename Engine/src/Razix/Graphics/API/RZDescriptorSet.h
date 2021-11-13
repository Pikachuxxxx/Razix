#pragma once

#include "Razix/Core/Core.h"

#include "Razix/Graphics/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        /* what type of data does the descriptor set member represent in the layout to be passed to the pipeline that communicated with the shader stages */
        enum class DescriptorType
        {
            UNIFORM_BUFFER, IMAGE_SAMPLER
        };

        /* Descriptor Set layout */
        struct DescriptorLayout
        {

        };
        
        /* Vertex Input description */
        struct VertexInputDescription
        {

        };

        class RAZIX_API RZDescriptorSet
        {
        public:

        };
    }
}