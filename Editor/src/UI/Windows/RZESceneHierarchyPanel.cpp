#include "RZESceneHierarchyPanel.h"

#include <QTreeWidget>

#include <glm/glm.hpp>
#include <sol/sol.hpp>

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZRoot.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/Components/RZComponents.h"

namespace Razix {
    namespace Editor {
        RZESceneHierarchyPanel::RZESceneHierarchyPanel(RZEMainWindow* mainWindow, QWidget* parent)
            : QFrame(parent), m_MainWindow(mainWindow)
        {
            ui.setupUi(this);

            header = ui.sceneTree->header();
            header->setWindowTitle("Empty Scene");
            ui.sceneTree->setHeader(header);
            // Enable multi selection
            //ui.sceneTree->setSelectionMode(QAbstractItemView::MultiSelection);

            this->setBaseSize(QSize(80, 100));

            // Connect the signals
            connect(ui.sceneTree, SIGNAL(itemSelectionChanged()), this, SLOT(OnItemSelected()));

            connect(mainWindow, SIGNAL(OnEntityAddedToScene()), this, SLOT(UpdatePanel()));
        }

        RZESceneHierarchyPanel::~RZESceneHierarchyPanel() { }

        void RZESceneHierarchyPanel::populateHierarchy()
        {
            Razix::RZScene* scene    = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
            auto&           registry = scene->getRegistry();

            // Set the scene name as the header name
            header->setWindowTitle(scene->getSceneName().c_str());

            // Recursively draw the nodes based on the Hierarchy component
            registry.each([&](auto& entity) {
                if (registry.valid(entity)) {
                    auto hierarchyComponent = registry.try_get<HierarchyComponent>(entity);
                    if (!hierarchyComponent || hierarchyComponent->Parent == entt::null) {
                        drawEntityNode(RZEntity(entity, scene));
                    }
                }
            });
        }

        void RZESceneHierarchyPanel::drawEntityNode(RZEntity& entity)
        {
            const auto& nameComponent = entity.GetComponent<TagComponent>();

            QTreeWidgetItem* Entity = new QTreeWidgetItem;
            Entity->setText(0, nameComponent.Tag.c_str());
            ui.sceneTree->addTopLevelItem(Entity);
            // Set the entity as metadata, this way we don't have to search the registry when we select one from the list
            QVariant entityVariant = QVariant::fromValue<RZEntity>(entity);
            Entity->setData(0, Qt::UserRole, entityVariant);
        }

        void RZESceneHierarchyPanel::OnItemSelected()
        {
            // Find the entity selected by the name and emit a signal to the Inspector Window
            QList<QTreeWidgetItem*> selectedItems = ui.sceneTree->selectedItems();
            if (!selectedItems.size())
                return;

            // Find the entity from the registry
            Razix::RZScene* scene    = Razix::RZEngine::Get().getSceneManager().getCurrentScene();
            auto&           registry = scene->getRegistry();

            // TODO: Support multiple selection using the selectedItems list
            QVariant entityVariant = selectedItems[0]->data(0, Qt::UserRole);
            auto     entity        = entityVariant.value<RZEntity>();
            RZApplication::Get().setGuzimoForEntity(entity);
            //  Now send this entity to the Inspector via signal
            emit OnEntitySelected(entity);
        }

        void RZESceneHierarchyPanel::OnEntitySelectedByUser(RZEntity entity)
        {
            ui.sceneTree->clearSelection();
            // Select the item in the tree and Update the Inspector panel
            std::string             itemName = entity.GetComponent<TagComponent>().Tag;
            QList<QTreeWidgetItem*> clist    = ui.sceneTree->findItems(QString(itemName.c_str()), Qt::MatchContains | Qt::MatchRecursive, 0);
            // TODO: Add support multi entity editing sometime in future
            ui.sceneTree->setItemSelected(clist[0], true);
            RZApplication::Get().setGuzimoForEntity(entity);

            emit OnEntitySelected(entity);
        }

        void RZESceneHierarchyPanel::UpdatePanel()
        {
            ui.sceneTree->clear();
            populateHierarchy();
            repaint();
        }

    }    // namespace Editor
}    // namespace Razix