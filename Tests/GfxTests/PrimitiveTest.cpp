#include <Razix.h>
#include <gtest/gtest.h>

#include <TestCommon/GfxTestsBase.h>

#include "Passes/RZPrimitiveTestPass.h"

using namespace Razix;

class PrimitiveTestApp final : public RZGfxTestAppBase
{
public:
    PrimitiveTestApp(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const std::string& appName = "RazixGfxTestApp")
        : RZGfxTestAppBase(projectRoot, numFrames, appName) {}

    void OnStart() override
    {
        RZEngine::Get().getWorldRenderer().clearFrameGraph();
        RZEngine::Get().getWorldRenderer().pushRenderPass(&m_PrimitivePass, nullptr, &RZEngine::Get().getWorldSettings());
        RAZIX_CORE_INFO("Compiling FrameGraph ....");
        RZEngine::Get().getWorldRenderer().getFrameGraph().compile();

        std::string testsRootPath;
        RZVirtualFileSystem::Get().resolvePhysicalPath("//TestsRoot/GfxTests/", testsRootPath, true);

        SetGoldenImagePath(testsRootPath + "GoldenImages/GoldenImagePrimitiveTest_1280_720.ppm");
        SetScreenshotPath(testsRootPath + "TestImages/PrimitiveTest_1280_720.ppm");

        RZGfxTestAppBase::OnStart();
    }

    void OnQuit() override
    {
        RZEngine::Get().getWorldRenderer().flushGPUWork();
        m_PrimitivePass.destroy();
        RZGfxTestAppBase::OnQuit();
    }

private:
    Razix::Gfx::RZPrimitiveTestPass m_PrimitivePass;
};

static RZGfxTestAppBase* s_GfxTestBaseApp = NULL;
Razix::RZApplication*    Razix::CreateApplication(int argc, char** argv)
{
    s_GfxTestBaseApp = new PrimitiveTestApp(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Tests/")));
    return s_GfxTestBaseApp;
}

class PrimitiveTests : public ::testing::Test
{};

TEST_F(PrimitiveTests, DrawPrimitive)
{
    int result = EngineTestLoop();

    float psnr = s_GfxTestBaseApp->CompareWithGoldenImage();
    printf("PSNR: %6.2f dB\n", psnr);
    if (psnr == UINT32_MAX || psnr >= 40.f)
        result = SUCCESSFUL_ENGINE_EXIT_CODE;
    else
        result = -1;

    EXPECT_EQ(result, SUCCESSFUL_ENGINE_EXIT_CODE);
}
