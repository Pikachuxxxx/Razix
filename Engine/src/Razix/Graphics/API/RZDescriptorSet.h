#pragma once

#include "Razix/Core/RZCore.h"


namespace Razix {
    namespace Graphics {

        enum class ShaderStage;
        
        /* What type of data does the descriptor set member represent */
        enum class DescriptorType
        {
            UNIFORM_BUFFER, IMAGE_SAMPLER
        };

        /* The format of the input variables in the shader */
        enum class ShaderInputFormat
        {
            R8_UINT,
            R32_UINT,
            R32G32_UINT,
            R32G32B32_UINT,
            R32G32B32A32_UINT,
            R32_INT,
            R32G32_INT,
            R32G32B32_INT,
            R32G32B32A32_INT,
            R32_FLOAT,
            R32G32_FLOAT,
            R32G32B32_FLOAT,
            R32G32B32A32_FLOAT
        };

        /* The shader data type */
        enum class ShaderDataType
        {
            NONE,
            FLOAT32,
            VEC2,
            VEC3,
            VEC4,
            IVEC2,
            IVEC3,
            IVEC4,
            MAT3,
            MAT4,
            INT32,
            INT,
            UINT,
            BOOL,
            STRUCT,
            MAT4ARRAY
        };

        /* Descriptor Binding layout describes the binding and set information of the shader uniform variable, to which shader stages the variable is accessible from */
        struct DescriptorBindingLayout
        {
            std::string     name;           /* The name of the descriptor resource                                                                      */
            DescriptorType  type;           /* The type of the Descriptor, either a buffer or an texture image that is being consumed in the shader     */
            ShaderStage     stage;          /* The shader stage to which the descriptor is bound to                                                     */
            uint32_t        binding = 0;    /* The binding index of the shader                                                                          */
            uint32_t        setID   = 0;    /* The Set ID of the descriptor variables, it enables cross sharing and reuse of descriptors                */
            uint32_t        count   = 1;    /* The number of descriptors that are sent to the same binding slot, ex. used for Joint Transforms[N_BONES] */
        };
        
        /* Vertex Input description describes the input layout format of the Vertex data sent to the Input Assembly and input variable binding and location for input shader variable */
        struct VertexInputDescription
        {
            uint32_t            binding;   /* Binding slot to which the input shader variable is bound to  */
            uint32_t            location;  /* Location ID of the input shader variable                     */
            ShaderInputFormat   format;    /* The format of the vertex input data                          */
            uint32_t            offset;    /* The offset/stride in the vertex data cluster for each vertex */
        };

        /* Information about the uniform buffer members */
        struct UniformBufferMemberInfo
        {
            uint32_t        size;       /* The size of the member                                               */
            uint32_t        offset;     /* The offset of the member in the uniform buffer from the first member */
            ShaderDataType  type;       /* The type of the member, this can be used to resolve the format       */
            std::string     name;       /* The name of the member variable                                      */
            std::string     fullName;   /* The complete name of the member including uniform buffer as prefix   */
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