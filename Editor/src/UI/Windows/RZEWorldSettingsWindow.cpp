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
            connect(ui.filterRadius, SIGNAL(returnPressed()), this, SLOT(On_SetFilterRadius()));
            connect(ui.strength, SIGNAL(returnPressed()), this, SLOT(On_SetStrength()));

            connect(ui.tonemapMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnToneMapModeChanged()));

            connect(ui.SSAO, SIGNAL(clicked(bool)), this, SLOT(OnSSAOEnabled()));
            connect(ui.ImGui, SIGNAL(clicked(bool)), this, SLOT(OnImGuiEnabled()));
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

        void RZEWorldSettingsWindow::On_SetFilterRadius()
        {
            auto& settings              = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.radius = ui.filterRadius->text().toFloat();
        }

        void RZEWorldSettingsWindow::On_SetStrength()
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

    }    // namespace Editor
}    // namespace Razix