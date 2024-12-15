// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKUtilities.h"

#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"
#include "Razix/Gfx/RHI/API/RZIndexBuffer.h"
#include "Razix/Gfx/RHI/API/RZPipeline.h"
#include "Razix/Gfx/RHI/API/RZShader.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include "Razix/Platform/API/Vulkan/VKBuffer.h"
#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include <spirv_reflect.h>

namespace Razix {
    namespace Gfx {
        namespace VKUtilities {

            void CopyDataToGPUBufferResource(const void* cpuData, VkBuffer gpuBuffer, u32 size, u32 srcOffset, u32 dstOffset)
            {
                /**
                * For anything else we copy using a staging buffer to copy to the GPU
                */
                VKBuffer transferBuffer = VKBuffer(BufferUsage::Staging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, cpuData RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging buffer to copy to Device only GPU buffer"));
                {
                    // 1.1 Copy from staging buffer to Image
                    VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                    VkBufferCopy region = {};
                    region.srcOffset    = 0;
                    region.dstOffset    = 0;
                    region.size         = size;

                    vkCmdCopyBuffer(commandBuffer, transferBuffer.getBuffer(), gpuBuffer, 1, &region);

                    VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
                }
                transferBuffer.destroy();
            }

            void CopyDataToGPUTextureResource(const void* cpuData, VkImage gpuTexture, u32 width, u32 height, u64 size, u32 mipLevel /*= 0*/, u32 layersCount /*= 1*/, u32 baseArrayLayer /*= 0*/)
            {
                // Create a Staging buffer (Transfer from source) to transfer texture data from HOST memory to DEVICE memory
                VKBuffer* stagingBuffer = new VKBuffer(BufferUsage::Staging, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<u32>(size), cpuData RZ_DEBUG_NAME_TAG_STR_E_ARG("Staging Buffer for VKTexture"));

                // 1.1 Copy from staging buffer to Image
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkBufferImageCopy region               = {};
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset                     = {0, 0, 0};
                region.imageExtent                     = {width, height, 1};

                vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), gpuTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);

                stagingBuffer->destroy();
                delete stagingBuffer;
            }

            //-----------------------------------------------------------------------------------
            // Texture Utility Functions
            //-----------------------------------------------------------------------------------

