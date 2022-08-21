#pragma once

#include "Razix/Graphics/Lighting/RZLight.h"

namespace Razix {

    class RAZIX_API LightComponent
    {
    public:
        LightComponent()                      = default;
        LightComponent(const LightComponent&) = default;

        Graphics::RZLight light;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Light", light));
        }
    };
}    // namespace Razix