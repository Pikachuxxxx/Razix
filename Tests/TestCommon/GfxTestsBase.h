#pragma once

#include <Razix.h>
#include <gtest/gtest.h>

namespace Razix {

#define TEST_APP_NUM_FRAMES_DEFAULT 120
#define TEST_APP_NUM_FRAMES_INF     INT32_MAX - 1

    /**
     * Base class for Graphics testing applications
     */
    class RZGfxTestAppBase : public RZApplication
    {
    public:
        RZGfxTestAppBase(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const std::string& appName = "RazixGfxTestApp")
            : RZApplication(projectRoot, appName), m_NumFrames(numFrames), m_CurrentFrame(0)
        {
            Razix::RZInput::SelectGLFWInputManager();
            RZApplication::Init();

            //-------------------------------------------------------------------------------------
            // Override the Graphics API here! for testing
#ifdef RAZIX_PLATFORM_WINDOWS
            rzGfxCtx_SetRenderAPI(RZ_RENDER_API_D3D12);
#elif defined RAZIX_PLATFORM_MACOS
            rzGfxCtx_SetRenderAPI(RZ_RENDER_API_VULKAN);
#endif
            //-------------------------------------------------------------------------------------

            // Mount the tests root directory to load test specific resources
            RZVirtualFileSystem::Get().mount("TestsRoot", projectRoot);

            RZFileSystem::CreateDir(projectRoot + "/GfxTests/TestImages/");
        }

        virtual ~RZGfxTestAppBase() {}

        void OnStart() override
        {
        }

        void OnRender() override
        {
            RAZIX_INFO("Current Frame: {0}", m_CurrentFrame);

            if (m_CurrentFrame == 10) {
                RZEngine::Get().getWorldRenderer().setReadbackSwapchainThisFrame();
            }

            if (m_CurrentFrame >= m_NumFrames) {
                // Request app to close
                RZApplication::Get().setAppState(AppState::Closing);
            }
            m_CurrentFrame++;
        }

        void OnQuit() override
        {
            m_SwapchainReadback = RZEngine::Get().getWorldRenderer().getSwapchainReadbackPtr();

            if (!WriteScreenshot()) RAZIX_ERROR("Failed to write swapchain capture readback texture!");
        }

        void  SetGoldenImagePath(const std::string& path);
        void  SetScreenshotPath(const std::string& path);
        float CompareWithGoldenImage();

    protected:
        i32                            m_NumFrames;
        i32                            m_CurrentFrame;
        std::string                    m_GoldenImagePath;
        std::string                    m_ScreenShotPath;
        const rz_gfx_texture_readback* m_SwapchainReadback;

    private:
        bool  WriteScreenshot();
        float CalculatePSNR(const std::string& capturedImagePath, const std::string& goldenImagePath);
    };
}    // namespace Razix

static int EngineTestLoop(void)
{
    //-------------------------------------------------------------------------------------
    // Override the Graphics API here! for testing
#ifdef RAZIX_PLATFORM_WINDOWS
    rzGfxCtx_SetRenderAPI(RZ_RENDER_API_D3D12);
#elif defined RAZIX_PLATFORM_MACOS
    rzGfxCtx_SetRenderAPI(RZ_RENDER_API_VULKAN);
#endif
    //-------------------------------------------------------------------------------------

    Razix::RZEngine::Get().setEngineInTestMode();
    EngineMain(0, NULL);

    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    int result = EngineExit();
    return result;
}
