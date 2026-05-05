#include <Razix.h>

#include "TanuBrain.h"

using namespace Razix;

class TanuGameApp : public Razix::RZApplication
{
public:
    TanuGameApp()
        : RZApplication(RZString(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + RZString("/Tanu/")), "Tanu")
    {
        RZApplication::Init();
    }

    void OnStart() override
    {
        // Hook up the Tanu brain thought feed — fetches the latest thought
        // from the tanu-brain GitHub repo every TANU_THOUGHT_FETCH_INTERVAL_MS
        // and prints it to console (even in distribution builds via printf).
        tanu_brain_init();
    }

    void OnUpdate(const RZTimestep& dt) override
    {
        // Tick the Tanu brain module each frame: prints any new thought that
        // arrived from the worker job and schedules the next timed fetch.
        tanu_brain_update();
    }

    void OnRender() override
    {
    }

    void OnQuit() override
    {
        // Wait for any in-flight fetch job to finish before shutdown.
        tanu_brain_shutdown();
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Tanu Application");
    return new TanuGameApp();
}

RAZIX_PLATFORM_MAIN
