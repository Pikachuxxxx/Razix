// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZBindingInfoAccessViews.h"

#include <glm/integer.hpp>    // bitfield{Insert/Extract}

// Modified from Source: https://github.com/skaarj1989/SupernovaEngine/blob/57f0fba69b36de45255971080ea082bea8219cbb/modules/Renderer/WorldRenderer/src/FrameGraphResourceAccess.cpp (MIT License)

namespace Razix {
    namespace Graphics {

        /**
         * BindingLocation : 9 bits
         * 
         * encoding format:
         * set, binding each of them max take up to 32 each so even 8 bits for each is still a lot
         * cause shaders won't have more than 32 sets/bindings at one point
         * 
         * | 4 bits  | 5 bits  |
         * | 0...16  | 0...16  |
         * |   set   | binding |
         */

        constexpr auto kBindingLocationBits = 9;

        constexpr auto kSetIndexBits     = 4;
        constexpr auto kBindingIndexBits = 5;

        constexpr auto kSetIndexOffset     = 0;
        constexpr auto kBindingIndexOffset = kSetIndexOffset + kSetIndexBits;

        RAZIX_NO_DISCARD u32 BindingLocation::encode()
        {
            return EncodeBindingLocation(*this);
        }

        RAZIX_NO_DISCARD BindingLocation::operator u32() const
        {
            return EncodeBindingLocation(*this);
        }

        RAZIX_NO_DISCARD u32 EncodeBindingLocation(BindingLocation info)
        {
            uint32_t bits{0};
            bits = glm::bitfieldInsert(bits, info.set, kSetIndexOffset, kSetIndexBits);
            bits = glm::bitfieldInsert(bits, info.binding, kBindingIndexOffset, kBindingIndexBits);

            return bits;
        }

        Razix::Graphics::BindingLocation DecodeBindingLocation(u32 bits)
        {
            BindingLocation info{};
            info.set     = glm::bitfieldExtract(bits, kSetIndexOffset, kSetIndexBits);
            info.binding = glm::bitfieldExtract(bits, kBindingIndexOffset, kBindingIndexBits);

            return info;
        }
        //-----------------------------------------------------------------------------------

        /**
         * DescriptorBindingInfo : 29 bits
         * 
         * encoding format:
         * count is array elements of max 1024 which might be less but we're already packing this up
         * 
         * 
         * | 3 bits | 5 bits | 9 bits   | 12        |
         * | 0...7  | 0...5  | 0...8 x2 | 0...4096  |
         * | type   | stage  | binding  | count     |
         * 
         */

        constexpr auto kDescriptorBindingInfoBits = 29;

        constexpr auto kTypeBits  = 3;
        constexpr auto kStageBits = 5;
        constexpr auto kCountBits = 12;

        constexpr auto kTypeOffset       = 0;
        constexpr auto kStageOffset      = kTypeOffset + kTypeBits;
        constexpr auto kBindingLocOffset = kStageOffset + kStageBits;
        constexpr auto kCountOffset      = kBindingLocOffset + kBindingLocationBits;

        RAZIX_NO_DISCARD u32 DescriptorBindingInfo::encode()
        {
            return EncodeDescriptorBindingInfo(*this);
        }

        RAZIX_NO_DISCARD DescriptorBindingInfo::operator u32() const
        {
            return EncodeDescriptorBindingInfo(*this);
        }

        RAZIX_NO_DISCARD u32 EncodeDescriptorBindingInfo(DescriptorBindingInfo info)
        {
            uint32_t bits{0};
            bits = glm::bitfieldInsert(bits, (u32) info.type, kTypeOffset, kTypeBits);
            bits = glm::bitfieldInsert(bits, (u32) info.stage, kStageOffset, kStageBits);
            bits = glm::bitfieldInsert(bits, (u32) EncodeBindingLocation(info.location), kBindingLocOffset, kBindingLocationBits);
            bits = glm::bitfieldInsert(bits, (u32) info.count, kCountOffset, kCountBits);

            return bits;
        }

