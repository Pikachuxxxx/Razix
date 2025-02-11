#include "GfxTestsBase.h"

namespace Razix {

    static void write_texture_readback_to_ppm(Gfx::TextureReadback* texture, const char* filename)
    {
        if (!texture || !texture->data || texture->bits_per_pixel != 32) {
            return;
        }

        FILE* file = fopen(filename, "wb");
        if (!file) {
            RAZIX_ERROR("Failed to open file: %s", filename);
            return;
        }

        // PPM header
        fprintf(file, "P6\n%u %u\n255\n", texture->width, texture->height);

        for (uint32_t y = 0; y < texture->height; ++y) {
            for (uint32_t x = 0; x < texture->width; ++x) {
                uint32_t index = (y * texture->width + x) * 4;
                // BGRA8_UNORM format
                uint8_t* pixels = (uint8_t*) texture->data;
                uint8_t  b      = (pixels[index + 0]);
                uint8_t  g      = (pixels[index + 1]);
                uint8_t  r      = (pixels[index + 2]);
                //uint8_t  a    = (uint8_t) ((texture->pixels[index + 3] / 255.0f) * 255);
                fwrite(&r, 1, 1, file);
                fwrite(&g, 1, 1, file);
                fwrite(&b, 1, 1, file);
            }
        }

        fclose(file);
    }

    void RZGfxTestAppBase::SetGoldenImagePath(const std::string& path)
    {
        m_GoldenImagePath = path;
    }

    void RZGfxTestAppBase::SetScreenshotPath(const std::string& path)
    {
        m_ScreenShotPath = path;
    }

    float RZGfxTestAppBase::CompareWithGoldenImage()
    {
        if (m_GoldenImagePath.empty() || m_ScreenShotPath.empty()) {
            RAZIX_ERROR("Golden/Screenshot image path is not set!");
            return -1.0f;
        }

        float psnr = 0.0f;
        psnr       = CalculatePSNR(m_ScreenShotPath, m_GoldenImagePath);
        return psnr;
    }

    bool RZGfxTestAppBase::WriteScreenshot()
    {
        bool success = false;
        if (m_SwapchainReadback.data) {
            write_texture_readback_to_ppm(&m_SwapchainReadback, m_ScreenShotPath.c_str());
            success = true;
        }

        return success;
    }

    float RZGfxTestAppBase::CalculatePSNR(const std::string& capturedImagePath, const std::string& goldenImagePath)
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
        if (mse == 0) return 0.0f;    // Images are identical
        float maxPixelValue = 255.0f;                                   // For 8-bit images
        float psnr          = 10.0f * std::log10((maxPixelValue * maxPixelValue) / mse);
        return psnr;
    }
}    // namespace Razix