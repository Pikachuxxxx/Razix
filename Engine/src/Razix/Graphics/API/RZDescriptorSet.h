#pragma once

#include "Razix/Core/RZCore.h"


namespace Razix {
    namespace Graphics {

        /* Forward declaring types to reduce include file complexity */
        class RZTexture;
        class RZUniformBuffer;
        enum class ShaderStage;
        
        /* What type of data does the descriptor set member represent */
        enum class DescriptorType
        {
            UNIFORM_BUFFER, IMAGE_SAMPLER
        };

        /* The format of the input variables in the shader */
        enum class VertexInputFormat
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
        
        /* Vertex Input binding information describes the input layout format of the Vertex data sent to the Input Assembly and input variable binding and location for input shader variable */
        struct RZVertexInputBindingInfo
        {
            uint32_t                                binding;        /* Binding slot to which the input shader variable is bound to                                              */
            uint32_t                                location;       /* Location ID of the input shader variable                                                                 */
            VertexInputFormat                       format;         /* The format of the vertex input data                                                                      */
            uint32_t                                offset;         /* The offset/stride in the vertex data cluster for each vertex                                             */
        };

        /* Descriptor Binding layout describes the binding and set information of the shader uniform variable, to which shader stages the variable is accessible from */
        struct RZDescriptorLayoutBinding
        {
            std::string                             name;           /* The name of the descriptor resource                                                                      */
            DescriptorType                          type;           /* The type of the Descriptor, either a buffer or an texture image that is being consumed in the shader     */
            ShaderStage                             stage;          /* The shader stage to which the descriptor is bound to                                                     */
            uint32_t                                binding = 0;    /* The binding index of the shader                                                                          */
            uint32_t                                count = 1;    /* The number of descriptors that are sent to the same binding slot, ex. used for Joint Transforms[N_BONES] */
        };

        /* Information about the uniform buffer members */
        struct RZShaderBufferMemberInfo
        {
            uint32_t                                size;           /* The size of the member                                                                                   */
            uint32_t                                offset;         /* The offset of the member in the uniform buffer from the first member                                     */
            ShaderDataType                          type;           /* The type of the member, this can be used to resolve the format                                           */
            std::string                             name;           /* The name of the member variable                                                                          */
            std::string                             fullName;       /* The complete name of the member including uniform buffer as prefix                                       */
        };

        // TODO: Add support for texture arrays
        /* A descriptor describes the shader resource. Stored details about the binding, the data and other necessary information to create the set of descriptor resources */
        struct RZDescriptor
        {
            std::string                             name;
            RZUniformBuffer*                        uniformBuffer;
            RZTexture*                              texture;
            std::vector<RZShaderBufferMemberInfo>    uboMembers;
            RZDescriptorLayoutBinding                 bindingInfo;
            uint32_t                                size;  //? The size of the descriptor data, can also be extracted from UBO/Texture??
            uint32_t                                offset; //? I don't think this is needed
        };

        struct RZPushConstant
        {
            std::string name;
            ShaderStage shaderStage;
            uint8_t*    data;
            uint32_t    size;
            uint32_t    offset = 0;

            RZPushConstant(const std::string& name, ShaderStage stage, uint8_t* data, uint32_t size, uint32_t offset)
                : name(name), shaderStage(stage), data(data), size(size), offset(offset) { }

            std::vector<RZShaderBufferMemberInfo> m_Members;

            void setValue(const std::string& name, void* value)
            {
                for (auto& member : m_Members) {
                    if (member.name == name) {
                        memcpy(&data[member.offset], value, member.size);
                        break;
                    }
                }
            }

            void setData(void* value)
            {
                memcpy(data, value, size);
            }
        };

        /* Encapsulating the descriptors of a set along with the setID */
        struct DescriptorSetInfo
        {
            uint32_t                                setID;          /* The set number to which the descriptors correspond to */
            std::vector<RZDescriptor>                 descriptors;    /* The descriptors that will be bound to this set and passed to the GPU */
        };

        /* Shader pointer kind of variable that refers to a bunch of buffers or an image resources and their layout/binding information */
        class RAZIX_API RZDescriptorSet
        {
        public:
            /**
             * Creates the Razix Descriptor set with the given descriptors, it encapsulates the resource per set for all the shader stages
             * @note : As the name suggest the descriptor set contains a set of descriptor resources, along with the data and their binding information
             * 
             * @param descriptor The list of descriptor resources that will be uploaded by the set to various shader stages
             */
            static RZDescriptorSet* Create(const std::vector<RZDescriptor>& descriptors);

            /* Updates the descriptor set with the given descriptors */
            virtual void UpdateSet(const std::vector<RZDescriptor>& descriptors) = 0;
        };
    }
}