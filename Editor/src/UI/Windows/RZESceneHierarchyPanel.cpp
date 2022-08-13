#include "RZESceneHierarchyPanel.h"

#include <QTreeWidget>

#include "Razix/Scene/Components/RZComponents.h"

namespace Razix {
    namespace Editor {
        RZESceneHierarchyPanel::RZESceneHierarchyPanel(QWidget* parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            header = ui.sceneTree->header();
            header->setWindowTitle("Empty Scene");
            ui.sceneTree->setHeader(header);

            // Adding some child elements for test
            //QTreeWidgetItem* Entity1 = new QTreeWidgetItem;
            //Entity1->setText(0, "Entity_1");
            //ui.sceneTree->addTopLevelItem(Entity1);
            //QTreeWidgetItem* Entity2 = new QTreeWidgetItem;
            //ui.sceneTree->addTopLevelItem(Entity2);
            //Entity2->setText(0, "Entity_2");
            //
            //QTreeWidgetItem* Entity_2_child_1 = new QTreeWidgetItem;
            //Entity_2_child_1->setText(0, "Entity_2_child_1");
            //
            //Entity2->addChild(Entity_2_child_1);

            // Connect the signals
            connect(ui.sceneTree, SIGNAL(itemSelectionChanged()), this, SLOT(OnItemSelected()));
        }

        RZESceneHierarchyPanel::~RZESceneHierarchyPanel()
        {}

        void RZESceneHierarchyPanel::populateHierarchy()
        {
            Razix::RZScene* scene    = RZEngine::Get().getSceneManager().getCurrentScene();
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

            std::cout << nameComponent.Tag << std::endl;

            // Add the children using the Hierarchy component
        }

        void RZESceneHierarchyPanel::OnItemSelected()
        {
            // Find the entity selected by the name and emit a signal to the Inspector Window
            QList<QTreeWidgetItem*> selectedItems = ui.sceneTree->selectedItems();
            std::cout << selectedItems[0]->text(0).toStdString() << std::endl;

            // Find the entity from the registry
            Razix::RZScene* scene    = RZEngine::Get().getSceneManager().getCurrentScene();
            auto&           registry = scene->getRegistry();

            // TODO: Find a better way to find the selected entity from the registry
            //registry.each([&](auto& entity) {
            //    if (registry.valid(entity)) {
            //        TagComponent* tagComponent = registry.try_get<TagComponent>(entity);
            //        if (tagComponent->Tag == selectedItems[0]->text(0).toStdString()) {
            //            std::cout << tagComponent->Tag << std::endl;
            //            RZEntity entity(entity, scene);
            //            std::cout << entity.GetComponent<IDComponent>().UUID << std::endl;
            //            // Now send this entity to the Inspector via signal (can you pass arguments via signals in QT????)
            //            emit OnEntitySelected(entity);
            //            return;
            //        }
            //    }
            //});

            QVariant entityVariant = selectedItems[0]->data(0, Qt::UserRole);
            auto     entity        = entityVariant.value<RZEntity>();
            std::cout << entity.GetComponent<TagComponent>().Tag << std::endl;
            std::cout << entity.GetComponent<IDComponent>().UUID << std::endl;
            //  Now send this entity to the Inspector via signal
            emit OnEntitySelected(entity);
        }

    }    // namespace Editor
}    // namespace Razix