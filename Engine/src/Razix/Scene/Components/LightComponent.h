#pragma once

//#include "Light.h"

namespace Razix {
    class RAZIX_API LightComponent
    {
    public:
        LightComponent()                      = default;
        LightComponent(const LightComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp());
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp());
        }
    };
}    // namespace Razix
