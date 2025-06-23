#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Gfx/RHI/API/RZTexture.h"

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"
    #include "Razix/Platform/API/Vulkan/VKUtilities.h"

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Gfx {

        /**
         * Vulkan handles to maintain different views for the image
         */
        struct VKImageResourceView
        {
            VkImageView uav = VK_NULL_HANDLE;
            VkImageView srv = VK_NULL_HANDLE;
            VkImageView rtv = VK_NULL_HANDLE;    // for swapchain
            VkImageView dsv = VK_NULL_HANDLE;

            void destroy() const
            {
                // FIXME: Fix this nasty clean up bug
                if (uav != VK_NULL_HANDLE) {
                    vkDestroyImageView(VKDevice::Get().getDevice(), uav, nullptr);
                }

                if (srv != VK_NULL_HANDLE && uav != srv) {
                    vkDestroyImageView(VKDevice::Get().getDevice(), srv, nullptr);
                }

                if (rtv != VK_NULL_HANDLE && uav != srv && uav != rtv)
                    vkDestroyImageView(VKDevice::Get().getDevice(), rtv, nullptr);

                if (dsv != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), dsv, nullptr);
            }
        };

        /* Vulkan implementation of the RZTexture class */
        class VKTexture final : public RZTexture
        {
        public:
            VKTexture(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            //---------------------------------------
            /* Releases the IRZResource */
            RAZIX_CLEANUP_RESOURCE
            //---------------------------------------

            virtual void    Resize(u32 width, u32 height) override;
            virtual void*   GetAPIHandlePtr() const override { return (void*) &m_Image; }
            virtual void    GenerateMipsAndViews() override;
            virtual void    UploadToBindlessSet() override;
            virtual int32_t ReadPixels(u32 x, u32 y) override;

            void transitonImageLayoutToSRV();
            void transitonImageLayoutToUAV();

            inline VkImageAspectFlags getAspectFlags() const { return m_AspectFlags; }

            inline VkImageLayout getImageLayoutValue() const { return m_FinalImageLayout; }
            inline void          setImageLayoutValue(VkImageLayout layout) { m_FinalImageLayout = layout; }
            inline VkImage       getImage() const { return m_Image; };
            inline VkImageView   getFullSRVImageView() const { return m_FullResourceView.srv; }
            inline VkImageView   getFullUAVImageView() const { return m_FullResourceView.uav; }
            inline VkImageView   getFullDSVImageView() const { return m_FullResourceView.dsv; }
            inline VkImageView   getFullRTVImageView() const { return m_FullResourceView.rtv; }
            inline VkImageView   getLayerMipSRVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].srv; }
            inline VkImageView   getLayerMipUAVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].uav; }
            inline VkImageView   getLayerMipDSVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].dsv; }
            inline VkImageView   getLayerMipRTVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].rtv; }
            inline VkImageView   getMipSRVImageView(u32 mip) const { return m_PerMipResourceViews[mip].srv; }
            inline VkImageView   getMipUAVImageView(u32 mip) const { return m_PerMipResourceViews[mip].uav; }
            inline VkImageView   getMipDSVImageView(u32 mip) const { return m_PerMipResourceViews[mip].dsv; }
            inline VkImageView   getMipRTVImageView(u32 mip) const { return m_PerMipResourceViews[mip].rtv; }
    #if RAZIX_USE_VMA
            inline VmaAllocation getVMAAllocation() const { return m_ImageMemoryWrapper.vmaAllocation; }
    #else
            inline VkDeviceMemory getDeviceMemory() const { return m_ImageMemoryWrapper.nativeAllocation; }
    #endif

        private:
            VkImage                                  m_Image                                                     = VK_NULL_HANDLE;
            VKImageResourceView                      m_FullResourceView                                          = {};
            VKImageResourceView                      m_LayerMipResourceViews[RZ_TEX_MAX_LAYERS][RZ_MAX_TEX_MIPS] = {};
            VKImageResourceView                      m_PerMipResourceViews[RZ_MAX_TEX_MIPS]                      = {};
            VkImageLayout                            m_FinalImageLayout                                          = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageLayout                            m_OldImageLayout                                            = VK_IMAGE_LAYOUT_UNDEFINED;    // For internal state tracking only
            VkImageAspectFlags                       m_AspectFlags                                               = VK_IMAGE_ASPECT_NONE;
            VKUtilities::VKImageMemoryBackendWrapper m_ImageMemoryWrapper                                        = {};

        private:
            // specialized constructor for swapchain class only
            friend class VKSwapchain;
            VKTexture(VkImage image, VkImageView imageView);

            void               initializeBackendHandles(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            void               loadImageDataInfoFromFile();
            void               evaluateMipsCount();
            VkImageUsageFlags  resolveUsageFlags();
            VkImageCreateFlags resolveCreateFlags();
            VkImageAspectFlags resolveAspectFlags();
            void               createVkImageHandleWithDesc(VkImageCreateFlags flags, VkImageUsageFlags usageBits RZ_DEBUG_NAME_TAG_E_ARG);
            void               copyInitDataToGPU();
            void               loadImageDataFromFile();
            void               createSpecializedRWCubemapViews();
            void               createFullResourceViews();
            void               createMipViewsPerFace();
            void               destroyMipViewsPerFace();
            void               createMipViewsFullFace();
            void               destroyMipViewsFullFace();
            bool               isDepthFormat();
            bool               isRWImage();
        };
    }    // namespace Gfx
}    // namespace Razix
#endif
