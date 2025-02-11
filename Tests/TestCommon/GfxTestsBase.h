#pragma once

#include <Razix.h>
#include <gtest/gtest.h>

namespace Razix {

#define TEST_APP_NUM_FRAMES_DEFAULT 120
#define TEST_APP_NUM_FRAMES_INF     UINT32_MAX

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
            Razix::Gfx::RZGraphicsContext::SetRenderAPI(Razix::Gfx::RenderAPI::VULKAN);
#elif defined RAZIX_PLATFORM_MACOS
            Razix::Gfx::RZGraphicsContext::SetRenderAPI(Razix::Gfx::RenderAPI::VULKAN);
#endif
            //-------------------------------------------------------------------------------------

            // Init Graphics Context
            //-------------------------------------------------------------------------------------
            // Creating the Graphics Context and Initialize it
            RAZIX_CORE_INFO("Creating Graphics Context...");
            Razix::Gfx::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
            RAZIX_CORE_INFO("Initializing Graphics Context...");
            Razix::Gfx::RZGraphicsContext::GetContext()->Init();
            //-------------------------------------------------------------------------------------

            // Mount the tests root directory to load test specific resources
            RZVirtualFileSystem::Get().mount("TestsRoot", projectRoot);
        }

        virtual ~RZGfxTestAppBase() {}

        void OnRender() override final
        {
            if (m_CurrentFrame >= m_NumFrames) {
                // Request app to close
                RZApplication::Get().setAppState(AppState::Closing);
            }
            m_CurrentFrame++;
        }

        void SetGoldenImagePath(const std::string& path)
        {
            m_GoldenImagePath = path;
        }

        float CalculatePSNR(const std::string& capturedImagePath, const std::string& goldenImagePath)
        {
            u32 capturedImgWidth  = 0;
            u32 capturedImgHeight = 0;
            u32 capturedImgBpp    = 0;
            u32 goldenImgWidth    = 0;
            u32 goldenImgHeight   = 0;
            u32 goldenImgBpp      = 0;

            u8* capturedImageData = Razix::Utilities::LoadImageData(capturedImagePath, &capturedImgWidth, &capturedImgHeight, &capturedImgBpp);
            u8* goldenImageData   = Razix::Utilities::LoadImageData(goldenImagePath, &goldenImgWidth, &goldenImgHeight, &goldenImgBpp);

            float mse = 0.0f;

            // Compute MSE (Mean Squared Error)
            for (uint32_t i = 0; i < capturedImgWidth * capturedImgHeight; i++) {
                float diff = (float) capturedImageData[i] - (float) goldenImageData[i];
                mse += diff * diff;
            }

            mse /= (capturedImgWidth * capturedImgHeight);

            // Calculate PSNR (Peak Signal-to-Noise Ratio)
            if (mse == 0) return std::numeric_limits<float>::infinity();    // Images are identical
            float maxPixelValue = 255.0f;                                   // For 8-bit images
            float psnr          = 10.0f * std::log10((maxPixelValue * maxPixelValue) / mse);
            return psnr;
        }

        float CompareWithGoldenImage()
        {
            if (m_GoldenImagePath.empty()) {
                RAZIX_ERROR("Golden image path is not set!");
                return -1.0f;
            }

            float psnr = 0.0f;
            psnr       = CalculatePSNR(m_ScreenShotPath, m_GoldenImagePath);
            return psnr;
        }

        bool TakeScreenshot()
        {
            bool success = false;
            // TODO: Use RHI to capture and readback the u8* imagedata to m_ScreenShotPath location
            return success;
        }

    protected:
        int         m_NumFrames;       /* Number of frames to run for the test          */
        int         m_CurrentFrame;    /* Current frame number in the test              */
        std::string m_GoldenImagePath; /* Path to the golden image for comparison       */
        std::string m_ScreenShotPath;  /* Path to the screenshot image for comparison   */
    };
}    // namespace Razix

static int EngineTestLoop(void)
{
    EngineMain(0, NULL);
    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    int result = EngineExit();
    return result;
}