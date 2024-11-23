// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEViewport.h"

namespace Razix {
    namespace Editor {
        RZEViewport::RZEViewport(RZESceneHierarchyPanel* sceneHierarchyPanel, QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            m_VulkanViewportWindow = new RZEVulkanWindow;

            ui.centralVLayout->addWidget(QWidget::createWindowContainer(m_VulkanViewportWindow));

            setObjectName("Viewport");

            // Connections
            connect(m_VulkanViewportWindow, &RZEVulkanWindow::OnEntitySelected, sceneHierarchyPanel, &RZESceneHierarchyPanel::OnEntitySelectedByUser);
        }

        RZEViewport::~RZEViewport()
        {
        }
    }    // namespace Editor
}    // namespace Razix
