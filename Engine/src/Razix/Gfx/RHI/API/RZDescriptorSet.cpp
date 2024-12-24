// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDescriptorSet.h"

#include "Razix/Gfx/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#endif

#ifdef RAZIX_RENDER_API_DIRECTX12
    #include "Razix/Platform/API/DirectX12/DX12DescriptorSet.h"
#endif

namespace Razix {
    namespace Gfx {

        GET_INSTANCE_SIZE_IMPL(DescriptorSet);

        RZDescriptorSet* RZDescriptorSet::Create(void* where, const RZDescriptorSetDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        RZDescriptor::RZDescriptor(const RZDescriptor& other)
            : name(other.name), typeName(other.typeName), uboMembers(other.uboMembers), bindingInfo(other.bindingInfo), size(other.size), offset(other.offset)
        {
            uniformBuffer = other.uniformBuffer;
        }

        RZDescriptor& RZDescriptor::operator=(const RZDescriptor& other)
        {
            if (this != &other) {
                name        = other.name;
                typeName    = other.typeName;
                uboMembers  = other.uboMembers;
                bindingInfo = other.bindingInfo;
                size        = other.size;
                offset      = other.offset;

                uniformBuffer = other.uniformBuffer;
            }
            return *this;
        }

        RZDescriptorSet* RZDescriptorSet::Create(const std::vector<RZDescriptor>& descriptors RZ_DEBUG_NAME_TAG_E_ARG)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Gfx::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Gfx::RenderAPI::OPENGL: return new OpenGLDescriptorSet(descriptors); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Gfx::RenderAPI::VULKAN: return new (where) VKDescriptorSet(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Gfx::RenderAPI::D3D12: return new (where) DX12DescriptorSet(desc RZ_DEBUG_E_ARG_NAME); break;
#endif
                default:
                    return nullptr;
                    break;
            }
        }
    }    // namespace Gfx
}    // namespace Razix