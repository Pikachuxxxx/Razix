#include "GfxTestsBase.h"

namespace Razix {

    static void WiteSwapchainTextureReadbackToPPM(const rz_gfx_texture_readback* texture, const char* filename)
    {
        if (!texture || !texture->data || texture->bpp != 32) {
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

    static bool ReadPPM(const char* filename, uint32_t* width, uint32_t* height, uint8_t** pixels)
    {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            RAZIX_ERROR("Failed to open PPM file: %s\n", filename);
            return false;
        }

        // Read the PPM header (magic number, width, height, and max color value)
        char magic[3];
        if (fscanf(file, "%2s", magic) != 1 || magic[0] != 'P' || magic[1] != '6') {
            RAZIX_ERROR("Invalid PPM format in file: %s\n", filename);
            fclose(file);
            return false;
        }

        int ch;
        while ((ch = fgetc(file)) == '#') {
            while ((ch = fgetc(file)) != '\n' && ch != EOF);    // Skip to end of comment
        }

        if (fscanf(file, "%d %d", width, height) != 2) {
            RAZIX_ERROR("Failed to read image dimensions in file: %s\n", filename);
            fclose(file);
            return false;
        }

        int max_color_value;
        if (fscanf(file, "%d", &max_color_value) != 1 || max_color_value != 255) {
            RAZIX_ERROR("Invalid max color value in file: %s\n", filename);
            fclose(file);
            return false;
        }

        while ((ch = fgetc(file)) == '\n' || ch == ' ');

        *pixels = (uint8_t*) malloc(*width * *height * 3);
        if (!*pixels) {
            RAZIX_ERROR("Failed to allocate memory for pixel data\n");
            fclose(file);
            return false;
        }

        fread(*pixels, 1, *width * *height * 3, file);

        fclose(file);
        return true;
    }

    //-----------------------------------------------------------------------------------

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
        if (m_SwapchainReadback && m_SwapchainReadback->data) {
            WiteSwapchainTextureReadbackToPPM(m_SwapchainReadback, m_ScreenShotPath.c_str());
            success = true;
        }

        return success;
    }

    float RZGfxTestAppBase::CalculatePSNR(const std::string& capturedImagePath, const std::string& goldenImagePath)
    {
        u32      capturedImgWidth   = 0;
        u32      capturedImgHeight  = 0;
        u32      goldenImgWidth     = 0;
        u32      goldenImgHeight    = 0;
        uint8_t* captureImagePixels = NULL;
        uint8_t* goldenImagePixels  = NULL;

        if (!ReadPPM(capturedImagePath.c_str(), &capturedImgWidth, &capturedImgHeight, &captureImagePixels))
            return 100.0f;

        if (!ReadPPM(goldenImagePath.c_str(), &goldenImgWidth, &goldenImgHeight, &goldenImagePixels))
            return 100.0f;

        if ((capturedImgWidth != goldenImgWidth) && (capturedImgHeight != goldenImgHeight))
            return 100.0f;

        int total_pixels        = capturedImgWidth * capturedImgHeight;
        int total_squared_error = 0;

        // Calculate the squared error for each pixel
        for (int i = 0; i < total_pixels; i++) {
            for (int j = 0; j < 3; j++) {    // 3 channels: R, G, B
                int index = i * 3 + j;
                int diff  = captureImagePixels[index] - goldenImagePixels[index];
                total_squared_error += diff * diff;
            }
        }

        free(captureImagePixels);
        free(goldenImagePixels);

        float mse = (float) total_squared_error / (total_pixels * 3);

        // If MSE is 0, return infinite PSNR (perfect match)
        if (mse == 0.0f) {
            return 0.0f;    // it should be INF but for brevity we return 0
        }

        // Compute PSNR
        float max_pixel_value = 255.0f;    // Max value for 8-bit color channels (0-255)
        float psnr            = 10.0f * log10((max_pixel_value * max_pixel_value) / mse);

        return psnr;
    }
}    // namespace Razix