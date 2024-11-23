#pragma once

#include "Razix/Gfx/Lighting/RZLight.h"

#include <cereal/cereal.hpp>

namespace Razix {

    class RAZIX_API LightComponent
    {
    public:
        LightComponent()                      = default;
        LightComponent(const LightComponent&) = default;

        Gfx::RZLight light;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Light", light));
        }
    };
}    // namespace Razix