            VkFormat TextureFormatToVK(const TextureFormat format, bool srgb /*= false*/)
            {
                if (srgb) {
                    switch (format) {
                        case TextureFormat::R8:
                            return VK_FORMAT_R8_SRGB;
                            break;
                        case TextureFormat::R32_UINT:
                            return VK_FORMAT_R32_UINT;
                            break;
                        case TextureFormat::R32_INT:
                            return VK_FORMAT_R32_SINT;
                            break;
                        case TextureFormat::R32F:
                            return VK_FORMAT_R32_SFLOAT;
                            break;
                        case TextureFormat::RG8:
                            return VK_FORMAT_R8G8_SRGB;
                            break;
                        case TextureFormat::RG16F:
                            return VK_FORMAT_R16G16_SFLOAT;
                            break;
                        case TextureFormat::RGB8:
                            return VK_FORMAT_R8G8B8_SRGB;
                            break;
                        case TextureFormat::RGBA8:
                            return VK_FORMAT_R8G8B8A8_SRGB;
                            break;
                        case TextureFormat::RGB16:
                            return VK_FORMAT_R16G16B16_SFLOAT;
                            break;
                        case TextureFormat::RGBA16:
                            return VK_FORMAT_R16G16B16A16_SFLOAT;
                            break;
                        case TextureFormat::RGBA16F:
                            return VK_FORMAT_R16G16B16A16_SFLOAT;
                            break;
                        case TextureFormat::RGB32:
                            return VK_FORMAT_R32G32B32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32F:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGB:
                            return VK_FORMAT_R8G8B8_SRGB;
                            break;
                        case TextureFormat::RGBA:
                            return VK_FORMAT_R8G8B8A8_SRGB;
                            break;
                        case TextureFormat::BGRA8_UNORM:
                            return VK_FORMAT_B8G8R8A8_UNORM;
                            break;
                        case TextureFormat::SCREEN:
                            return VK_FORMAT_B8G8R8A8_SRGB;
                            break;
                        case TextureFormat::DEPTH16_UNORM:
                            return VK_FORMAT_D16_UNORM;
                            break;
                        case TextureFormat::DEPTH_STENCIL:
                            return VK_FORMAT_D32_SFLOAT_S8_UINT;
                            break;
                        case TextureFormat::DEPTH32F:
                            return VK_FORMAT_D32_SFLOAT;
                            break;
                        case TextureFormat::NONE:
                            return VK_FORMAT_UNDEFINED;
                            break;
                        default:
                            RAZIX_CORE_WARN("[Texture VK] Unsupported Texture format");
                            return VK_FORMAT_UNDEFINED;
                            break;
                    }
                } else {
                    switch (format) {
                        case TextureFormat::R8:
                            return VK_FORMAT_R8_UNORM;
                            break;
                        case TextureFormat::R32_UINT:
                            return VK_FORMAT_R32_UINT;
                            break;
                        case TextureFormat::R32_INT:
                            return VK_FORMAT_R32_SINT;
                            break;
                        case TextureFormat::R32F:
                            return VK_FORMAT_R32_SFLOAT;
                            break;
                        case TextureFormat::RG8:
                            return VK_FORMAT_R8G8_UINT;
                            break;
                        case TextureFormat::RG16F:
                            return VK_FORMAT_R16G16_SFLOAT;
                            break;
                            ;
                        case TextureFormat::RGB8:
                            return VK_FORMAT_R8G8B8_UNORM;
                            break;
                        case TextureFormat::RGBA8:
                            return VK_FORMAT_R8G8B8A8_UNORM;
                            break;
                        case TextureFormat::RGB16:
                            return VK_FORMAT_R16G16B16_UNORM;
                            break;
                        case TextureFormat::RGBA16:
                            return VK_FORMAT_R16G16B16A16_UNORM;
                            break;
                        case TextureFormat::RGBA16F:
                            return VK_FORMAT_R16G16B16A16_SFLOAT;
                            break;
                        case TextureFormat::RGB32:
                            return VK_FORMAT_R32G32B32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGBA32F:
                            return VK_FORMAT_R32G32B32A32_SFLOAT;
                            break;
                        case TextureFormat::RGB:
                            return VK_FORMAT_R8G8B8_UNORM;
                            break;
                        case TextureFormat::RGBA:
                            return VK_FORMAT_R8G8B8A8_UNORM;
                            break;
                        case TextureFormat::BGRA8_UNORM:
                            return VK_FORMAT_B8G8R8A8_UNORM;
                            break;
                        case TextureFormat::SCREEN:
                            return VK_FORMAT_B8G8R8A8_UNORM;
                            break;
                        case TextureFormat::DEPTH16_UNORM:
                            return VK_FORMAT_D16_UNORM;
                            break;
                        case TextureFormat::DEPTH_STENCIL:
                            return VK_FORMAT_D32_SFLOAT_S8_UINT;
                            break;
                        case TextureFormat::DEPTH32F:
                            return VK_FORMAT_D32_SFLOAT;
                            break;
                        case TextureFormat::NONE:
                            return VK_FORMAT_UNDEFINED;
                            break;
                        default:
                            RAZIX_CORE_WARN("[Texture VK] Unsupported Texture format");
                            return VK_FORMAT_UNDEFINED;
                            break;
                    }
                }
            }

            VkSamplerAddressMode TextureWrapToVK(const Wrapping wrap)
            {
                switch (wrap) {
                    case Wrapping::REPEAT:
                        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
                        break;
                    case Wrapping::MIRRORED_REPEAT:
                        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                        break;
                    case Wrapping::CLAMP_TO_EDGE:
                        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                        break;
                    case Wrapping::CLAMP_TO_BORDER:
                        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                        break;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported Wrap Mode");
                        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                        break;
                }
            }

            VkImageType TextureTypeToVK(const TextureType type)
            {
                switch (type) {
                    case TextureType::Texture_1D:
                        return VK_IMAGE_TYPE_1D;
                    case TextureType::Texture_2D:
                    case TextureType::Texture_2DArray:
                    case TextureType::Texture_Depth:
                    case TextureType::Texture_CubeMap:
                    case TextureType::Texture_CubeMapArray:
                    case TextureType::Texture_SwapchainImage:
                        return VK_IMAGE_TYPE_2D;
                    case TextureType::Texture_3D:
                        return VK_IMAGE_TYPE_3D;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported Texture Type");
                        return VK_IMAGE_TYPE_2D;
                        break;
                }
            }

