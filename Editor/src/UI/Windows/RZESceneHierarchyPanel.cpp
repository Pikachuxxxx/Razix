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

            std::cout << nameComponent.Tag << std::endl;

            // Add the children using the Hierarchy component
        }

    }    // namespace Editor
}    // namespace Razix