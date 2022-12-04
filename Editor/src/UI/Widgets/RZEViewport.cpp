#include "RZEViewport.h"

namespace Razix {
    namespace Editor {
        RZEViewport::RZEViewport(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            m_VulkanViewportWindow = new RZEVulkanWindow;

            ui.centralVLayout->addWidget(QWidget::createWindowContainer(m_VulkanViewportWindow));
        }

        RZEViewport::~RZEViewport()
        {}
    }    // namespace Editor
}    // namespace Razix
