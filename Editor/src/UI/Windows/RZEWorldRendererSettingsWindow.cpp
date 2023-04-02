// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEWorldRendererSettingsWindow.h"

#include "Razix/Core/RZEngine.h"

namespace Razix {
    namespace Editor {
        RZEWorldRendererSettingsWindow::RZEWorldRendererSettingsWindow(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.filterRadius, SIGNAL(returnPressed()), this, SLOT(On_SetFilterRadius()));
            connect(ui.strength, SIGNAL(returnPressed()), this, SLOT(On_SetStrength()));
        }

        RZEWorldRendererSettingsWindow::~RZEWorldRendererSettingsWindow()
        {
        }

        void RZEWorldRendererSettingsWindow::On_SetFilterRadius()
        {
            auto& settings              = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.radius = ui.filterRadius->text().toFloat();
        }
         
        void RZEWorldRendererSettingsWindow::On_SetStrength()
        {
            auto& settings                = Razix::RZEngine::Get().getWorldSettings();
            settings.bloomConfig.strength = ui.strength->text().toFloat();
        }
    }    // namespace Editor
}    // namespace Razix