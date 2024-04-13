// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEWorldSettingsWindow.h"

#include <QCheckBox>

#include "Razix/Core/RZEngine.h"

namespace Razix {
    namespace Editor {
        RZEWorldSettingsWindow::RZEWorldSettingsWindow(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.useProcSkyboxChkBox, SIGNAL(clicked(bool)), this, SLOT(On_UseProceduralSkybox()));
            connect(ui.tonemapMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnToneMapModeChanged()));
            connect(ui.aaMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAAModeChanged()));
            connect(ui.sceneSamplingPattern, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSceneSamplingPatternChanged()));

            connect(ui.filterRadius, SIGNAL(returnPressed()), this, SLOT(OnSetFilterRadius()));
            connect(ui.strength, SIGNAL(returnPressed()), this, SLOT(OnSetStrength()));

            connect(ui.Shadows, SIGNAL(clicked(bool)), this, SLOT(OnShadowsEnabled()));
            connect(ui.Skybox, SIGNAL(clicked(bool)), this, SLOT(OnSkyboxEnabled()));
            connect(ui.SSAO, SIGNAL(clicked(bool)), this, SLOT(OnSSAOEnabled()));
            connect(ui.ImGui, SIGNAL(clicked(bool)), this, SLOT(OnImGuiEnabled()));

            // Set the default scene sampling pattern to Halton mode
            ui.sceneSamplingPattern->setCurrentIndex((u32) Razix::Graphics::SceneSamplingPattern::Halton);
        }

        RZEWorldSettingsWindow::~RZEWorldSettingsWindow()
        {
        }

        void RZEWorldSettingsWindow::On_UseProceduralSkybox()
        {
            auto& settings               = Razix::RZEngine::Get().getWorldSettings();
            settings.useProceduralSkybox = ui.useProcSkyboxChkBox->isChecked();
        }

        void RZEWorldSettingsWindow::OnToneMapModeChanged()
        {
            auto& settings       = Razix::RZEngine::Get().getWorldSettings();
            settings.tonemapMode = (Razix::Graphics::TonemapMode) ui.tonemapMode->currentIndex();
        }

        void RZEWorldSettingsWindow::OnAAModeChanged()
        {
            auto& settings  = Razix::RZEngine::Get().getWorldSettings();
            settings.aaMode = (Razix::Graphics::Antialising) ui.aaMode->currentIndex();
        }

        void RZEWorldSettingsWindow::OnSceneSamplingPatternChanged()
        {
            auto& settings           = Razix::RZEngine::Get().getWorldSettings();
            settings.samplingPattern = (Razix::Graphics::SceneSamplingPattern) ui.sceneSamplingPattern->currentIndex();
        }

        void RZEWorldSettingsWindow::OnSetFilterRadius()
        {
            auto& settings              = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.radius = ui.filterRadius->text().toFloat();
        }

        void RZEWorldSettingsWindow::OnSetStrength()
        {
            auto& settings                = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.strength = ui.strength->text().toFloat();
        }

        void RZEWorldSettingsWindow::OnSSAOEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.SSAO->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_SSAO;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_SSAO;
        }

        void RZEWorldSettingsWindow::OnImGuiEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.ImGui->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_ImGui;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_ImGui;
        }

        void RZEWorldSettingsWindow::OnShadowsEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.Shadows->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_Shadows;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_Shadows;
        }

        void RZEWorldSettingsWindow::OnSkyboxEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.Skybox->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_Skybox;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_Skybox;
        }
    }    // namespace Editor
}    // namespace Razix