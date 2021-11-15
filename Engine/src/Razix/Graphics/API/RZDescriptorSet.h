#pragma once

#include "Razix/Core/Core.h"

#include "Razix/Graphics/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        /* What type of data does the descriptor set member represent in the layout to be passed to the pipeline that communicated with the shader stages */
        enum class DescriptorType
        {
            UNIFORM_BUFFER, IMAGE_SAMPLER
        };

        /* Descriptor Set layout */
        struct DescriptorLayout
        {
            DescriptorType type;
            ShaderStage stage;
            uint32_t binding = 0;
            uint32_t setID = 0;
            uint32_t count = 1;
        };
        
        /* Vertex Input description that will be passed to the first stage of graphics pipeline for Input Assembly */
        struct VertexInputDescription
        {
            uint32_t binding;
            uint32_t location;
            uint32_t format;
            uint32_t offset;
        };

        /* Shader pointer kind of variable that refers to a buffer or an image resource */
        class RAZIX_API RZDescriptor
        {
        public:
            static RZDescriptor* Create(const DescriptorLayout& layout);

        };
    }
}