            VkImageViewType TextureTypeToVKViewType(const TextureType type)
            {
                switch (type) {
                    case TextureType::Texture_1D:
                        return VK_IMAGE_VIEW_TYPE_1D;
                    case TextureType::Texture_2D:
                        return VK_IMAGE_VIEW_TYPE_2D;
                    case TextureType::Texture_2DArray:
                        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                    case TextureType::Texture_Depth:
                        return VK_IMAGE_VIEW_TYPE_2D;
                    case TextureType::Texture_CubeMap:
                        return VK_IMAGE_VIEW_TYPE_CUBE;
                    case TextureType::Texture_CubeMapArray:
                        return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                    case TextureType::Texture_SwapchainImage:
                        return VK_IMAGE_VIEW_TYPE_2D;
                    case TextureType::Texture_3D:
                        return VK_IMAGE_VIEW_TYPE_3D;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported Texture View Type");
                        return VK_IMAGE_VIEW_TYPE_2D;
                        break;
                }
            }

            VkFilter TextureFilterToVK(const Filtering::Mode filter)
            {
                switch (filter) {
                    case Filtering::Mode::LINEAR:
                        return VK_FILTER_LINEAR;
                        break;
                    case Filtering::Mode::NEAREST:
                        return VK_FILTER_NEAREST;
                        break;
                    default:
                        RAZIX_CORE_WARN("[Texture] Unsupported TextureFilter type!");
                        return VK_FILTER_LINEAR;
                }
            }

            void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mipLevels /*= 1*/, u32 layerCount /*= 1*/)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // Begin the buffer since this done for computability with shader pipeline stages we use pipeline barrier to synchronize the transition
                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommandBuffer();

                VkImageMemoryBarrier barrier = {};
                barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.oldLayout            = oldLayout;
                barrier.newLayout            = newLayout;
                barrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
                barrier.image                = image;
                if (format >= 124 && format <= 130)    // All possible depth formats
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                else
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseMipLevel   = 0;
                barrier.subresourceRange.levelCount     = mipLevels;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount     = layerCount;

                //if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
                //    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                //    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
                //        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                //    }
                //} else {
                //    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                //}

                VkPipelineStageFlags sourceStage      = 0;
                VkPipelineStageFlags destinationStage = 0;

