#pragma once
#pragma once

namespace Razix {
    namespace Gfx {

        /**
         * Flags passed to the Frame Graph Resource during preRead & preWrite
         * 
         * Thanks to Dawid for sharing these references and suggestions during our talks
         * Ref Source: https://github.com/skaarj1989/SupernovaEngine/blob/57f0fba69b36de45255971080ea082bea8219cbb/modules/Renderer/WorldRenderer/src/FrameGraphResourceAccess.hpp (MIT License)
         */

        /**
         * So we can either bind a resource as attachment or pass the Binding info struct 
         * Razix already has these 2 structs in the Graphics API everywhere
         * 
         * These structs have been loaned from RZDescriptorSet.h and RHI.h and
         */

        /**
         * We encode them into 32 bytes before we pass them off to framegraph as flags
         */

        enum class ShaderStage;

        /* What type of data does the descriptor set member represent */
        enum class DescriptorType : u32
        {
            kNone          = UINT32_MAX,    // TODO: Remove this?
            kUniformBuffer = 0,
            kImageSamplerCombined,    // (combined image sampler, Vulkan only)
            kTexture,
            kSampler,
            kRWTyped,    // ??? IDK why/what this is
            kStructured,
            kRWStructured,
            kByteAddress,
            kRWByteAddress,
            kAppendStructured,
            kConsumeStructured,
            kRWStructuredCounter,
            kRTAccelerationStructure,
            COUNT
        };

        //-----------------------------------------------------------------------------------

        /* Descriptor binding location in the shader (set/space, binding) */
        struct RAZIX_MEM_ALIGN_16 BindingLocation
        {
            u32 set     = 0;
            u32 binding = 0;

            RAZIX_NO_DISCARD u32 encode();
            RAZIX_NO_DISCARD     operator u32() const;
        };
        RAZIX_NO_DISCARD u32 EncodeBindingLocation(BindingLocation info);
        BindingLocation      DecodeBindingLocation(u32 bits);

        //-----------------------------------------------------------------------------------

        /* Descriptor Binding layout describes the binding and set information of the shader uniform variable, to which shader stages the variable is accessible from */
        struct RAZIX_MEM_ALIGN_16 DescriptorBindingInfo
        {
            DescriptorType  type;          /* The type of the Descriptor, either a buffer or an texture image that is being consumed in the shader     */
            ShaderStage     stage;         /* The shader stage to which the descriptor is bound to                                                     */
            BindingLocation location = {}; /* The binding index of the shader                                                                          */
            u32             count    = 1;  /* The number of descriptors that are sent to the same binding slot, ex. used for Joint Transforms[N_BONES] */

            RAZIX_NO_DISCARD u32 encode();
            RAZIX_NO_DISCARD     operator u32() const;
        };
        RAZIX_NO_DISCARD u32  EncodeDescriptorBindingInfo(DescriptorBindingInfo info);
        DescriptorBindingInfo DecodeDescriptorBindingInfo(u32 bits);

        //-----------------------------------------------------------------------------------

        enum class ClearColorPresets : u32
        {
            OpaqueBlack,
            OpaqueWhite,
            TransparentBlack,
            TransparentWhite,
            Pink,
            DepthZeroToOne,
            DepthOneToZero,
            COUNT
        };

        static const char* ClearColorPresetsNames[] =
            {
                "OpaqueBlack",
                "OpaqueWhite",
                "TransparentBlack",
                "TransparentWhite",
                "Pink",
                "DepthZeroToOne",
                "DepthOneToZero"};

        RAZIX_ENUM_NAMES_ASSERT(ClearColorPresetsNames, ClearColorPresets);

        glm::vec4 ClearColorFromPreset(ClearColorPresets preset);

        enum class DepthClearColor
        {
            ZeroToOne,
            OneToZero
        };

        /* Gives information for the attachment Info */
        struct RAZIX_MEM_ALIGN_16 AttachmentInfo
        {
            bool              clear      = true;                                /* Whether or not to clear the particular attachment              */
            ClearColorPresets clearColor = ClearColorPresets::TransparentBlack; /* Clear color with which the attachment is cleared               */
            u32               bindingIdx = 0;                                   /* Which output slot to bind the render target to                 */
            u32               mip        = 0;                                   /* Which mip map to render to                                     */
            u32               layer      = 0;                                   /* Which face/array layer to render to in a cubemap/texture array */

            RAZIX_NO_DISCARD u32 encode();
            RAZIX_NO_DISCARD     operator u32() const;
        };
        RAZIX_NO_DISCARD u32 EncodeAttachmentInfo(AttachmentInfo info);
        AttachmentInfo       DecodeAttachmentInfo(u32 bits);

        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
