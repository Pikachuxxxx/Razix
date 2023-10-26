// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKUtilities.h"

#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"

namespace Razix {
    namespace Graphics {
        namespace VKUtilities {

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
                            return VK_FORMAT_R8G8_UNORM;
                            break;
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
                        case TextureFormat::SCREEN: // Is always sRGB
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
                    case Razix::Graphics::ImageLayout::kUndefined:
                        return VK_IMAGE_LAYOUT_UNDEFINED;
                        break;
                    case Razix::Graphics::ImageLayout::kGeneral:
                        return VK_IMAGE_LAYOUT_GENERAL;
                        break;
                    case Razix::Graphics::ImageLayout::kColorAttachmentOptimal:
                        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                        break;
                    case Razix::Graphics::ImageLayout::kDepthStencilAttachmentOptimal:
                        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                        break;
                    case Razix::Graphics::ImageLayout::kDepthStencilReadOnlyOptimal:
                        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                        break;
                    case Razix::Graphics::ImageLayout::kShaderReadOnlyOptimal:
                        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        break;
                    case Razix::Graphics::ImageLayout::kTransferSrcOptimal:
                        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        break;
                    case Razix::Graphics::ImageLayout::kTransferDstOptimal:
                        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                        break;
                    case Razix::Graphics::ImageLayout::kPresentationEngine:
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
                    case Razix::Graphics::MemoryAccessMask::kNone:
                        return VK_ACCESS_NONE;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kIndirectCommandReadBit:
                        return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kIndexBufferDataReadBit:
                        return VK_ACCESS_INDEX_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kVertexAttributeReadBit:
                        return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kUniformReadBit:
                        return VK_ACCESS_UNIFORM_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kInputAttachmentReadBit:
                        return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kShaderReadBit:
                        return VK_ACCESS_SHADER_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kShaderWriteBit:
                        return VK_ACCESS_SHADER_WRITE_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kColorAttachmentReadBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kColorAttachmentWriteBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kColorAttachmentReadWriteBit:
                        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kDepthStencilAttachmentReadBit:
                        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kDepthStencilAttachmentWriteBit:
                        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kTransferReadBit:
                        return VK_ACCESS_TRANSFER_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kTransferWriteBit:
                        return VK_ACCESS_TRANSFER_WRITE_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kHostReadBit:
                        return VK_ACCESS_HOST_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kHostWriteBit:
                        return VK_ACCESS_HOST_WRITE_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kMemoryReadBit:
                        return VK_ACCESS_MEMORY_READ_BIT;
                        break;
                    case Razix::Graphics::MemoryAccessMask::kMemoryWriteBit:
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
                    case Razix::Graphics::PipelineStage::kTopOfPipe:
                        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kDrawIndirect:
                        return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kDraw:
                        return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kVertexInput:
                        return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kVertexShader:
                        return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kTessellationControlShader:
                        return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kTessellationEvaluationShader:
                        return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kGeometryShader:
                        return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kFragmentShader:
                        return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kEarlyFragmentTests:
                        return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kLateFragmentTests:
                        return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kEarlyOrLateTests:
                        return (VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
                        break;
                    case Razix::Graphics::PipelineStage::kColorAttachmentOutput:
                        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kComputeShader:
                        return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kTransfer:
                        return VK_PIPELINE_STAGE_TRANSFER_BIT;
                        break;
                    case Razix::Graphics::PipelineStage::kMeshShader:
                        return VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
                        break;
                    case Razix::Graphics::PipelineStage::kTaskShader:
                        return VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
                        break;
                    case Razix::Graphics::PipelineStage::kBottomOfPipe:
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
                VkCommandBufferAllocateInfo allocInfo = {};
                allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocInfo.commandPool                 = VKDevice::Get().getCommandPool()->getVKPool();
                allocInfo.commandBufferCount          = 1;

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

                vkFreeCommandBuffers(VKDevice::Get().getDevice(), VKDevice::Get().getCommandPool()->getVKPool(), 1, &commandBuffer);
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

            VkPrimitiveTopology DrawTypeToVK(Razix::Graphics::DrawType type)
            {
                switch (type) {
                    case Razix::Graphics::DrawType::Point:
                        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                        break;
                    case Razix::Graphics::DrawType::Triangle:
                        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                        break;
                    case Razix::Graphics::DrawType::Line:
                        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown Draw Type! using triangle list to draw the geometry");
                        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                        break;
                }
            }

            VkCullModeFlags CullModeToVK(Razix::Graphics::CullMode cullMode)
            {
                switch (cullMode) {
                    case Razix::Graphics::CullMode::Front:
                        return VK_CULL_MODE_FRONT_BIT;
                        break;
                    case Razix::Graphics::CullMode::Back:
                        return VK_CULL_MODE_BACK_BIT;
                        break;
                    case Razix::Graphics::CullMode::FrontBack:
                        return VK_CULL_MODE_FRONT_AND_BACK;
                        break;
                    case Razix::Graphics::CullMode::None:
                        return VK_CULL_MODE_NONE;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown Cull Mode! Using Back Face Culling by default");
                        return VK_CULL_MODE_BACK_BIT;
                        break;
                }
            }

            VkPolygonMode PolygoneModeToVK(Razix::Graphics::PolygonMode polygonMode)
            {
                switch (polygonMode) {
                    case Razix::Graphics::PolygonMode::Fill:
                        return VK_POLYGON_MODE_FILL;
                        break;
                    case Razix::Graphics::PolygonMode::Line:
                        return VK_POLYGON_MODE_LINE;
                        break;
                    case Razix::Graphics::PolygonMode::Point:
                        return VK_POLYGON_MODE_POINT;
                        break;
                    default:
                        RAZIX_CORE_WARN("Unknown polygon mode! Using fill by default");
                        return VK_POLYGON_MODE_FILL;
                        break;
                }
            }

            VkBlendOp BlendOpToVK(Razix::Graphics::BlendOp blendOp)
            {
                switch (blendOp) {
                    case Razix::Graphics::BlendOp::Add:
                        return VK_BLEND_OP_ADD;
                        break;
                    case Razix::Graphics::BlendOp::Subtract:
                        return VK_BLEND_OP_SUBTRACT;
                        break;
                    case Razix::Graphics::BlendOp::ReverseSubtract:
                        return VK_BLEND_OP_REVERSE_SUBTRACT;
                        break;
                    case Razix::Graphics::BlendOp::Min:
                        return VK_BLEND_OP_MIN;
                        break;
                    case Razix::Graphics::BlendOp::Max:
                        return VK_BLEND_OP_MAX;
                        break;
                    default:
                        return VK_BLEND_OP_ADD;
                        break;
                }
                return VK_BLEND_OP_ADD;
            }

            VkBlendFactor BlendFactorToVK(Razix::Graphics::BlendFactor blendFactor)
            {
                switch (blendFactor) {
                    case Razix::Graphics::BlendFactor::Zero:
                        return VK_BLEND_FACTOR_ZERO;
                        break;
                    case Razix::Graphics::BlendFactor::One:
                        return VK_BLEND_FACTOR_ONE;
                        break;
                    case Razix::Graphics::BlendFactor::SrcColor:
                        return VK_BLEND_FACTOR_SRC_COLOR;
                        break;
                    case Razix::Graphics::BlendFactor::OneMinusSrcColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
                        break;
                    case Razix::Graphics::BlendFactor::DstColor:
                        return VK_BLEND_FACTOR_DST_COLOR;
                        break;
                    case Razix::Graphics::BlendFactor::OneMinusDstColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
                        break;
                    case Razix::Graphics::BlendFactor::SrcAlpha:
                        return VK_BLEND_FACTOR_SRC_ALPHA;
                        break;
                    case Razix::Graphics::BlendFactor::OneMinusSrcAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        break;
                    case Razix::Graphics::BlendFactor::DstAlpha:
                        return VK_BLEND_FACTOR_DST_ALPHA;
                        break;
                    case Razix::Graphics::BlendFactor::OneMinusDstAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
                        break;
                    case Razix::Graphics::BlendFactor::ConstantColor:
                        return VK_BLEND_FACTOR_CONSTANT_COLOR;
                        break;
                    case BlendFactor::OneMinusConstantColor:
                        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
                        break;
                    case Razix::Graphics::BlendFactor::ConstantAlpha:
                        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
                        break;
                    case Razix::Graphics::BlendFactor::OneMinusConstantAlpha:
                        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
                        break;
                    case Razix::Graphics::BlendFactor::SrcAlphaSaturate:
                        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
                        break;
                    default:
                        return VK_BLEND_FACTOR_ONE;
                        break;
                }
                return VK_BLEND_FACTOR_ONE;
            }

            VkCompareOp CompareOpToVK(Razix::Graphics::CompareOp compareOp)
            {
                switch (compareOp) {
                    case Razix::Graphics::CompareOp::Never:
                        return VK_COMPARE_OP_NEVER;
                        break;
                    case Razix::Graphics::CompareOp::Less:
                        return VK_COMPARE_OP_LESS;
                        break;
                    case Razix::Graphics::CompareOp::Equal:
                        return VK_COMPARE_OP_EQUAL;
                        break;
                    case Razix::Graphics::CompareOp::LessOrEqual:
                        return VK_COMPARE_OP_LESS_OR_EQUAL;
                        break;
                    case Razix::Graphics::CompareOp::Greater:
                        return VK_COMPARE_OP_GREATER;
                        break;
                    case Razix::Graphics::CompareOp::NotEqual:
                        return VK_COMPARE_OP_NOT_EQUAL;
                        break;
                    case Razix::Graphics::CompareOp::GreaterOrEqual:
                        return VK_COMPARE_OP_GREATER_OR_EQUAL;
                        break;
                    case Razix::Graphics::CompareOp::Always:
                        return VK_COMPARE_OP_ALWAYS;
                        break;
                    default:
                        return VK_COMPARE_OP_LESS_OR_EQUAL;
                        break;
                }
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            }

            VkDescriptorType DescriptorTypeToVK(Razix::Graphics::DescriptorType descriptorType)
            {
                switch (descriptorType) {
                    case Razix::Graphics::DescriptorType::UniformBuffer:
                        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case Razix::Graphics::DescriptorType::ImageSamplerCombined:
                        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                    default:
                        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                }
            }

            VkShaderStageFlagBits ShaderStageToVK(Razix::Graphics::ShaderStage stage)
            {
                switch (stage) {
                    case Razix::Graphics::ShaderStage::NONE:
                        return VK_SHADER_STAGE_ALL;
                        break;
                    case Razix::Graphics::ShaderStage::Vertex:
                        return VK_SHADER_STAGE_VERTEX_BIT;
                        break;
                    case Razix::Graphics::ShaderStage::Pixel:
                        return VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    case Razix::Graphics::ShaderStage::Geometry:
                        return VK_SHADER_STAGE_GEOMETRY_BIT;
                        break;
                    case Razix::Graphics::ShaderStage::TCS:
                        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                        break;
                    case Razix::Graphics::ShaderStage::TES:
                        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                        break;
                    case Razix::Graphics::ShaderStage::Compute:
                        return VK_SHADER_STAGE_COMPUTE_BIT;
                        break;
                    default:
                        return VK_SHADER_STAGE_ALL_GRAPHICS;
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
                RAZIX_CORE_ASSERT((handle != NULL), "NULL HANDLE DETECTED!");

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
    }        // namespace Graphics
}    // namespace Razix
