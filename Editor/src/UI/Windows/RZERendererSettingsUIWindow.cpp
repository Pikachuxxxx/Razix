// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZERendererSettingsUIWindow.h"

#include <QCheckBox>

#include "Razix/Core/RZEngine.h"

namespace Razix {
    namespace Editor {
        RZERendererSettingsUIWindow::RZERendererSettingsUIWindow(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.useProcSkyboxChkBox, SIGNAL(clicked(bool)), this, SLOT(On_UseProceduralSkybox()));
            connect(ui.tonemapMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnToneMapModeChanged()));
            connect(ui.aaMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAAModeChanged()));
            connect(ui.sceneSamplingPattern, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSceneSamplingPatternChanged()));

            connect(ui.filterRadius, SIGNAL(returnPressed()), this, SLOT(OnSetFilterRadius()));
            connect(ui.strength, SIGNAL(returnPressed()), this, SLOT(OnSetStrength()));

            connect(ui.debugflag_visCSM, SIGNAL(clicked(bool)), this, SLOT(OnDebugVisCSM()));

            connect(ui.Shadows, SIGNAL(clicked(bool)), this, SLOT(OnShadowsEnabled()));
            connect(ui.Skybox, SIGNAL(clicked(bool)), this, SLOT(OnSkyboxEnabled()));
            connect(ui.SSAO, SIGNAL(clicked(bool)), this, SLOT(OnSSAOEnabled()));
            connect(ui.ImGui, SIGNAL(clicked(bool)), this, SLOT(OnImGuiEnabled()));
            connect(ui.FXAA, SIGNAL(clicked(bool)), this, SLOT(OnFXAAEnabled()));
            connect(ui.enableTonemapping, SIGNAL(clicked(bool)), this, SLOT(OnTonemapEnabled()));

            // Set the default scene sampling pattern to Halton mode
            ui.sceneSamplingPattern->setCurrentIndex((u32) Razix::Graphics::SceneSamplingPattern::Halton);
        }

        //-------------------------------------------------------------------------------------------

        void RZERendererSettingsUIWindow::On_UseProceduralSkybox()
        {
            auto& settings               = Razix::RZEngine::Get().getWorldSettings();
            settings.useProceduralSkybox = ui.useProcSkyboxChkBox->isChecked();
        }

        void RZERendererSettingsUIWindow::OnToneMapModeChanged()
        {
            auto& settings       = Razix::RZEngine::Get().getWorldSettings();
            settings.tonemapMode = (Razix::Graphics::TonemapMode) ui.tonemapMode->currentIndex();
        }

        void RZERendererSettingsUIWindow::OnAAModeChanged()
        {
            auto& settings  = Razix::RZEngine::Get().getWorldSettings();
            settings.aaMode = (Razix::Graphics::Antialising) ui.aaMode->currentIndex();
        }

        void RZERendererSettingsUIWindow::OnSceneSamplingPatternChanged()
        {
            auto& settings           = Razix::RZEngine::Get().getWorldSettings();
            settings.samplingPattern = (Razix::Graphics::SceneSamplingPattern) ui.sceneSamplingPattern->currentIndex();
        }

        void RZERendererSettingsUIWindow::OnSetFilterRadius()
        {
            auto& settings              = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.radius = ui.filterRadius->text().toFloat();
        }

        void RZERendererSettingsUIWindow::OnSetStrength()
        {
            auto& settings                = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.strength = ui.strength->text().toFloat();
        }

        //-------------------------------------------------------------------------------------------

        void RZERendererSettingsUIWindow::OnDebugVisCSM()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.debugflag_visCSM->isChecked())
                settings.debugFlags |= Razix::Graphics::RendererDebugFlag_VisCSMCascades;
            else
                settings.debugFlags &= ~Razix::Graphics::RendererDebugFlag_VisCSMCascades;
        }

        //-------------------------------------------------------------------------------------------

        void RZERendererSettingsUIWindow::OnShadowsEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.Shadows->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_Shadows;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_Shadows;
        }

        void RZERendererSettingsUIWindow::OnSkyboxEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.Skybox->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_Skybox;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_Skybox;
        }

        void RZERendererSettingsUIWindow::OnSSAOEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.SSAO->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_SSAO;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_SSAO;
        }

        void RZERendererSettingsUIWindow::OnImGuiEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.ImGui->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_ImGui;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_ImGui;
        }

        void RZERendererSettingsUIWindow::OnFXAAEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.FXAA->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_FXAA;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_FXAA;
        }

        void RZERendererSettingsUIWindow::OnTonemapEnabled()
        {
            auto& settings = Razix::RZEngine::Get().getWorldSettings();
            if (ui.enableTonemapping->isChecked())
                settings.renderFeatures |= Razix::Graphics::RendererFeature_Tonemap;
            else
                settings.renderFeatures &= ~Razix::Graphics::RendererFeature_Tonemap;
        }

    }    // namespace Editor
}    // namespace Razix