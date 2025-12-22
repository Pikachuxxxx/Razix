#pragma once

#include "Razix/Gfx/Cameras/RZCamera3D.h"
#include <cereal/cereal.hpp>

namespace Razix {

    /**
     * The camera component attaches a camera to the entity that can be used to view the world from
     */
    struct RAZIX_API CameraComponent
    {
        RZCamera3D Camera;
        bool          Primary = true;    // TODO: think about moving this to Scene

        CameraComponent()                       = default;
        CameraComponent(const CameraComponent&) = default;

        template<class Archive>
        void serialize(Archive& archive)
        {
        }
    };

}    // namespace Razix
