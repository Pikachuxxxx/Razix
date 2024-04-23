#pragma once

#include "generated/ui_RZERendererSettingsUIWindow.h"
#include <QWidget>

namespace Razix {
    namespace Editor {

        class RZERendererSettingsUIWindow : public QWidget
        {
            Q_OBJECT

        public:
            RZERendererSettingsUIWindow(QWidget *parent = nullptr);
            ~RZERendererSettingsUIWindow() {}

        public slots:
            // Environment Settings
            void On_UseProceduralSkybox();
            // Tonemap Mode
            void OnToneMapModeChanged();
            // AA
            void OnAAModeChanged();
            // Scene Sampling pattern
            void OnSceneSamplingPatternChanged();

            // Bloom Settings
            void OnSetFilterRadius();
            void OnSetStrength();

            // Debug Flags
            void OnDebugVisCSM();

            // Enable Renderer settings
            void OnShadowsEnabled();
            void OnSkyboxEnabled();
            void OnSSAOEnabled();
            void OnImGuiEnabled();
            void OnFXAAEnabled();
            void OnTonemapEnabled();

        private:
            Ui::WorldSettings ui;
        };
    }    // namespace Editor
}    // namespace Razix