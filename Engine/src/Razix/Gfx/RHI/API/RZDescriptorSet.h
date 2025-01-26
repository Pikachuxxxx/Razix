#pragma once

#include "Razix/Core/RZDebugConfig.h"
#include "Razix/Gfx/RHI/API/RZBindingInfoAccessViews.h"

#include "Razix/Gfx/RHI/API/RZAPIDesc.h"
#include "Razix/Gfx/Resources/IRZResource.h"

namespace Razix {
    namespace Gfx {

        /* Forward declaring types to reduce include file complexity */
        class RZTexture;
        class RZUniformBuffer;
        enum ShaderStage : u32;

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

        enum class DescriptorHeapType
        {
            kCbvUavSrvHeap,
            kSamplerHeap,
            kRenderTargetHeap,
            kDepthStencilHeap,
            COUNT
        };

        /* Vertex Input binding information describes the input layout format of the Vertex data sent to the Input Assembly and input variable binding and location for input shader variable */
        struct RAZIX_MEM_ALIGN_16 RZVertexInputBindingInfo
        {
            u32               binding      = 0;
            u32               location     = 0;
            VertexInputFormat vertexFormat = VertexInputFormat::R8_UINT;
            u32               offset       = 0;
        };

        /* Information about the uniform buffer members */
        struct RAZIX_MEM_ALIGN_16 RZShaderBufferMemberInfo
        {
            std::string    name     = "";
            std::string    fullName = "";
            u32            size     = 0;
            u32            offset   = 0;
            ShaderDataType type     = ShaderDataType::INT;
            u32            _padding = 0;
        };

        /* A descriptor describes the shader resource. Stored details about the binding, the data and other necessary information to create the set of descriptor resources */
        struct RAZIX_MEM_ALIGN_16 RZDescriptor
        {
            std::string name     = "$DESCRIPTOR_UNNAMED";
            std::string typeName = "$DESCRIPTOR_TYPE_UNNAMED";
            union
            {
                RZUniformBufferHandle uniformBuffer = {};
                RZTextureHandle       texture;
                RZSamplerHandle       sampler;
            };
            std::vector<RZShaderBufferMemberInfo> uboMembers  = {};
            DescriptorBindingInfo                 bindingInfo = {};
            u32                                   size        = 0;
            u32                                   offset      = 0;
            ///////////////////////////////////////////////////

            // since RZHandle has custom copy and move constructors and operators, it make RZHandle non-trivially copyable
            // so we need to define copy constructors for the union to work and make this type trivial again
            RZDescriptor() {}
            RZDescriptor(const RZDescriptor& other);
            RZDescriptor& operator=(const RZDescriptor& other);
        };

        struct RAZIX_MEM_ALIGN_16 RZPushConstant
        {
            std::string                           typeName      = "$PUSH_CONSTANT_TYPE_UNNAMED";
            std::string                           name          = "$PUSH_CONSTANT_UNNAMED";
            std::vector<RZShaderBufferMemberInfo> structMembers = {};
            void*                                 data          = nullptr;
            DescriptorBindingInfo                 bindingInfo   = {};
            u32                                   size          = 0;
            u32                                   offset        = 0;
            ShaderStage                           shaderStage   = ShaderStage(0);
            u32                                   _padding      = 0;

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
        using DescriptorsPerHeapMap = std::map<u32, std::vector<RZDescriptor>>;
        using DescriptorSets        = std::vector<Gfx::RZDescriptorSetHandle>;    // vector IDx == set Idx

        // https://www.reddit.com/r/vulkan/comments/ybmld8/how_expensive_is_descriptor_set_creationupdate/
        // https://gist.github.com/nanokatze/bb03a486571e13a7b6a8709368bd87cf
        // https://github.com/ARM-software/vulkan_best_practice_for_mobile_developers/blob/master/samples/performance/descriptor_management/descriptor_management_tutorial.md
        // [Transient Resource System] https://logins.github.io/graphics/2021/05/31/RenderGraphs.html#:~:text=Transient%20Resource%20System,are%20also%20called%20transient%20resources.

        /* Shader pointer kind of variable that refers to a bunch of buffers or an image resources and their layout/binding information */
        class RAZIX_API RZDescriptorSet : public IRZResource<RZDescriptorSet>
        {
        public:
            RZDescriptorSet() = default;
            RAZIX_VIRTUAL_DESCTURCTOR(RZDescriptorSet);

            GET_INSTANCE_SIZE;

            /* Updates the descriptor set with the given descriptors */
            virtual void UpdateSet(const std::vector<RZDescriptor>& descriptors) = 0;

            RAZIX_INLINE u32  getSetIdx() const { return m_Desc.setIdx; }
            RAZIX_INLINE void setSetIdx(u32 idx) { m_Desc.setIdx = idx; }

        protected:
            RZDescriptorSetDesc m_Desc;

        private:
            /**
             * Creates the Razix Descriptor set with the given descriptors, it encapsulates the resource per set for all the shader stages
             * @note : As the name suggest the descriptor set contains a set of descriptor resources, along with the data and their binding information
             * 
             * @param descriptor The list of descriptor resources that will be uploaded by the set to various shader stages
             */
            static void Create(void* where, const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            // only resource manager can create an instance of this class
            friend class RZResourceManager;
        };
    }    // namespace Gfx
}    // namespace Razix
