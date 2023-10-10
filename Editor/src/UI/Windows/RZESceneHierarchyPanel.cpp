\// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZESceneHierarchyPanel.h"

#include <QTreeWidget>

#include <glm/glm.hpp>
#include <sol/sol.hpp>

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZRoot.h"
#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZEntity.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Editor {
        RZESceneHierarchyPanel::RZESceneHierarchyPanel(RZEMainWindow* mainWindow, QWidget* parent)
            : QFrame(parent), m_MainWindow(mainWindow)
        {
            ui.setupUi(this);

            setObjectName(this->windowTitle());

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

        RZESceneHierarchyPanel::~RZESceneHierarchyPanel()
        {
        }

        void RZESceneHierarchyPanel::populateHierarchy()
        {
            Razix::RZScene* scene    = RZSceneManager::Get().getCurrentScene();
            auto&           registry = scene->getRegistry();

            // Set the scene name as the header name
            header->setWindowTitle(scene->getSceneName().c_str());

            // Recursively draw the nodes based on the Hierarchy component
            registry.each([&](auto& entity) {
                if (registry.valid(entity)) {
                    auto hierarchyComponent = registry.try_get<HierarchyComponent>(entity);
                    if (!hierarchyComponent || hierarchyComponent->Parent == entt::null) {
                        QTreeWidgetItem* rootItem = new QTreeWidgetItem;
                        //const auto&      nameComponent = RZEntity(entity, scene).GetComponent<TagComponent>();
                        auto name = registry.get<TagComponent>(entity).Tag;
                        rootItem->setText(0, name.c_str());
                        // Set the entity as metadata, this way we don't have to search the registry when we select one from the list
                        QVariant entityVariant = QVariant::fromValue<RZEntity>(RZEntity(entity, scene));
                        rootItem->setData(0, Qt::UserRole, entityVariant);

                        ui.sceneTree->addTopLevelItem(rootItem);

                        drawEntityNode(RZEntity(entity, scene), scene, registry, rootItem);
                    }
                }
            });
        }

        void RZESceneHierarchyPanel::drawEntityNode(RZEntity parentEntity, RZScene* scene, entt::registry& registry, QTreeWidgetItem* parentItem)
        {
            auto hierarchyComponent = registry.try_get<HierarchyComponent>(parentEntity);
            bool noChildren         = true;

            if (hierarchyComponent != nullptr && hierarchyComponent->First != entt::null)
                noChildren = false;

            if (!noChildren) {
                entt::entity child = hierarchyComponent->First;
                while (child != entt::null && registry.valid(child)) {
                    auto childHerarchyComponent = registry.try_get<HierarchyComponent>(child);
                    // Add this child to the Tree
                    QTreeWidgetItem* childTreeItem = new QTreeWidgetItem;
                    parentItem->addChild(childTreeItem);

                    // Set the entity as metadata, this way we don't have to search the registry when we select one from the list
                    QVariant entityVariant = QVariant::fromValue<RZEntity>(RZEntity(child, scene));
                    childTreeItem->setData(0, Qt::UserRole, entityVariant);
                    const auto& childname = registry.get<TagComponent>(child).Tag;
                    childTreeItem->setText(0, childname.c_str());

                    drawEntityNode(RZEntity(child, scene), scene, registry, childTreeItem);

                    if (registry.valid(child)) {
                        auto hierarchyComponent = registry.try_get<HierarchyComponent>(child);
                        child                   = hierarchyComponent ? hierarchyComponent->Next : entt::null;
                    }
                }
            }
        }

        void RZESceneHierarchyPanel::DestroyEntity(entt::entity entity, entt::registry& registry)
        {
            auto hierarchyComponent = registry.try_get<HierarchyComponent>(entity);
            if (hierarchyComponent) {
                entt::entity child = hierarchyComponent->First;
                while (child != entt::null) {
                    auto hierarchyComponent = registry.try_get<HierarchyComponent>(child);
                    auto next               = hierarchyComponent ? hierarchyComponent->Next : entt::null;
                    DestroyEntity(child, registry);
                    child = next;
                }
            }
            registry.destroy(entity);
        }

        void RZESceneHierarchyPanel::OnItemSelected()
        {
            // Find the entity selected by the name and emit a signal to the Inspector Window
            QList<QTreeWidgetItem*> selectedItems = ui.sceneTree->selectedItems();
            if (!selectedItems.size())
                return;

            // Find the entity from the registry
            Razix::RZScene* scene    = RZSceneManager::Get().getCurrentScene();
            auto&           registry = scene->getRegistry();

            // TODO: Support multiple selection using the selectedItems list
            QVariant entityVariant = selectedItems[0]->data(0, Qt::UserRole);
            auto     entity        = entityVariant.value<RZEntity>();
            RZApplication::Get().setGuizmoForEntity(entity);
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
            RZApplication::Get().setGuizmoForEntity(entity);

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