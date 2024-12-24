// clang-format off
#include "rzxpch.h"
// clang-format on
#include "VKSampler.h"

namespace Razix {
    namespace Gfx {

        VKSampler::VKSampler(const RZSamplerDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
            m_Desc = desc;

            createSamplerHandle();
        }

        //-------------------------------------------------------------------------------------------

        RAZIX_CLEANUP_RESOURCE_IMPL(VKSampler)
        {
            if (m_ImageSampler != VK_NULL_HANDLE)
                vkDestroySampler(VKDevice::Get().getDevice(), m_ImageSampler, nullptr);
        }

        void VKSampler::createSamplerHandle()
        {
            auto                             physicalDeviceProps = VKDevice::Get().getPhysicalDevice().get()->getProperties();
            VKUtilities::VKCreateSamplerDesc samplerDesc         = {};
            samplerDesc.magFilter                                = VKUtilities::TextureFilterToVK(m_Desc.filtering.magFilter);
            samplerDesc.minFilter                                = VKUtilities::TextureFilterToVK(m_Desc.filtering.minFilter);
            samplerDesc.anisotropyEnable                         = true;
            samplerDesc.minLod                                   = 0.0f;
            samplerDesc.maxLod                                   = m_Desc.maxLOD;    // Make sure this is safely set by the user based on width and height
            samplerDesc.maxAnisotropy                            = physicalDeviceProps.limits.maxSamplerAnisotropy;
            samplerDesc.modeU                                    = VKUtilities::TextureWrapToVK(m_Desc.wrapping);
            samplerDesc.modeV                                    = VKUtilities::TextureWrapToVK(m_Desc.wrapping);
            samplerDesc.modeW                                    = VKUtilities::TextureWrapToVK(m_Desc.wrapping);

            m_ImageSampler = VKUtilities::CreateImageSampler(samplerDesc RZ_DEBUG_NAME_TAG_STR_E_ARG(m_Desc.name));
        }
        //-----------------------------------------------------------------------------------
    }    // namespace Gfx
}    // namespace Razix
