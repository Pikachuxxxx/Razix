#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Graphics/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        /* What type of data does the descriptor set member represent */
        enum class DescriptorType
        {
            UNIFORM_BUFFER, IMAGE_SAMPLER
        };

        /* Descriptor Binding layout describes the binding and set information of the shader uniform variable, to which shader stages the variable is accessible from */
        struct DescriptorBindingLayout
        {
            DescriptorType  type;           /* The type of the Descriptor, either a buffer or an texture image that is being consumed in the shader     */
            ShaderStage     stage;          /* The shader stage to which the descriptor is bound to                                                     */
            uint32_t        binding = 0;    /* The binding index of the shader                                                                          */
            uint32_t        setID   = 0;    /* The Set ID of the descriptor variables, it enables cross sharing and reuse of descriptors                */
            uint32_t        count   = 1;    /* The number of descriptors that are sent to the same binding slot, ex. used for Joint Transforms[N_BONES] */
        };
        
        /* Vertex Input description describes the input layout format of the Vertex data sent to the Input Assembly and input variable binding and location for input shader variable */
        struct VertexInputDescription
        {
            uint32_t binding;   /* Binding slot to which the input shader variable is bound to  */
            uint32_t location;  /* Location ID of the input shader variable                     */
            uint32_t format;    /* The format of the vertex input data                          */
            uint32_t offset;    /* The offset/stride in the vertex data cluster for each vertex */
        };

        /* Shader pointer kind of variable that refers to a buffer or an image resource */
        class RAZIX_API RZDescriptor
        {
        public:
            /**
             * Creates the Razix Descriptor object with the given descriptor layout
             */
            static RZDescriptor* Create(const DescriptorBindingLayout& layout);

        };
    }
}