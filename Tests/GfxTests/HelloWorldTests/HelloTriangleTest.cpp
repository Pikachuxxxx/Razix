#include <Razix.h>
#include <gtest/gtest.h>

#include <TestCommon/GfxTestsBase.h>

//#include "Razix/Gfx/Passes/Tests/RZGeomShadersCubeTestPass.h"
//#include "Razix/Gfx/Passes/Tests/RZHelloTextureTestPass.h"
//#include "Razix/Gfx/Passes/Tests/RZWaveInstrinsicsTestPass.h"
#include "Passes/RZHelloTriangleTestPass.h"

using namespace Razix;

class HelloTriangleTestBase final : public RZGfxTestAppBase
{
public:
    HelloTriangleTestBase(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const std::string& appName = "RazixGfxTestApp")
        : RZGfxTestAppBase(projectRoot, numFrames, appName)
    {
    }

    void OnStart() override
    {
        RZEngine::Get().getWorldRenderer().clearFrameGraph();
        RZEngine::Get().getWorldRenderer().pushRenderPass(&helloTriangleTestPass, nullptr, &RZEngine::Get().getWorldSettings());
        RAZIX_CORE_INFO("Compiling FrameGraph ....");
        RZEngine::Get().getWorldRenderer().getFrameGraph().compile();

        std::string testsRootPath;
        RZVirtualFileSystem::Get().resolvePhysicalPath("//TestsRoot/GfxTests/", testsRootPath, true);

        SetGoldenImagePath(testsRootPath + "GoldenImages/GoldenImageHelloTriangle_1280_720.ppm");
        SetScreenshotPath(testsRootPath + "TestImages/HelloTriangle_1280_720.ppm");

        RZGfxTestAppBase::OnStart();
    }

    void OnQuit() override
    {
        Gfx::RZGraphicsContext::GetContext()->Wait();

        helloTriangleTestPass.destroy();
        RZGfxTestAppBase::OnQuit();
    }

private:
    Razix::Gfx::RZHelloTriangleTestPass helloTriangleTestPass;
};

static HelloTriangleTestBase* s_GfxTestBaseApp = NULL;

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    s_GfxTestBaseApp = new HelloTriangleTestBase(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Tests/")));
    return s_GfxTestBaseApp;
}

class HelloTriangleTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HelloTriangleTests, HelloTriangle)
{
    int result = EngineTestLoop();

    float psnr = s_GfxTestBaseApp->CompareWithGoldenImage();
    if (psnr > 10.f)
        result = -1;
    else
        result = SUCCESSFUL_ENGINE_EXIT_CODE;

    EXPECT_EQ(result, SUCCESSFUL_ENGINE_EXIT_CODE);
}
