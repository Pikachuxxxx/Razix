#pragma once

#include "generated/ui_RZEWorldSettingsWindow.h"
#include <QWidget>

namespace Razix {
    namespace Editor {

        class RZEWorldSettingsWindow : public QWidget
        {
            Q_OBJECT

        public:
            RZEWorldSettingsWindow(QWidget *parent = nullptr);
            ~RZEWorldSettingsWindow();

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

            // Enable Renderer settings
            void OnShadowsEnabled();
            void OnSkyboxEnabled();
            void OnSSAOEnabled();
            void OnImGuiEnabled();

        private:
            Ui::WorldSettings ui;
        };
    }    // namespace Editor
}    // namespace Razix