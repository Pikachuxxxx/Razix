#pragma once

#include <Razix.h>
#include <gtest/gtest.h>

namespace Razix {

#define TEST_APP_NUM_FRAMES_DEFAULT 120

    /**
     * Base class for Graphics testing applications
     */
    class RZGfxTestAppBase : public RZApplication
    {
    public:
        RZTestApplication(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const std::string& appName = "Razix Gfx Test App")
            : RZApplication(projectRoot, appName), m_NumFrames(numFrames), m_CurrentFrame(0)
        {
            RZApplication::Init();
        }

        virtual ~RZTestApplication() {}

        bool RenderFrame() override
        {
            if (m_CurrentFrame >= m_NumFrames) {
                // Request app to close
                RZApplication::Get().setAppState(AppState::Closing);
            }

            bool result = RZApplication::Get().RenderFrame();
            m_CurrentFrame++;

            return result;
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
            for (int i = 0; i < width * height; i++) {
                float diff = capturedImageData.data[i] - goldenImageData.data[i];
                mse += diff * diff;
            }

            mse /= (width * height);

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
