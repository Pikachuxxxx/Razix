#pragma once

#ifdef RAZIX_RENDER_API_VULKAN

    #include "Razix/Gfx/RHI/API/RZTexture.h"

    #include "Razix/Platform/API/Vulkan/VKBuffer.h"
    #include "Razix/Platform/API/Vulkan/VKDevice.h"

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
                if (uav != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), uav, nullptr);

                if (srv != VK_NULL_HANDLE)
                    vkDestroyImageView(VKDevice::Get().getDevice(), srv, nullptr);

                if (rtv != VK_NULL_HANDLE)
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
            virtual void    GenerateMips() override;
            virtual void    UploadToBindlessSet() override;
            virtual int32_t ReadPixels(u32 x, u32 y) override;

            void transitonImageLayoutToSRV();
            void transitonImageLayoutToUAV();

            inline VkImageLayout getImageLayout() const { return m_ImageLayout; }
            // TODO: call it overrideImageLayoutValue(...) since it has no actual responsibility?
            inline void        setImageLayout(VkImageLayout layout) { m_ImageLayout = layout; }
            inline VkImage     getImage() const { return m_Image; };
            inline VkImageView getFullSRVImageView() const { return m_FullResourceView.srv; }
            inline VkImageView getFullUAVImageView() const { return m_FullResourceView.uav; }
            inline VkImageView getFullDSVImageView() const { return m_FullResourceView.dsv; }
            inline VkImageView getFullRTVImageView() const { return m_FullResourceView.rtv; }
            inline VkImageView getLayerMipSRVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].srv; }
            inline VkImageView getLayerMipUAVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].uav; }
            inline VkImageView getLayerMipDSVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].dsv; }
            inline VkImageView getLayerMipRTVImageView(u32 layer, u32 mip) const { return m_LayerMipResourceViews[layer][mip].rtv; }
            RAZIX_DEPRECATED("VkSampler is depricated in VKTexture class, it will be separated soon!")
            inline VkSampler      getSampler() const { return m_ImageSampler; }
            inline VkDeviceMemory getDeviceMemory() const { return m_ImageMemory; }
    #if RAZIX_USE_VMA
            inline VmaAllocation getVMAAllocation() const { return m_VMAAllocation; }
    #endif

        private:
            VkImage m_Image = VK_NULL_HANDLE;
            RAZIX_DEPRECATED("vkSampler is depricated in VKTexture class, it will be separated soon!")
            VkSampler             m_ImageSampler                                                            = VK_NULL_HANDLE;
            VKImageResourceView   m_FullResourceView                                                        = {};
            VKImageResourceView   m_LayerMipResourceViews[RAZIX_MAX_TEXTURE_LAYERS][RAZIX_MAX_TEXTURE_MIPS] = {};
            VkImageLayout         m_ImageLayout                                                             = VK_IMAGE_LAYOUT_UNDEFINED;
            bool                  m_DeleteImageData                                                         = false;
            VkImageAspectFlagBits m_AspectBit                                                               = VK_IMAGE_ASPECT_NONE;
            VkDeviceMemory        m_ImageMemory                                                             = VK_NULL_HANDLE;
    #if RAZIX_USE_VMA
            VmaAllocation m_VMAAllocation = {};
    #endif

        private:
            // specialized constructor for swapchain class only
            friend class VKSwapchain;
            VKTexture(VkImage image, VkImageView imageView);

            // TODO: split into more does one thing functions
            void initializeBackendHandles(const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            void initMipViewsPerFace();
            void loadImageDataFromFile();
        };
    }    // namespace Gfx
}    // namespace Razix
#endif
