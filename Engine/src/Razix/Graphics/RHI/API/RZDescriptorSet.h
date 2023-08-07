#pragma once

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        /* Forward declaring types to reduce include file complexity */
        class RZTexture;
        class RZUniformBuffer;
        enum class ShaderStage;

        /* What type of data does the descriptor set member represent */
        enum class DescriptorType : u32
        {
            UNIFORM_BUFFER,
            IMAGE_SAMPLER    // (combined image sampler)
            // TODO: Add more types like STORAGE_BUFFER, STORAGE_IMAGE, UNIFORM_TEXEL etc.
        };

        /* The format of the input variables in the shader */
        enum class VertexInputFormat : u32
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
            R32_float,
            R32G32_float,
            R32G32B32_float,
            R32G32B32A32_float
        };

        /* The shader data type */
        enum class ShaderDataType : u32
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
        struct RAZIX_MEM_ALIGN RZVertexInputBindingInfo : public RZRoot
        {
            u32               binding  = 0; /* Binding slot to which the input shader variable is bound to                                              */
            u32               location = 0; /* Location ID of the input shader variable                                                                 */
            VertexInputFormat format;       /* The format of the vertex input data                                                                      */
            u32               offset = 0;   /* The offset/stride in the vertex data cluster for each vertex                                             */
        };

        /* Descriptor Binding layout describes the binding and set information of the shader uniform variable, to which shader stages the variable is accessible from */
        struct RAZIX_MEM_ALIGN RZDescriptorLayoutBinding : public RZRoot
        {
            DescriptorType type;        /* The type of the Descriptor, either a buffer or an texture image that is being consumed in the shader     */
            ShaderStage    stage;       /* The shader stage to which the descriptor is bound to                                                     */
            u32            binding = 0; /* The binding index of the shader                                                                          */
            u32            count   = 1; /* The number of descriptors that are sent to the same binding slot, ex. used for Joint Transforms[N_BONES] */
        };

        /* Information about the uniform buffer members */
        struct RAZIX_MEM_ALIGN RZShaderBufferMemberInfo : public RZRoot
        {
            std::string    name;         /* The name of the member variable                                                                             */
            std::string    fullName;     /* The complete name of the member including uniform buffer as prefix                                          */
            u32            size   = 0;   /* The size of the member                                                                                      */
            u32            offset = 0;   /* The offset of the member in the uniform buffer from the first member                                        */
            ShaderDataType type;         /* The type of the member, this can be used to resolve the format                                              */
            u32            _padding = 0; /* Padding variable to pad the structure to 16-byte alignment                                                  */
        };

        // TODO: Add support for texture arrays
        /* A descriptor describes the shader resource. Stored details about the binding, the data and other necessary information to create the set of descriptor resources */
        struct RAZIX_MEM_ALIGN RZDescriptor : public RZRoot
        {
            std::string                           typeName = "Fuck you";
            std::string                           name;
            RZUniformBuffer*                      uniformBuffer = nullptr;
            RZTextureHandle                       texture;
            std::vector<RZShaderBufferMemberInfo> uboMembers;
            u32                                   size   = 0;    //? The size of the descriptor data, can also be extracted from UBO/Texture??
            u32                                   offset = 0;    //? I don't think this is needed
            RZDescriptorLayoutBinding             bindingInfo;
        };

        struct RAZIX_MEM_ALIGN RZPushConstant : public RZRoot
        {
            std::string                           typeName = "Fuck you";
            std::string                           name;
            std::vector<RZShaderBufferMemberInfo> structMembers;
            void*                                 data = nullptr;
            RZDescriptorLayoutBinding             bindingInfo;
            u32                                   size   = 0;
            u32                                   offset = 0;
            ShaderStage                           shaderStage;
            u32                                   _padding = 0;

            RZPushConstant() {}
            RZPushConstant(const std::string& name, ShaderStage stage, u8* data, u32 size, u32 offset)
                : name(name), shaderStage(stage), data(data), size(size), offset(offset) {}

            void setValue(const std::string& name, void* value)
            {
                memset(data, 0, size);
                for (auto& member: structMembers) {
                    if (member.name == name) {
                        // TODO: FIXME
                        //memcpy(&data[member.offset], value, member.size);
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
        using DescriptorSetsCreateInfos = std::map<u32, std::vector<RZDescriptor>>;

        /* Shader pointer kind of variable that refers to a bunch of buffers or an image resources and their layout/binding information */
        class RAZIX_API RZDescriptorSet : public RZRoot
        {
        public:
            RZDescriptorSet() = default;
            ~RZDescriptorSet() { RAZIX_CORE_ERROR("RZPushConstant::OH FUCK"); }

            /**
             * Creates the Razix Descriptor set with the given descriptors, it encapsulates the resource per set for all the shader stages
             * @note : As the name suggest the descriptor set contains a set of descriptor resources, along with the data and their binding information
             * 
             * @param descriptor The list of descriptor resources that will be uploaded by the set to various shader stages
             */
            static RZDescriptorSet* Create(const std::vector<RZDescriptor>& descriptors RZ_DEBUG_NAME_TAG_E_ARG, bool layoutTransition = true);

            /* Updates the descriptor set with the given descriptors */
            virtual void UpdateSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition = true) = 0;
            virtual void Destroy()                                                                             = 0;
        };
    }    // namespace Graphics
}    // namespace Razix