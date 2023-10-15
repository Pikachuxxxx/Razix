#pragma once

#include <QMenu>
#include <QSignalMapper>
#include <QtNodeGraph.h>

#include "generated/ui_RZEFrameGraphEditor.h"

#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

namespace Razix {
    namespace Editor {

        class RZEPassNodeUI;
        class RZEBufferResourceNodeUI;
        class RZETextureResourceNodeUI;
        class RZEImportNodeUI;

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

            void OnLeftMousePress(QMouseEvent* event) override
            {
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
            //-----------------------------------------------------------
            void OnNodeSelected(Node* node);
            void OnPassNodeNameChanged();
            //-----------------------------------------------------------
            // Pass Node Panel
            void OnAddInputPinClicked();
            void OnInputPinNameChanged(int idx);
            void OnRemoveInputPinClicked();
            //-----------------------------------------------------------
            void OnAddOutputPinClicked();
            void OnOutputPinNameChanged(int idx);
            void OnRemoveOutputPinClicked();
            //-----------------------------------------------------------
            void OnAddColorFormatClicked();
            void OnColorFormatChanged(int idx);
            void OnRemoveColorFormatClicked();
            //-----------------------------------------------------------
            void OnBrowseShaderPressed();
            //-----------------------------------------------------------
            void OnPipelineNameChanged();
            void OnPipelineCullModeSelected();
            void OnPipelinePolygonModeSelected();
            void OnPipelineDrawTypeSelected();
            void OnPipelineEnableTransparencyChecked();
            void OnPipelineEnableDepthTestChecked();
            void OnPipelineEnableDepthWriteChecked();
            void OnPipelineDepthOperationSelected();
            void OnPipelineColorSrcSelected();
            void OnPipelineColorDstSelected();
            void OnPipelineColorOperationSelected();
            void OnPipelineAlphaSrcSelected();
            void OnPipelineAlphaDstSelected();
            void OnPipelineAlphaOperationSelected();
            void OnPipelineDepthFormatSelected();
            //-----------------------------------------------------------
            void OnSceneGeometryModeSelected();
            void OnEnableResize();
            void OnResolutionSelected();
            void OnExtentXChanged();
            void OnExtentYChanged();
            void OnLayersChanged();
            //-----------------------------------------------------------
            // Buffer Resource Panel
            void OnBufferNameChanged();
            void OnBufferSizeChanged();
            void OnBufferUsageSelected();

        private:
            Ui::FrameGraphEditor               ui;
            NodeGraphWidget*                   m_NodeGraphWidget                  = nullptr;
            QSignalMapper*                     m_IpLineEditsSignalMapper          = nullptr;
            QSignalMapper*                     m_OpLineEditsSignalMapper          = nullptr;
            QSignalMapper*                     m_ColorFormatsComboBoxSignalMapper = nullptr;
            RZEPassNodeUI*                     m_CurrentEditingPassNode           = nullptr;
            RZEBufferResourceNodeUI*           m_CurrentEditingBufferNode         = nullptr;
            RZETextureResourceNodeUI*          m_CurrentEditingTextureNode        = nullptr;
            QStringList                        m_FormatsStringList                = {};
            Graphics::FrameGraph::RZFrameGraph m_FrameGraph                       = {}; /* FrameGraph classes used to export into a JSON file */

        private:
            void initializePassNodePropertiesInspector();
            void initializeResourceNodePropertiesInspector();
            //-----------------------------------------------------------
            void populatePopertiesPanelWithPassNode();
            void populatePopertiesPanelWithBufferNode();
            void populatePopertiesPanelWithTextureNode();
            void populatePopertiesPanelWithImportNode();
            //-----------------------------------------------------------
            void initializePresetResourceNodesList();
            void initializePresetImportNodesList();
            void initializePresetPassNodesList();
        };
    }    // namespace Editor
}    // namespace Razix
