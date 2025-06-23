// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DX12Sampler.h"

#ifdef RAZIX_RENDER_API_DIRECTX12

namespace Razix {
    namespace Gfx {

        DX12Sampler::DX12Sampler(const RZSamplerDesc& desc RZ_DEBUG_NAME_TAG_E_ARG)
        {
        }

        RAZIX_CLEANUP_RESOURCE_IMPL(DX12Sampler)
        {
        }

    }    // namespace Gfx
}    // namespace Razix

#endif