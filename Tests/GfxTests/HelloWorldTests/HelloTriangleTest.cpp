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
    HelloTriangleTestBase(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_INF, const std::string& appName = "RazixGfxTestApp")
        : RZGfxTestAppBase(projectRoot, numFrames, appName) {}

    void OnStart() override
    {
        RZEngine::Get().getWorldRenderer().clearFrameGraph();
        RZEngine::Get().getWorldRenderer().pushRenderPass(&helloTriangleTestPass, nullptr, &RZEngine::Get().getWorldSettings());
        RAZIX_CORE_INFO("Compiling FrameGraph ....");
        RZEngine::Get().getWorldRenderer().getFrameGraph().compile();
    }

    void OnQuit() override
    {
        helloTriangleTestPass.destroy();
    }

private:
    Razix::Gfx::RZHelloTriangleTestPass helloTriangleTestPass;
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    return new HelloTriangleTestBase(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Tests/")));
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

    // TODO: Screenshot test validation

#define SUCCESSFUL_ENGINE_EXIT_CODE 0
    EXPECT_EQ(result, SUCCESSFUL_ENGINE_EXIT_CODE);
}
