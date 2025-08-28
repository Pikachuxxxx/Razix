#include <Razix.h>
#include <gtest/gtest.h>

#include <TestCommon/GfxTestsBase.h>

#include "Passes/RZMandleBrotPass.h"
#include "Passes/RZBlitToSwapchainPass.h"

using namespace Razix;

class ComputeTestApp final : public RZGfxTestAppBase
{
public:
    ComputeTestApp(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_INF, const std::string& appName = "RazixGfxTestApp")
        : RZGfxTestAppBase(projectRoot, numFrames, appName) {}

    void OnStart() override
    {
        RZEngine::Get().getWorldRenderer().clearFrameGraph();
        // Push compute then blit
        RZEngine::Get().getWorldRenderer().pushRenderPass(&m_MandleBrotPass, nullptr, &RZEngine::Get().getWorldSettings());
        //RZEngine::Get().getWorldRenderer().pushRenderPass(&m_BlitPass, nullptr, &RZEngine::Get().getWorldSettings());
        RAZIX_CORE_INFO("Compiling FrameGraph ....");
        RZEngine::Get().getWorldRenderer().getFrameGraph().compile();

        std::string testsRootPath;
        RZVirtualFileSystem::Get().resolvePhysicalPath("//TestsRoot/GfxTests/", testsRootPath, true);

        SetGoldenImagePath(testsRootPath + "GoldenImages/GoldenImageComputeTest_1280_720.ppm");
        SetScreenshotPath(testsRootPath + "TestImages/ComputeTest_1280_720.ppm");

        RZGfxTestAppBase::OnStart();
    }

    void OnQuit() override
    {
        RZEngine::Get().getWorldRenderer().flushGPUWork();
        m_MandleBrotPass.destroy();
        m_BlitPass.destroy();
        RZGfxTestAppBase::OnQuit();
    }

private:
    Razix::Gfx::RZMandleBrotPass     m_MandleBrotPass;
    Razix::Gfx::RZBlitToSwapchainPass m_BlitPass;
};

static RZGfxTestAppBase* s_GfxTestBaseApp = NULL;
Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    s_GfxTestBaseApp = new ComputeTestApp(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Tests/")));
    return s_GfxTestBaseApp;
}

class ComputeTests : public ::testing::Test { };

TEST_F(ComputeTests, ComputeMandleBrot)
{
    int result = EngineTestLoop();

    float psnr = s_GfxTestBaseApp->CompareWithGoldenImage();
    if (psnr > 90.f)
        result = -1;
    else
        result = SUCCESSFUL_ENGINE_EXIT_CODE;

    EXPECT_EQ(result, SUCCESSFUL_ENGINE_EXIT_CODE);
}
