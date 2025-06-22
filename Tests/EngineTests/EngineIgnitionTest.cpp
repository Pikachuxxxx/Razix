#pragma once

#include <Razix.h>
#include <gtest/gtest.h>

namespace Razix {

#define TEST_APP_NUM_FRAMES_DEFAULT 120

    /**
     * Base class for Engine Ignition testing applications
     */
    class EngineIngnitionTestBase final : public RZApplication
    {
    public:
        EngineIngnitionTestBase(const std::string& projectRoot, u32 numFrames = TEST_APP_NUM_FRAMES_DEFAULT, const std::string& appName = "RazixEngineTestApp")
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
        }

        ~EngineIngnitionTestBase() {}

        void OnRender() override final
        {
            if (m_CurrentFrame >= m_NumFrames) {
                // Request app to close
                RZApplication::Get().setAppState(AppState::Closing);
            }

            m_CurrentFrame++;
        }

    protected:
        int m_NumFrames;
        int m_CurrentFrame;
    };
}    // namespace Razix

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    return new Razix::EngineIngnitionTestBase(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Tests/")));
}

class EngineIgnitionTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(EngineIgnitionTests, ignite_and_shutdown)
{
#ifdef RAZIX_BUILD_GITHUB_CI
    GTEST_SKIP() << "Skipping RTX test on GitHub Actions (no GPU)";
#endif

    EngineMain(0, NULL);
    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    int result = EngineExit();

#define SUCCESSFUL_ENGINE_EXIT_CODE 0
    EXPECT_EQ(result, SUCCESSFUL_ENGINE_EXIT_CODE);
}
