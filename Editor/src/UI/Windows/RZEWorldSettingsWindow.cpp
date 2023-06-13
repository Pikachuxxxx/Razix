// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEWorldSettingsWindow.h"

#include "Razix/Core/RZEngine.h"

namespace Razix {
    namespace Editor {
        RZEWorldSettingsWindow::RZEWorldSettingsWindow(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            connect(ui.filterRadius, SIGNAL(returnPressed()), this, SLOT(On_SetFilterRadius()));
            connect(ui.strength, SIGNAL(returnPressed()), this, SLOT(On_SetStrength()));
        }

        RZEWorldSettingsWindow::~RZEWorldSettingsWindow()
        {
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
    }    // namespace Editor
}    // namespace Razix