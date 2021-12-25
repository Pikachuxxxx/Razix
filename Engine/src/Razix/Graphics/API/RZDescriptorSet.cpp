#include "rzxpch.h"
#include "RZDescriptorSet.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#endif

namespace Razix {
    namespace Graphics {
    
        RZDescriptorSet* RZDescriptorSet::Create(const std::vector<RZDescriptor>& descriptors)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:   
                case Razix::Graphics::RenderAPI::VULKAN:        return new VKDescriptorSet(descriptors); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }
    }
}