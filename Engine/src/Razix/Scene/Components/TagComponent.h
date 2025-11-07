#pragma once

#include <cereal/cereal.hpp>

namespace Razix {
    /**
     * A Tag components allows an entity to have a name
     */
    struct RAZIX_API TagComponent
    {
        /* Name of the entity */
        RZString Tag;

        TagComponent()                    = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const RZString& tag)
            : Tag(tag) {}

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Tag", Tag));
        }
    };
}    // namespace Razix