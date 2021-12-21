#include "rzxpch.h"
#include "RZDescriptorSet.h"

#include "Razix/Graphics/API/RZGraphicsContext.h"

namespace Razix {
    namespace Graphics {
    
        RZDescriptorSet* RZDescriptorSet::Create(const DescriptorLayoutBinding& layout)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:   
                case Razix::Graphics::RenderAPI::VULKAN:   
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
        }
    }
}