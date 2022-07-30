#pragma once

#include "Razix/Scene/RZSceneCamera.h"

namespace Razix {

    /**
     * The camera component attaches a camera to the entity that can be used to view the world from
     */
    struct RAZIX_API CameraComponent
    {
        RZSceneCamera Camera;
        bool          Primary = true;    // TODO: think about moving this to Scene

        CameraComponent()                       = default;
        CameraComponent(const CameraComponent&) = default;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("isPrimary", Primary));
            archive(cereal::make_nvp("Camera", Camera));
        }
    };

}    // namespace Razix