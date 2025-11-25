#include <Razix.h>
#include <gtest/gtest.h>

#include <TestCommon/GfxTestsBase.h>

#include "Passes/RZWaveIntrinsicsTestPass.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

using namespace Razix;

class WaveIntrinsicsTest final : public RZGfxTestAppBase
{
public:
    WaveIntrinsicsTest(const RZString& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const RZString& appName = "RazixGfxTestApp")
        : RZGfxTestAppBase(projectRoot, numFrames, appName)
    {
    }

    void OnStart() override
    {
        RZEngine::Get().getWorldRenderer().clearFrameGraph();
        RZEngine::Get().getWorldRenderer().pushRenderPass(&WaveIntrinsicsTestPass, nullptr, &RZEngine::Get().getWorldSettings());
        RAZIX_CORE_INFO("Compiling FrameGraph ....");
        RZEngine::Get().getWorldRenderer().getFrameGraph().compile();

        RZString testsRootPath;
        RZVirtualFileSystem::Get().resolvePhysicalPath("//TestsRoot/GfxTests/", testsRootPath, true);

        SetGoldenImagePath(testsRootPath + "GoldenImages/GoldenImageWaveIntrinsics_1280_720.ppm");
        SetScreenshotPath(testsRootPath + "TestImages/WaveIntrinsics_1280_720.ppm");

        RZGfxTestAppBase::OnStart();
    }

    void OnQuit() override
    {
        RZEngine::Get().getWorldRenderer().flushGPUWork();

        WaveIntrinsicsTestPass.destroy();
        RZGfxTestAppBase::OnQuit();
    }

private:
    Razix::Gfx::RZWaveIntrinsicsTestPass WaveIntrinsicsTestPass;
};

static RZGfxTestAppBase* s_GfxTestBaseApp = NULL;
Razix::RZApplication*    Razix::CreateApplication(int argc, char** argv)
{
    s_GfxTestBaseApp = new WaveIntrinsicsTest(RZString(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + RZString("/Tests/")));
    return s_GfxTestBaseApp;
}

class WaveIntrinsicsTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(WaveIntrinsicsTests, WaveIntrinsics)
{
    int result = EngineTestLoop();

    float psnr = s_GfxTestBaseApp->CompareWithGoldenImage();
    printf("PSNR: %6.2f dB\n", psnr);
    result = SUCCESSFUL_ENGINE_EXIT_CODE;

    EXPECT_EQ(result, SUCCESSFUL_ENGINE_EXIT_CODE);
}
