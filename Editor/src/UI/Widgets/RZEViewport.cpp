#include "RZEViewport.h"

namespace Razix {
    namespace Editor {
        RZEViewport::RZEViewport(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);
        }

        RZEViewport::~RZEViewport()
        {}
    }    // namespace Editor
}    // namespace Razix