                // set up source properties
                if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
                    barrier.srcAccessMask = 0;
                    sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
                    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL) {
                    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                    sourceStage           = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                } else {
                    RAZIX_CORE_WARN("[Vulkan] Unsupported layout transition!");
                }

                // set up destination properties
                if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
                    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
                    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                } else if (newLayout == VK_IMAGE_LAYOUT_GENERAL) {
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                    destinationStage      = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                } else {
                    RAZIX_CORE_WARN("[Vulkan] Unsupported layout transition!");
                }

                // Use a pipeline barrier to make sure the transition is done properly
                vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                // End the buffer
                VKUtilities::EndSingleTimeCommandBuffer(commandBuffer);
            }

            u32 EngineImageLayoutToVK(ImageLayout layout)
            {
                switch (layout) {
                    case Razix::Gfx::ImageLayout::kUndefined:
                        return VK_IMAGE_LAYOUT_UNDEFINED;
                        break;
                    case Razix::Gfx::ImageLayout::kGeneral:
                        return VK_IMAGE_LAYOUT_GENERAL;
                        break;
                    case Razix::Gfx::ImageLayout::kColorAttachmentOptimal:
                        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                        break;
                    case Razix::Gfx::ImageLayout::kDepthStencilAttachmentOptimal:
                        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                        break;
                    case Razix::Gfx::ImageLayout::kDepthStencilReadOnlyOptimal:
                        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                        break;
                    case Razix::Gfx::ImageLayout::kShaderReadOnlyOptimal:
                        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        break;
                    case Razix::Gfx::ImageLayout::kTransferSrcOptimal:
                        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        break;
                    case Razix::Gfx::ImageLayout::kTransferDstOptimal:
                        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                        break;
                    case Razix::Gfx::ImageLayout::kPresentationEngine:
                        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                        break;
                    default:
                        return VK_IMAGE_LAYOUT_UNDEFINED;
                        break;
                }
            }

            u32 EngineMemoryAcsessMaskToVK(MemoryAccessMask mask)
            {
                switch (mask) {
                    case Razix::Gfx::MemoryAccessMask::kNone:
                        return VK_ACCESS_NONE;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kIndirectCommandReadBit:
                        return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kIndexBufferDataReadBit:
                        return VK_ACCESS_INDEX_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kVertexAttributeReadBit:
                        return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kUniformReadBit:
                        return VK_ACCESS_UNIFORM_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kInputAttachmentReadBit:
                        return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kShaderReadBit:
                        return VK_ACCESS_SHADER_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kShaderWriteBit:
                        return VK_ACCESS_SHADER_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kColorAttachmentReadBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kColorAttachmentWriteBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kColorAttachmentReadWriteBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kDepthStencilAttachmentReadBit:
                        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kDepthStencilAttachmentWriteBit:
                        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kTransferReadBit:
                        return VK_ACCESS_TRANSFER_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kTransferWriteBit:
                        return VK_ACCESS_TRANSFER_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kHostReadBit:
                        return VK_ACCESS_HOST_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kHostWriteBit:
                        return VK_ACCESS_HOST_WRITE_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kMemoryReadBit:
                        return VK_ACCESS_MEMORY_READ_BIT;
                        break;
                    case Razix::Gfx::MemoryAccessMask::kMemoryWriteBit:
                        return VK_ACCESS_MEMORY_WRITE_BIT;
                        break;
                    default:
                        return VK_ACCESS_NONE;
                        break;
                }
            }

            u32 EnginePipelineStageToVK(PipelineStage ppstage)
            {
                switch (ppstage) {
                    case Razix::Gfx::PipelineStage::kTopOfPipe:
                        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kDrawIndirect:
                        return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kDraw:
                        return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kVertexInput:
                        return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kVertexShader:
                        return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kTessellationControlShader:
                        return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kTessellationEvaluationShader:
                        return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kGeometryShader:
                        return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kFragmentShader:
                        return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kEarlyFragmentTests:
                        return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kLateFragmentTests:
                        return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kEarlyOrLateTests:
                        return (VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
                        break;
                    case Razix::Gfx::PipelineStage::kColorAttachmentOutput:
                        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kComputeShader:
                        return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kTransfer:
                        return VK_PIPELINE_STAGE_TRANSFER_BIT;
                        break;
                    case Razix::Gfx::PipelineStage::kMeshShader:
                        return VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
                        break;
                    case Razix::Gfx::PipelineStage::kTaskShader:
                        return VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
                        break;
                    case Razix::Gfx::PipelineStage::kBottomOfPipe:
                        return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                        break;
                    default:
                        return VK_PIPELINE_STAGE_NONE;
                        break;
                }
            }

            //-----------------------------------------------------------------------------------
            // Single Time Command Buffer Utility Functions
            //-----------------------------------------------------------------------------------

            VkCommandBuffer BeginSingleTimeCommandBuffer()
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                VkCommandBufferAllocateInfo allocInfo = {};
                allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                // TODO: Use a dedicated transfer only pool for copy operations, but this is a generic one and we use it for IBL too maybe this if fine.
                allocInfo.commandPool        = VKDevice::Get().getSingleTimeGraphicsCommandPool()->getVKPool();
                allocInfo.commandBufferCount = 1;

                VkCommandBuffer commandBuffer;
                VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().getDevice(), &allocInfo, &commandBuffer));

                VkCommandBufferBeginInfo beginInfo = {};
                beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

                return commandBuffer;
            }

            void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

                VkSubmitInfo submitInfo         = {};
                submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount   = 1;
                submitInfo.pCommandBuffers      = &commandBuffer;
                submitInfo.pSignalSemaphores    = nullptr;
                submitInfo.pNext                = nullptr;
                submitInfo.pWaitDstStageMask    = nullptr;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.waitSemaphoreCount   = 0;

                VK_CHECK_RESULT(vkQueueSubmit(VKDevice::Get().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
                VK_CHECK_RESULT(vkQueueWaitIdle(VKDevice::Get().getGraphicsQueue()));

                vkFreeCommandBuffers(VKDevice::Get().getDevice(), VKDevice::Get().getSingleTimeGraphicsCommandPool()->getVKPool(), 1, &commandBuffer);
            }

            //-----------------------------------------------------------------------------------
            // Format Utility
            //-----------------------------------------------------------------------------------

            VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
            {
                for (VkFormat format: candidates) {
                    VkFormatProperties props;
                    vkGetPhysicalDeviceFormatProperties(VKDevice::Get().getGPU(), format, &props);

                    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                        return format;
                    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                        return format;
                    }
                }
                RAZIX_CORE_WARN("Could not find supported format");
                return VK_FORMAT_UNDEFINED;
            }

            VkFormat FindDepthFormat()
            {
                return FindSupportedFormat(
                    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
            }

            //-----------------------------------------------------------------------------------
            // Enum Conversions
            //-----------------------------------------------------------------------------------

            VkPrimitiveTopology DrawTypeToVK(Razix::Gfx::DrawType type)
            {
                switch (type) {
                    case Razix::Gfx::DrawType::Point:
                        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                        break;
                    case Razix::Gfx::DrawType::Triangle:
                        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                        break;
                    case Razix::Gfx::DrawType::Line:
                        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown Draw Type! using triangle list to draw the geometry");
                        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                        break;
                }
            }

            VkCullModeFlags CullModeToVK(Razix::Gfx::CullMode cullMode)
            {
                switch (cullMode) {
                    case Razix::Gfx::CullMode::Front:
                        return VK_CULL_MODE_FRONT_BIT;
                        break;
                    case Razix::Gfx::CullMode::Back:
                        return VK_CULL_MODE_BACK_BIT;
                        break;
                    case Razix::Gfx::CullMode::FrontBack:
                        return VK_CULL_MODE_FRONT_AND_BACK;
                        break;
                    case Razix::Gfx::CullMode::None:
                        return VK_CULL_MODE_NONE;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown Cull Mode! Using Back Face Culling by default");
                        return VK_CULL_MODE_BACK_BIT;
                        break;
                }
            }

            VkPolygonMode PolygoneModeToVK(Razix::Gfx::PolygonMode polygonMode)
            {
                switch (polygonMode) {
                    case Razix::Gfx::PolygonMode::Fill:
                        return VK_POLYGON_MODE_FILL;
                        break;
                    case Razix::Gfx::PolygonMode::Line:
                        return VK_POLYGON_MODE_LINE;
                        break;
                    case Razix::Gfx::PolygonMode::Point:
                        return VK_POLYGON_MODE_POINT;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown polygon mode! Using fill by default");
                        return VK_POLYGON_MODE_FILL;
                        break;
                }
            }

            VkBlendOp BlendOpToVK(Razix::Gfx::BlendOp blendOp)
            {
                switch (blendOp) {
                    case Razix::Gfx::BlendOp::Add:
                        return VK_BLEND_OP_ADD;
                        break;
                    case Razix::Gfx::BlendOp::Subtract:
                        return VK_BLEND_OP_SUBTRACT;
                        break;
                    case Razix::Gfx::BlendOp::ReverseSubtract:
                        return VK_BLEND_OP_REVERSE_SUBTRACT;
                        break;
                    case Razix::Gfx::BlendOp::Min:
                        return VK_BLEND_OP_MIN;
                        break;
                    case Razix::Gfx::BlendOp::Max:
                        return VK_BLEND_OP_MAX;
                        break;
                    default:
                        return VK_BLEND_OP_ADD;
                        break;
                }
                return VK_BLEND_OP_ADD;
            }

            VkBlendFactor BlendFactorToVK(Razix::Gfx::BlendFactor blendFactor)
            {
                switch (blendFactor) {
                    case Razix::Gfx::BlendFactor::Zero:
                        return VK_BLEND_FACTOR_ZERO;
                        break;
                    case Razix::Gfx::BlendFactor::One:
                        return VK_BLEND_FACTOR_ONE;
                        break;
                    case Razix::Gfx::BlendFactor::SrcColor:
                        return VK_BLEND_FACTOR_SRC_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusSrcColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::DstColor:
                        return VK_BLEND_FACTOR_DST_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusDstColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::SrcAlpha:
                        return VK_BLEND_FACTOR_SRC_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusSrcAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::DstAlpha:
                        return VK_BLEND_FACTOR_DST_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusDstAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::ConstantColor:
                        return VK_BLEND_FACTOR_CONSTANT_COLOR;
                        break;
                    case BlendFactor::OneMinusConstantColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
                        break;
                    case Razix::Gfx::BlendFactor::ConstantAlpha:
                        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::OneMinusConstantAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
                        break;
                    case Razix::Gfx::BlendFactor::SrcAlphaSaturate:
                        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
                        break;
                    default:
                        return VK_BLEND_FACTOR_ONE;
                        break;
                }
                return VK_BLEND_FACTOR_ONE;
            }

            VkCompareOp CompareOpToVK(Razix::Gfx::CompareOp compareOp)
            {
                switch (compareOp) {
                    case Razix::Gfx::CompareOp::Never:
                        return VK_COMPARE_OP_NEVER;
                        break;
                    case Razix::Gfx::CompareOp::Less:
                        return VK_COMPARE_OP_LESS;
                        break;
                    case Razix::Gfx::CompareOp::Equal:
                        return VK_COMPARE_OP_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::LessOrEqual:
                        return VK_COMPARE_OP_LESS_OR_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::Greater:
                        return VK_COMPARE_OP_GREATER;
                        break;
                    case Razix::Gfx::CompareOp::NotEqual:
                        return VK_COMPARE_OP_NOT_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::GreaterOrEqual:
                        return VK_COMPARE_OP_GREATER_OR_EQUAL;
                        break;
                    case Razix::Gfx::CompareOp::Always:
                        return VK_COMPARE_OP_ALWAYS;
                        break;
                    default:
                        return VK_COMPARE_OP_LESS_OR_EQUAL;
                        break;
                }
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            }

            VkShaderStageFlagBits ShaderStageToVK(Razix::Gfx::ShaderStage stage)
            {
                int result = 0;

                if ((stage & Razix::Gfx::ShaderStage::kVertex) == Razix::Gfx::ShaderStage::kVertex)
                    result |= VK_SHADER_STAGE_VERTEX_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kPixel) == Razix::Gfx::ShaderStage::kPixel)
                    result |= VK_SHADER_STAGE_FRAGMENT_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kGeometry) == Razix::Gfx::ShaderStage::kGeometry)
                    result |= VK_SHADER_STAGE_GEOMETRY_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kTesselationControl) == Razix::Gfx::ShaderStage::kTesselationControl)
                    result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kTesselationEvaluation) == Razix::Gfx::ShaderStage::kTesselationEvaluation)
                    result |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                if ((stage & Razix::Gfx::ShaderStage::kCompute) == Razix::Gfx::ShaderStage::kCompute)
                    result |= VK_SHADER_STAGE_COMPUTE_BIT;

                return (VkShaderStageFlagBits) result;
            }

            u32 GetStrideFromVulkanFormat(VkFormat format)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                switch (format) {
                    case VK_FORMAT_R8_SINT:
                        return sizeof(int);
                    case VK_FORMAT_R32_SFLOAT:
                        return sizeof(float);
                    case VK_FORMAT_R32G32_SFLOAT:
                        return sizeof(glm::vec2);
                    case VK_FORMAT_R32G32B32_SFLOAT:
                        return sizeof(glm::vec3);
                    case VK_FORMAT_R32G32B32A32_SFLOAT:
                        return sizeof(glm::vec4);
                    case VK_FORMAT_R32G32_SINT:
                        return sizeof(glm::ivec2);
                    case VK_FORMAT_R32G32B32_SINT:
                        return sizeof(glm::ivec3);
                    case VK_FORMAT_R32G32B32A32_SINT:
                        return sizeof(glm::ivec4);
                    case VK_FORMAT_R32G32_UINT:
                        return sizeof(glm::uvec2);
                    case VK_FORMAT_R32G32B32_UINT:
                        return sizeof(glm::uvec3);
                    case VK_FORMAT_R32G32B32A32_UINT:
                        return sizeof(glm::uvec4);    //Need uintvec?
                    case VK_FORMAT_R32_UINT:
                        return sizeof(u32);
                    default:
                        RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                        return 0;
                }

                return 0;
            }

            u32 PushBufferLayout(VkFormat format, const std::string& name, RZBufferLayout& layout)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                // TODO: Add buffer layout for all supported types
                switch (format) {
                    case VK_FORMAT_R8_SINT:
                        layout.push<int>(name);
                        break;
                    case VK_FORMAT_R32_SFLOAT:
                        layout.push<float>(name);
                        break;
                    case VK_FORMAT_R32G32_SFLOAT:
                        layout.push<glm::vec2>(name);
                        break;
                    case VK_FORMAT_R32G32B32_SFLOAT:
                        layout.push<glm::vec3>(name);
                        break;
                    case VK_FORMAT_R32G32B32A32_SFLOAT:
                        layout.push<glm::vec4>(name);
                        break;
                    default:
                        RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                        return 0;
                }

                return 0;
            }

            DescriptorType VKToEngineDescriptorType(SpvReflectDescriptorType type)
            {
                RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                switch (type) {
                    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                        return DescriptorType::kImageSamplerCombined;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        return DescriptorType::kUniformBuffer;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                        return DescriptorType::kTexture;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                        return DescriptorType::kSampler;
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                        return DescriptorType::kRWTexture;
                        break;
                    default:
                        RAZIX_CORE_ERROR("[VULKAN] SpvReflectDescriptorType is not resolved!");
                        return DescriptorType::kNone;
                        break;
                }

                // FIXME: Make this return something like NONE and cause a ASSERT_ERROR
                // return DescriptorType::kUniformBuffer;
                RAZIX_CORE_ERROR("[VULKAN] SpvReflectDescriptorType is not resolved!");
                return DescriptorType::kNone;
            }

            VkDescriptorType DescriptorTypeToVK(Razix::Gfx::DescriptorType descriptorType)
            {
                switch (descriptorType) {
                    case Razix::Gfx::DescriptorType::kUniformBuffer:
                        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case Razix::Gfx::DescriptorType::kImageSamplerCombined:
                        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                    default:
                        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case Razix::Gfx::DescriptorType::kTexture:
                        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                        break;
                    case Razix::Gfx::DescriptorType::kRWTexture:
                        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                        break;
                    case Razix::Gfx::DescriptorType::kSampler:
                        return VK_DESCRIPTOR_TYPE_SAMPLER;
                    case Razix::Gfx::DescriptorType::kRWTyped:
                    case Razix::Gfx::DescriptorType::kStructured:
                    case Razix::Gfx::DescriptorType::kRWStructured:
                    case Razix::Gfx::DescriptorType::kByteAddress:
                    case Razix::Gfx::DescriptorType::kRWByteAddress:
                    case Razix::Gfx::DescriptorType::kAppendStructured:
                    case Razix::Gfx::DescriptorType::kConsumeStructured:
                    case Razix::Gfx::DescriptorType::kRWStructuredCounter:
                    case Razix::Gfx::DescriptorType::kRTAccelerationStructure:
                        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;
                }
            }

            //-----------------------------------------------------------------------------------
            // Debug Utils
            //-----------------------------------------------------------------------------------

            void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer, const std::string& name, glm::vec4 color)
            {
                VkDebugUtilsLabelEXT label{};
                label.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                label.pLabelName = name.c_str();
                memcpy(label.color, &color[0], 4 * sizeof(f32));

                auto func = (PFN_vkCmdBeginDebugUtilsLabelEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkCmdBeginDebugUtilsLabelEXT");
                if (func != nullptr)
                    func(cmdBuffer, &label);
                //else
                //    RAZIX_CORE_ERROR("CmdBeginDebugUtilsLabelEXT Function not found");
            }

            void CmdInsertDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer, const std::string& name, glm::vec4 color)
            {
                VkDebugUtilsLabelEXT label{};
                label.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                label.pLabelName = name.c_str();
                memcpy(label.color, &color[0], 4 * sizeof(f32));

                auto func = (PFN_vkCmdInsertDebugUtilsLabelEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkCmdInsertDebugUtilsLabelEXT");
                if (func != nullptr)
                    func(cmdBuffer, &label);
                //else
                //    RAZIX_CORE_ERROR("CmdInsertDebugUtilsLabelEXT Function not found");
            }

            void CmdEndDebugUtilsLabelEXT(VkCommandBuffer cmdBuffer)
            {
                auto func = (PFN_vkCmdEndDebugUtilsLabelEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkCmdEndDebugUtilsLabelEXT");
                if (func != nullptr)
                    func(cmdBuffer);
                //else
                //    RAZIX_CORE_ERROR("CmdEndDebugUtilsLabelEXT Function not found");
            }

            VkResult CreateDebugObjName(const std::string& name, VkObjectType type, uint64_t handle)
            {
                RAZIX_CORE_ASSERT((handle != 0), "NULL HANDLE DETECTED!");

                VkDebugUtilsObjectNameInfoEXT info{};
                info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
                info.pObjectName  = name.c_str();
                info.objectType   = type;
                info.objectHandle = (uint64_t) handle;

                auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetInstanceProcAddr(VKContext::Get()->getInstance(), "vkSetDebugUtilsObjectNameEXT");
                if (func != nullptr)
                    return func(VKDevice::Get().getDevice(), &info);
                else
                    return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }    // namespace VKUtilities
    }        // namespace Gfx
}    // namespace Razix
