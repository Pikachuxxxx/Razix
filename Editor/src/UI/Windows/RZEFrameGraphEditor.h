#pragma once

#include <QMenu>
#include <QtNodeGraph.h>

#include "generated/ui_RZEFrameGraphEditor.h"

#include <QSignalMapper>

namespace Razix {
    namespace Editor {

        class FrameGraphGraphicsView : public NodeGraphicsView
        {
            Q_OBJECT

        public:
            FrameGraphGraphicsView();
            ~FrameGraphGraphicsView() {}

            void OnKeyPressEvent(QKeyEvent* e) override
            {
                if (e->key() == Qt::Key_Space) {
                    QMenu menu("Node");
                    auto  addPassNodeAction = menu.addAction("Add Pass Node");
                    connect(addPassNodeAction, SIGNAL(triggered()), this, SLOT(OnAddPassNode()));
                    auto addResourceNodeMenu = menu.addMenu("Resource Node");
                    auto addBuffer           = addResourceNodeMenu->addAction("Add Buffer Resource");
                    connect(addBuffer, SIGNAL(triggered()), this, SLOT(OnAddBufferNode()));
                    auto addTexture = addResourceNodeMenu->addAction("Add Texture Resource");
                    connect(addTexture, SIGNAL(triggered()), this, SLOT(OnAddTextureNode()));
                    auto addImport = menu.addAction("Import Resource");
                    connect(addImport, SIGNAL(triggered()), this, SLOT(OnImportResource()));

                    menu.exec(QCursor::pos());
                }
            }

            void OnRightMousePress(QMouseEvent* event) override
            {
            }

        public slots:
            void OnAddPassNode();
            void OnAddBufferNode();
            void OnAddTextureNode();
            void OnImportResource();
        };

        //-----------------------------------------------------------

        class RZEFrameGraphEditor : public QWidget
        {
            Q_OBJECT

        public:
            RZEFrameGraphEditor(QWidget* parent = nullptr);
            ~RZEFrameGraphEditor() {}

        public slots:
            void OnImportPresetButtonClicked();

            void OnAddInputPinClicked();
            void OnInputPinNameChanged(int idx);
            void OnRemoveInputPinClicked(int idx);

        private:
            Ui::FrameGraphEditor ui;
            NodeGraphWidget*     m_NodeGraphWidget = nullptr;
            QSignalMapper*       m_LineEditsSignalMapper    = nullptr;
            QSignalMapper*       m_ButtonsSignalMapper = nullptr;

        private:
            void
                 populatePresetResourceNodesList();
            void populatePresetImportNodesList();
            void populatePresetPassNodesList();
        };
    }    // namespace Editor
}    // namespace Razix
