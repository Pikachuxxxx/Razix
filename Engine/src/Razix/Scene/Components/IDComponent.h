#pragma once

#include "Razix/Core/UUID/RZUUID.h"

#include <cereal/cereal.hpp>

namespace Razix {

    /**
     * An ID component contains an UUID that helps us to identify the entity uniquely in a scene
     */
    struct RAZIX_API IDComponent
    {
        /* Used to uniquely identify the entity */
        RZUUID UUID;

        IDComponent()                   = default;
        IDComponent(const IDComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            std::string uuid_string;
            archive(cereal::make_nvp("UUID", uuid_string));
            UUID = RZUUID(uuid_string);
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("UUID", UUID.bytes()));
        }
    };
}    // namespace Razix