        Razix::Graphics::DescriptorBindingInfo DecodeDescriptorBindingInfo(u32 bits)
        {
            DescriptorBindingInfo info{};
            info.type     = (DescriptorType) glm::bitfieldExtract(bits, kTypeOffset, kTypeBits);
            info.stage    = (ShaderStage) glm::bitfieldExtract(bits, kStageOffset, kStageBits);
            info.location = DecodeBindingLocation(glm::bitfieldExtract(bits, kBindingLocOffset, kBindingLocationBits));
            info.count    = glm::bitfieldExtract(bits, kCountOffset, kCountBits);

            return info;
        }

        //-----------------------------------------------------------------------------------

        /**
         * AttachmentInfo : 22 bits
         * 
         * encoding format:
         * Even a 16k texture (15360 × 8640) would have maximum mips of 14 so 4 bits are more than enough to represent mips
         * As for layers let's say even if we have texture arrays abnormally large let's say they have 256 or at max 1024 we need 8...10 bits at max
         * 
         * | 1 bits |  3 bits      | 4 bits    | 4 bits   | 10 bits   |
         * | 0...1  |  0...8       | 0...15    | 0...16   | 0...1024  |
         * | clear  | clear color  | binding   | mips     | layer     |
         * 
         */

        constexpr auto kAttachmentInfoBits = 22;

        constexpr auto kClearBits      = 1;
        constexpr auto kClearColorBits = 3;
        constexpr auto kBindingRTBits  = 4;
        constexpr auto kMipsBits       = 4;
        constexpr auto kLayerBits      = 10;

        constexpr auto kClearBitsOffset      = 0;
        constexpr auto kClearColorBitsOffset = kClearBitsOffset + kClearBits;
        constexpr auto kBindingRTBitsOffset  = kClearColorBitsOffset + kClearColorBits;
        constexpr auto kMipsBitsOffset       = kBindingRTBits + kBindingRTBitsOffset;
        constexpr auto kLayerBitsOffset      = kMipsBits + kMipsBitsOffset;

        glm::vec4 ClearColorFromPreset(ClearColorPresets preset)
        {
            switch (preset) {
                case Razix::Graphics::ClearColorPresets::OpaqueBlack:
                    return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case Razix::Graphics::ClearColorPresets::OpaqueWhite:
                    return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case Razix::Graphics::ClearColorPresets::TransparentBlack:
                    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                    break;
                case Razix::Graphics::ClearColorPresets::TransparentWhite:
                    return glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
                    break;
                case Razix::Graphics::ClearColorPresets::Pink:
                    return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
                    break;
                case Razix::Graphics::ClearColorPresets::DepthZeroToOne:
                    return glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
                    break;
                case Razix::Graphics::ClearColorPresets::DepthOneToZero:
                    return glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                    break;
                default:
                    return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
            }
        }

        RAZIX_NO_DISCARD u32 AttachmentInfo::encode()
        {
            return EncodeAttachmentInfo(*this);
        }

        RAZIX_NO_DISCARD AttachmentInfo::operator u32() const
        {
            return EncodeAttachmentInfo(*this);
        }

        RAZIX_NO_DISCARD u32 EncodeAttachmentInfo(AttachmentInfo info)
        {
            uint32_t bits{0};
            bits = glm::bitfieldInsert(bits, (u32) info.clear, kClearBitsOffset, kClearBits);
            bits = glm::bitfieldInsert(bits, (u32) info.clearColor, kClearColorBitsOffset, kClearColorBits);
            bits = glm::bitfieldInsert(bits, info.bindingIdx, kBindingRTBitsOffset, kBindingRTBits);
            bits = glm::bitfieldInsert(bits, info.mip, kMipsBitsOffset, kMipsBits);
            bits = glm::bitfieldInsert(bits, info.layer, kLayerBitsOffset, kLayerBits);

            return bits;
        }

        AttachmentInfo DecodeAttachmentInfo(u32 bits)
        {
            AttachmentInfo info{};
            info.clear      = glm::bitfieldExtract(bits, kClearBitsOffset, kClearBits);
            info.clearColor = (ClearColorPresets) glm::bitfieldExtract(bits, kClearColorBitsOffset, kClearColorBits);
            info.bindingIdx = glm::bitfieldExtract(bits, kBindingRTBitsOffset, kBindingRTBits);
            info.mip        = glm::bitfieldExtract(bits, kMipsBitsOffset, kMipsBits);
            info.layer      = glm::bitfieldExtract(bits, kLayerBitsOffset, kLayerBits);

            return info;
        }
    }    // namespace Graphics
}    // namespace Razix
