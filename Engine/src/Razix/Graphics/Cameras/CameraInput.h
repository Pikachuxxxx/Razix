#pragma once

#include "Razix/Core/RZCore.h"

#include "Razix/Core/RZRoot.h"

namespace Razix {
    namespace Graphics {

        /**
         * Controls the Camera Input to update it by interacting with the Engine Input system 
         */
        class RAZIX_API CameraInput : public RZRoot
        {
        public:
            CameraInput()          = default;
            virtual ~CameraInput() = default;

        private:
        };
    }    // namespace Graphics
}    // namespace Razix