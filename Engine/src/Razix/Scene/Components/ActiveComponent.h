#pragma once

#include <cereal/cereal.hpp>

namespace Razix {
    /**
     * Active component is used to tell whether the component is active or inactive in the scene
     */
    struct RAZIX_API ActiveComponent
    {
        bool Active = true;

        ActiveComponent()
            : Active(true) {}
        ActiveComponent(bool act)
            : Active(act) {}
        ActiveComponent(const ActiveComponent&) = default;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("isActive", Active));
        }
    };
}    // namespace Razix