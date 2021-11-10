#pragma once


namespace Razix {
    namespace Graphics {

        /* what type of data does the descriptor set member represent in the layout to be passed to the pipeline that communicated with the shader stages */
        enum class DescriptorType
        {
            UNIFORM_BUFFER, IMAGE_SAMPLER
        };

        // Descriptor Set layout
        // Vertex Input description 
    }
}