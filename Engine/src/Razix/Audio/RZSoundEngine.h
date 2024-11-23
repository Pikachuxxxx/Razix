#pragma once

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Audio {

        // TODO: add OpenAL stuff

        class RAZIX_API RZSoundEngine : public RZSingleton<RZSoundEngine>
        {
        public:
            void StartUp();
            void ShutDown();

            void renderAudio();
        };
    }    // namespace Audio
}    // namespace Razix
