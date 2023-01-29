// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSoundEngine.h"

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>    // Streaming Manager n

#include <AK/Tools/Common/AkPlatformFuncs.h>    // Thread defines

namespace Razix {
    namespace Audio {

        void RZSoundEngine::StartUp()
        {
            RAZIX_CORE_INFO("[Sound Engine] Starting Up Sound Engine");
            RAZIX_CORE_INFO("[Sound Engine] Backend: OpenAL");

            // Initialize the WWise : Audio Kinetic Sound Engine Memory
            AkMemSettings memSettings;
            AK::MemoryMgr::GetDefaultSettings(memSettings);
            RAZIX_CORE_ASSERT((AK::MemoryMgr::Init(&memSettings) == AK_Success), "[Sound Engine] Could not create the memory manager");

            //
            // Create and initialize an instance of the default streaming manager. Note
            // that you can override the default streaming manager with your own.
            //

            // Customize the Stream Manager settings here.
            AkStreamMgrSettings stmSettings;
            AK::StreamMgr::GetDefaultSettings(stmSettings);

            RAZIX_CORE_ASSERT((AK::StreamMgr::Create(stmSettings)), "[Sound Engine] Could not create the Streaming Manager");

            //
            // Create a streaming device with blocking low-level I/O handshaking.
            // Note that you can override the default low-level I/O module with your own.
            //

            AkDeviceSettings deviceSettings;
            AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

            //
            // Create the Sound Engine
            // Using default initialization parameters
            //

            AkInitSettings         initSettings;
            AkPlatformInitSettings platformInitSettings;
            AK::SoundEngine::GetDefaultInitSettings(initSettings);
            AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

            RAZIX_CORE_ASSERT(AK::SoundEngine::Init(&initSettings, &platformInitSettings) == AK_Success, "[Sound Engine] Could not initialize the Sound Engine")
        }

        void RZSoundEngine::ShutDown()
        {
            RAZIX_CORE_INFO("[Sound Engine] Shutting Down Sound Engine");
        }

        void RZSoundEngine::renderAudio()
        {
            AK::SoundEngine::RenderAudio();
        }

    }    // namespace Audio
}    // namespace Razix