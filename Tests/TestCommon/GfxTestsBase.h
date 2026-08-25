#pragma once

#include <Razix.h>
#include <gtest/gtest.h>

namespace Razix {

    extern int    g_argc;
    extern char** g_argv;

#define TEST_APP_NUM_FRAMES_DEFAULT 120
#define TEST_APP_NUM_FRAMES_INF     INT32_MAX - 1
#define RAZIX_TEST_PSNR_THRESHOLD   20.0f    // PSNR threshold value to consider a test as passed in dB

    /**
     * Base class for Graphics testing applications
     */
    class RZGfxTestAppBase : public RZApplication
    {
    public:
        RZGfxTestAppBase(const RZString& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const RZString& appName = "RazixGfxTestApp")
            : RZApplication(projectRoot, appName), m_NumFrames(numFrames)
        {
            RZApplication::Init();

            // Mount the tests root directory to load test specific resources
            RZVirtualFileSystem::Get().mount("TestsRoot", projectRoot);

            RZFileSystem::CreateDir(projectRoot + "/GfxTests/TestImages/");
        }

        virtual ~RZGfxTestAppBase() {}

        void OnStart() override
        {
        }

        void OnUpdate(const RZTimestep& dt) override
        {
            RAZIX_UNUSED(dt);
            u64 currFrameCount = RZEngine::Get().getWorldRenderer().getFrameCount();
            // RAZIX_INFO("Current Frame: {0}", currFrameCount);

            if (currFrameCount == 10) {
                RZEngine::Get().getWorldRenderer().setReadbackSwapchainThisFrame();
            }

            if (currFrameCount >= m_NumFrames) {
                // Request app to close
                RZApplication::Get().setAppState(AppState::kClosing);
            }
        }

        void OnQuit() override
        {
            m_SwapchainReadback = RZEngine::Get().getWorldRenderer().getSwapchainReadbackPtr();

            if (!WriteScreenshot()) RAZIX_ERROR("Failed to write swapchain capture readback texture!");
        }

        void  SetGoldenImagePath(const RZString& path);
        void  SetScreenshotPath(const RZString& path);
        float CompareWithGoldenImage();

    protected:
        i32                            m_NumFrames;
        RZString                       m_GoldenImagePath;
        RZString                       m_ScreenShotPath;
        const rz_gfx_texture_readback* m_SwapchainReadback;

    private:
        bool  WriteScreenshot();
        float CalculatePSNR(const RZString& capturedImagePath, const RZString& goldenImagePath);
    };
}    // namespace Razix

static int EngineTestLoop(void)
{
    Razix::RZEngine::Get().setEngineInTestMode();
    EngineMain(Razix::g_argc, Razix::g_argv);

    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    int result = EngineExit();
    return result;
}
