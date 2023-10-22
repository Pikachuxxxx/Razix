// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEFrameGraphEditor.h"

#include <fstream>

#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QListWidget>

#include "Nodes/RZEBufferResourceNodeUI.h"
#include "Nodes/RZEImportNodeUI.h"
#include "Nodes/RZEPassNodeUI.h"
#include "Nodes/RZETextureResourceNodeUI.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZAPIDesc.h"

#include "Razix/Graphics/RHI/API/Data/RZBufferData.h"
#include "Razix/Graphics/RHI/API/Data/RZPipelineData.h"
#include "Razix/Graphics/RHI/API/Data/RZTextureData.h"

#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <nlohmann/json.hpp>
using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

static QGraphicsDropShadowEffect* DropShadowEffect = new QGraphicsDropShadowEffect();
// effect->setBlurRadius(10);
// effect->setOffset(2, 4);
// effect->setColor(Qt::black);

static const char* ImportNodeStyleSheet = "color: rgb(0, 0, 0);\nbackground-color: qlineargradient(spread:pad, x1:0 y1:1, x2:0 y2:0, stop:0 rgba(88, 102, 135, 255), stop:1 rgba(181,205,250, 255));\nborder-style:dashed;\nborder-radius:4px;\nborder-width:2px;";

static const std::vector<std::string> ImportNodesPresets = {
    "NoiseTexture",
    "BrdfLUT",
    "EnvironmentMap",
    "PreFilteredMap",
    "IrradianceMap"};

static const std::unordered_map<std::string, std::string> ResourceNodesPresets = {
    {"Buffer", "color:#000000;\n background-color: #F6D975;\nborder-color: rgb(0, 0, 0);\nborder-width:1px;\nborder-style:dashed;\nborder-radius:4px;"},
    {"Texture", "color:#000000;\n background-color: #9AD0FC;\nborder-color: rgb(0, 0, 0);\nborder-width:1px;\nborder-style:dashed;\nborder-radius:4px;"},
    {"FrameData", "color:#000000;\n background-color: #9370DB;\nborder-color: rgb(0, 0, 0);\nborder-width:1px;\nborder-style:dashed;\nborder-radius:4px;"},
    {"SceneLightsData", "color:#000000;\n background-color: #9370DB;\nborder-color: rgb(0, 0, 0);\nborder-width:1px;\nborder-style:dashed;\nborder-radius:4px;"},
};

namespace Razix {
    namespace Editor {

        //-----------------------------------------------------------
        // FrameGraphGraphicsView
        //-----------------------------------------------------------

        FrameGraphGraphicsView::FrameGraphGraphicsView()
            : NodeGraphicsView()
        {
        }

        void FrameGraphGraphicsView::OnAddPassNode()
        {
            RZEPassNodeUI* passNodeUI = new RZEPassNodeUI("Test Pass Node", getScene());
        }

        void FrameGraphGraphicsView::OnAddBufferNode()
        {
            RZEBufferResourceNodeUI* bufferResNodeUI = new RZEBufferResourceNodeUI("Buffer", getScene());
        }

        void FrameGraphGraphicsView::OnAddTextureNode()
        {
            RZETextureResourceNodeUI* textureResNodeUI = new RZETextureResourceNodeUI("Texture", getScene());
        }

        void FrameGraphGraphicsView::OnImportResource()
        {
            RZEImportNodeUI* importNodeUI = new RZEImportNodeUI("Imported", getScene());
        }

        //-----------------------------------------------------------
        // RZEFrameGraphEditor
        //-----------------------------------------------------------n

        RZEFrameGraphEditor::RZEFrameGraphEditor(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            auto frameGraphGraphicsView = new FrameGraphGraphicsView();
            m_NodeGraphWidget           = new NodeGraphWidget(frameGraphGraphicsView);

            ui.body_layout->addWidget(m_NodeGraphWidget);

            DropShadowEffect->setBlurRadius(5);
            DropShadowEffect->setOffset(2, 4);
            DropShadowEffect->setColor(Qt::black);

            connect(frameGraphGraphicsView, SIGNAL(OnNodeSelected(Node*)), this, SLOT(OnNodeSelected(Node*)));

            // prepare Data for combo boxes in the props inspector and other such stuff
            initializePassNodePropertiesInspector();
            initializeResourceNodePropertiesInspector();
            initializeImportNodePropertiesInspector();

            // Load presets panel
            initializePresetImportNodesList();
            initializePresetResourceNodesList();

            // Setup Toolbar
            setupToolbar();
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::OnImportPresetButtonClicked()
        {
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::OnNodeSelected(Node* node)
        {
            if (node) {
                if (dynamic_cast<RZEPassNodeUI*>(node)) {
                    ui.stackedWidget->setCurrentIndex(1);
                    m_CurrentEditingPassNode = dynamic_cast<RZEPassNodeUI*>(node);
                    // Populate the panel
                    populatePopertiesPanelWithPassNode();

                } else if (dynamic_cast<RZEBufferResourceNodeUI*>(node)) {
                    ui.stackedWidget->setCurrentIndex(2);
                    m_CurrentEditingBufferNode = dynamic_cast<RZEBufferResourceNodeUI*>(node);
                    // Populate the panel
                    populatePopertiesPanelWithBufferNode();
                } else if (dynamic_cast<RZETextureResourceNodeUI*>(node)) {
                    ui.stackedWidget->setCurrentIndex(3);
                    m_CurrentEditingTextureNode = dynamic_cast<RZETextureResourceNodeUI*>(node);
                    // Populate the panel
                    populatePopertiesPanelWithTextureNode();
                } else if (dynamic_cast<RZEImportNodeUI*>(node)) {
                    ui.stackedWidget->setCurrentIndex(4);
                    m_CurrentEditingImportNode = dynamic_cast<RZEImportNodeUI*>(node);
                    // Populate the panel
                    populatePopertiesPanelWithImportNode();
                }
                node->update();
            } else {
                ui.stackedWidget->setCurrentIndex(0);
                m_CurrentEditingPassNode    = nullptr;
                m_CurrentEditingBufferNode  = nullptr;
                m_CurrentEditingTextureNode = nullptr;
                m_CurrentEditingImportNode  = nullptr;
            }

            m_NodeGraphWidget->repaint();
        }

        void RZEFrameGraphEditor::OnPassNodeNameChanged()
        {
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->setTitle(ui.PassName->text().toStdString());
            }
        }
        //-----------------------------------------------------------
        void RZEFrameGraphEditor::OnAddInputPinClicked()
        {
            // TODO: Add QT user data to identify the pin idx

            // Create HBoxLayout with a text edit and cross button and connect them to a signal
            //QHBoxLayout* hLayout = new QHBoxLayout;

            auto pinNameEdit = new QLineEdit();
            pinNameEdit->setPlaceholderText("Enter input socket name");

            //m_ButtonsSignalMapper   = new QSignalMapper(this);

            m_IpLineEditsSignalMapper->setMapping(pinNameEdit, ui.ip_pins_layout->rowCount());

            connect(pinNameEdit, SIGNAL(returnPressed()), m_IpLineEditsSignalMapper, SLOT(map()), Qt::UniqueConnection);
            connect(m_IpLineEditsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnInputPinNameChanged(int)), Qt::UniqueConnection);

#if 0
            hLayout->addWidget(pinNameEdit);

            QPushButton* deleteButton = new QPushButton;
            deleteButton->setIcon(QIcon(":/rzeditor/cross_red.png"));
            m_ButtonsSignalMapper->setMapping(deleteButton, ui.ip_pins_layout->rowCount());

            connect(deleteButton, SIGNAL(pressed()), m_ButtonsSignalMapper, SLOT(map()));
            connect(m_ButtonsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnRemoveInputPinClicked(int)));

            hLayout->addWidget(deleteButton);
#endif

            auto label = "input pin #" + std::to_string(ui.ip_pins_layout->rowCount());
            ui.ip_pins_layout->addRow(label.c_str(), pinNameEdit);

            // Add input node
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->addInputSocket(pinNameEdit->text().toStdString());
                m_CurrentEditingPassNode->update();
            }

            m_NodeGraphWidget->repaint();
        }

        void RZEFrameGraphEditor::OnInputPinNameChanged(int idx)
        {
            //auto    LabelLayoutWidget = ui.pins_layout->itemAt(idx, QFormLayout::ItemRole::LabelRole)->widget();
            //auto    label             = qobject_cast<QLabel*>(LabelLayoutWidget);
            //QString labelText         = label->text();
            //std::cout << "Text from Label: " << labelText.toStdString() << std::endl;

            auto FieldLayoutItem = ui.ip_pins_layout->itemAt(idx, QFormLayout::ItemRole::FieldRole);
            // NOTE: Since we are adding a layout we need to cast it as layout, be careful on how we are adding into the row!
            //QHBoxLayout* hLayout  = qobject_cast<QHBoxLayout*>(FieldLayoutItem->layout());

            QWidget*   widget   = qobject_cast<QWidget*>(FieldLayoutItem->widget());
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);

            std::cout << lineEdit->text().toStdString() << std::endl;

            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->getInputSocket(idx - 1)->setSocketName(lineEdit->text().toStdString());
                m_CurrentEditingPassNode->update();
            }

            m_NodeGraphWidget->repaint();
        }

        void RZEFrameGraphEditor::OnRemoveInputPinClicked()
        {
            // Always remove the last pin
            if (ui.ip_pins_layout->rowCount() > 1) {
                ui.ip_pins_layout->removeRow(ui.ip_pins_layout->rowCount() - 1);
                ui.ip_pins_layout->update();
            }
#if 0
            for (i32 i = 1; i < ui.ip_pins_layout->rowCount(); i++) {
                // First rename the label then update it's signal mapper Idx
                auto    LabelLayoutWidget = ui.ip_pins_layout->itemAt(i, QFormLayout::ItemRole::LabelRole)->widget();
                auto    label             = qobject_cast<QLabel*>(LabelLayoutWidget);
                QString pinlabelText      = label->text();

                auto FieldLayoutItem = ui.ip_pins_layout->itemAt(i, QFormLayout::ItemRole::FieldRole);

                // NOTE: Since we are adding a layout we need to cast it as layout, be careful on how we are adding into the row!
                QHBoxLayout* hLayout  = qobject_cast<QHBoxLayout*>(FieldLayoutItem->layout());
                QLineEdit*   lineEdit = qobject_cast<QLineEdit*>(hLayout->itemAt(0)->widget());
                QPushButton* button   = qobject_cast<QPushButton*>(hLayout->itemAt(1)->widget());

                auto labelText = "input pin #" + std::to_string(i);
                label->setText(labelText.c_str());

                m_LineEditsSignalMapper->removeMappings(lineEdit);
                //m_LineEditsSignalMapper->setMapping(lineEdit, i);

                //disconnect(lineEdit, SIGNAL(returnPressed()));
                //disconnect(m_LineEditsSignalMapper, SIGNAL(mapped(int)));

                //connect(lineEdit, SIGNAL(returnPressed()), m_LineEditsSignalMapper, SLOT(map()));
                //connect(m_LineEditsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnRemoveInputPinClicked(int)));

                m_ButtonsSignalMapper->removeMappings(button);
                //m_ButtonsSignalMapper->setMapping(button, i);

                //disconnect(button, SIGNAL(pressed()));
                //disconnect(m_ButtonsSignalMapper, SIGNAL(mapped(int)));

                //connect(button, SIGNAL(pressed()), m_ButtonsSignalMapper, SLOT(map()));
                //connect(m_ButtonsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnRemoveInputPinClicked(int)));
            }
#endif

            // Remove input sockets
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->removeInputSocket(ui.ip_pins_layout->rowCount() - 1);
                m_CurrentEditingPassNode->update();
            }

            m_NodeGraphWidget->repaint();
        }
        //-----------------------------------------------------------
        void RZEFrameGraphEditor::OnAddOutputPinClicked()
        {
            auto pinNameEdit = new QLineEdit();
            pinNameEdit->setPlaceholderText("Enter output socket name");

            m_OpLineEditsSignalMapper->setMapping(pinNameEdit, ui.op_pins_layout->rowCount());

            connect(pinNameEdit, SIGNAL(returnPressed()), m_OpLineEditsSignalMapper, SLOT(map()), Qt::UniqueConnection);
            connect(m_OpLineEditsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnOutputPinNameChanged(int)), Qt::UniqueConnection);

            auto label = "output pin #" + std::to_string(ui.op_pins_layout->rowCount());
            ui.op_pins_layout->addRow(label.c_str(), pinNameEdit);

            // Add output node
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->addOutputSocket(pinNameEdit->text().toStdString());
                m_CurrentEditingPassNode->update();
            }

            m_NodeGraphWidget->repaint();
        }

        void RZEFrameGraphEditor::OnOutputPinNameChanged(int idx)
        {
            auto FieldLayoutItem = ui.op_pins_layout->itemAt(idx, QFormLayout::ItemRole::FieldRole);

            QWidget*   widget   = qobject_cast<QWidget*>(FieldLayoutItem->widget());
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);

            std::cout << lineEdit->text().toStdString() << std::endl;

            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->getOutputSocket(idx - 1)->setSocketName(lineEdit->text().toStdString());
                m_CurrentEditingPassNode->update();
            }

            m_NodeGraphWidget->repaint();
        }

        void RZEFrameGraphEditor::OnRemoveOutputPinClicked()
        {
            // Always remove the last pin
            if (ui.op_pins_layout->rowCount() > 1) {
                ui.op_pins_layout->removeRow(ui.op_pins_layout->rowCount() - 1);
                ui.op_pins_layout->update();
            }

            // Remove output sockets
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->removeOutputSocket(ui.op_pins_layout->rowCount() - 1);
                m_CurrentEditingPassNode->update();
            }

            m_NodeGraphWidget->repaint();
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::OnAddColorFormatClicked()
        {
            auto formatsComboBox = new QComboBox();
            formatsComboBox->addItems(m_FormatsStringList);
            formatsComboBox->setCurrentIndex((u32) Razix::Graphics::TextureFormat::RGBA32F);    // Default format = RGBA32F

            m_ColorFormatsComboBoxSignalMapper->setMapping(formatsComboBox, ui.color_formats_layout->rowCount());

            connect(formatsComboBox, SIGNAL(currentIndexChanged(int)), m_ColorFormatsComboBoxSignalMapper, SLOT(map()), Qt::UniqueConnection);
            connect(m_ColorFormatsComboBoxSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnColorFormatChanged(int)), Qt::UniqueConnection);

            auto label = "#" + std::to_string(ui.color_formats_layout->rowCount());
            ui.color_formats_layout->addRow(label.c_str(), formatsComboBox);

            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->getPipelineSettings().colorFormats.push_back(formatsComboBox->currentText().toStdString());
            }

            m_NodeGraphWidget->repaint();
        }

        void RZEFrameGraphEditor::OnColorFormatChanged(int idx)
        {
            auto FieldLayoutItem = ui.color_formats_layout->itemAt(idx, QFormLayout::ItemRole::FieldRole);

            QWidget*   widget   = qobject_cast<QWidget*>(FieldLayoutItem->widget());
            QComboBox* comboBox = qobject_cast<QComboBox*>(widget);

            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->getPipelineSettings().colorFormats[idx] = comboBox->currentText().toStdString();
            }
        }

        void RZEFrameGraphEditor::OnRemoveColorFormatClicked()
        {
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->getPipelineSettings().colorFormats.pop_back();
            }

            // Always remove the last pin
            if (ui.color_formats_layout->rowCount() > 0) {
                ui.color_formats_layout->removeRow(ui.color_formats_layout->rowCount() - 1);
                ui.color_formats_layout->update();
            }

            m_NodeGraphWidget->repaint();
        }
        //-----------------------------------------------------------
        void RZEFrameGraphEditor::OnBrowseShaderPressed()
        {
            // TODO: Use VFS to get shader files directory and pass it here
            auto fileName = QFileDialog::getOpenFileName(this, "Select Razix Shader File", "", tr("Razix Shader File (*.rzsf)"));

            auto shaderName = Utilities::RemoveFilePathExtension(Utilities::GetFileName(fileName.toStdString()));
            ui.shaderFileLocation->setText(shaderName.c_str());

            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->setShaderName(shaderName);
            }
        }
        //-----------------------------------------------------------
        void RZEFrameGraphEditor::OnPipelineNameChanged()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().pipelineName = ui.pipelineName->text().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineCullModeSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().cullMode = ui.cullMode->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelinePolygonModeSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().polygonMode = ui.polygonMode->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineDrawTypeSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().drawMode = ui.drawType->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineEnableTransparencyChecked()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().enableTransparencey = ui.enableTransparency->isChecked();
        }

        void RZEFrameGraphEditor::OnPipelineEnableDepthTestChecked()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().enableDepthTest = ui.enableDepthTest->isChecked();
        }

        void RZEFrameGraphEditor::OnPipelineEnableDepthWriteChecked()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().enableDepthWrite = ui.enableDepthWrite->isChecked();
        }

        void RZEFrameGraphEditor::OnPipelineDepthOperationSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().depthOperation = ui.depthOperation->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineColorSrcSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().colorSrc = ui.colorSrc->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineColorDstSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().colorDst = ui.colorDst->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineColorOperationSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().colorOp = ui.colorOp->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineAlphaSrcSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().alphaOp = ui.alphaOp->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineAlphaDstSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().alphaOp = ui.alphaOp->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineAlphaOperationSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().alphaOp = ui.alphaOp->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnPipelineDepthFormatSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getPipelineSettings().depthFormat = ui.depthFormat->currentText().toStdString();
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::OnSceneGeometryModeSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getSceneSettings().geometryMode = ui.sceneGeometry->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnEnableResize()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getSceneSettings().enableResize = ui.enableResize->isChecked();
        }

        void RZEFrameGraphEditor::OnResolutionSelected()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getSceneSettings().resolution = ui.resolution->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnExtentXChanged()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getSceneSettings().extents.x = ui.extentX->text().toInt();
        }

        void RZEFrameGraphEditor::OnExtentYChanged()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getSceneSettings().extents.y = ui.extentY->text().toInt();
        }

        void RZEFrameGraphEditor::OnLayersChanged()
        {
            if (m_CurrentEditingPassNode)
                m_CurrentEditingPassNode->getSceneSettings().layers = ui.layers->text().toInt();
        }

        //-----------------------------------------------------------

        // Buffer Resource Panel
        void RZEFrameGraphEditor::OnBufferNameChanged()
        {
            if (m_CurrentEditingBufferNode)
                m_CurrentEditingBufferNode->setTitle(ui.bufferName->text().toStdString());
        }

        void RZEFrameGraphEditor::OnBufferSizeChanged()
        {
            if (m_CurrentEditingBufferNode)
                m_CurrentEditingBufferNode->setSize(ui.bufferSize->text().toInt());
        }

        void RZEFrameGraphEditor::OnBufferUsageSelected()
        {
            if (m_CurrentEditingBufferNode)
                m_CurrentEditingBufferNode->setUsage(ui.bufferUsage->currentText().toStdString());
        }

        // Texture Resource Panel
        void RZEFrameGraphEditor::OnTextureNameChanged()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->setTitle(ui.textureName->text().toStdString());
        }

        void RZEFrameGraphEditor::OnTextureWidthChanged()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Width = ui.textureWidth->text().toInt();
        }

        void RZEFrameGraphEditor::OnTextureHeightChanged()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Height = ui.textureHeight->text().toInt();
        }

        void RZEFrameGraphEditor::OnTextureDepthChanged()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Depth = ui.textureDepth->text().toInt();
        }

        void RZEFrameGraphEditor::OnTextureLayersChanged()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Layers = ui.textureLayers->text().toInt();
        }

        void RZEFrameGraphEditor::OnTextureTypeSelected()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Type = ui.textureType->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnTextureFormatSelected()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Format = ui.textureFormat->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnTextureWrapModeSelected()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_Wrapping = ui.wrapMode->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnTextureFilteringMinModeSelected()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_FilteringMin = ui.filterModeMin->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnTextureFilteringMagModeSelected()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_FilteringMag = ui.filterModeMag->currentText().toStdString();
        }

        void RZEFrameGraphEditor::OnEnableMips()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_EnableMips = ui.enableMips->isChecked();
        }

        void RZEFrameGraphEditor::OnEnableIsHDR()
        {
            if (m_CurrentEditingTextureNode)
                m_CurrentEditingTextureNode->m_IsHDR = ui.isHDR->isChecked();
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::OnImportNameChanged()
        {
            if (m_CurrentEditingImportNode)
                m_CurrentEditingImportNode->setTitle(ui.importResourceName->text().toStdString());
        }

        void RZEFrameGraphEditor::OnImportTextureBrowsePressed()
        {
            // TODO: Use VFS to get shader files directory and pass it here
            auto fileName = QFileDialog::getOpenFileName(this, "Select Texture File to import", "", tr("image file (*.png *jpg *jpeg)"));

            ui.importedTexturePath->setText(fileName.toStdString().c_str());

            if (m_CurrentEditingImportNode) {
                m_CurrentEditingImportNode->m_TexturePath = ui.importedTexturePath->text().toStdString();
            }
        }

        //-----------------------------------------------------------
        // Toolbar

        void RZEFrameGraphEditor::OnOpenPressed()
        {
            auto fileName        = QFileDialog::getOpenFileName(this, "Select Frame Graph File to import", "./Engine/content/FrameGraphs/Graphs/", tr("frame graph(*.json)"));
            m_FrameGraphFilePath = fileName.toStdString();

            parseJSONtoNodeScene(m_FrameGraphFilePath);
        }

        void RZEFrameGraphEditor::OnSavePressed()
        {
            if (m_FrameGraphFilePath.empty()) {
                auto fileName = QFileDialog::getSaveFileName(this, "Export Frame Graph to JSON", "", tr("frame graph (*.json)"));

                // Store the current file name and path
                if (!fileName.isEmpty()) {
                    auto label = std::string("FrameGraph File : ") + fileName.toStdString();
                    ui.fg_file_name->setText(label.c_str());

                    m_FrameGraphFilePath = fileName.toStdString();

                    // Now parse the Graph and start exporting to JSON file
                    exportNodeScenetoJSON();
                }
            } else {
                // Now parse the Graph and start exporting to JSON file
                exportNodeScenetoJSON();
            }
        }

        void RZEFrameGraphEditor::OnCompilePressed()
        {
            if (!m_FrameGraphFilePath.empty()) {
                //// Save again just in case we have any unsaved work
                //exportNodeScenetoJSON();
                //
                //// use a dummy FrameGraph class to compile the Exported JSON file to validate it
                //RAZIX_INFO("[Frame Graph Editor] Parsing Exported FrameGraph ....");
                //m_FrameGraph.parse(m_FrameGraphFilePath);
                //
                //// TODO!!!: FIXME Need to build this shit! for it to pick up import stuff, maybe separate world renderer into multiple function to make this possible, for now disable compilation!
                //
                //// Compile the Frame Graph
                //RAZIX_INFO("[Frame Graph Editor] Compiling FrameGraph ....");
                //m_FrameGraph.compile();
                //
                //// Dump the compiled Frame Graph for static visualization
                //auto location       = Utilities::GetFileLocation(m_FrameGraphFilePath);
                //auto name           = Utilities::GetFileName(m_FrameGraphFilePath);
                //auto exportLocation = location + "/../FrameGraph.Compiled." + name + ".dot";
                //
                //m_FrameGraph.exportToGraphViz(exportLocation);
            }
        }

        void RZEFrameGraphEditor::OnHotReloadPressed()
        {
            // 1. Save to JSON
            // 2. Check for compilation status, if not compile again
            //OnCompilePressed();

            // 3. Build the frame graph using the current exported JSON file
            //  3.1 Halt the Engine Rendering and Flush the GPU work
            // TODO: Only if we get a engine crash without halting
            //  3.2 Build the Framegraph into the current WorldRenderer Interface
            // 4. Resume Engine Rendering safely
            if (!m_FrameGraphFilePath.empty()) {
                //--------------------------------------------
                // HALT ENGINE
                //std::lock_guard<std::mutex> lk(Razix::RZApplication::m);
                //RZApplication::ready_for_execution = false;
                //RAZIX_INFO("Triggering worker thread to halt execution ::::");
                //RZApplication::halt_execution.notify_one();
                //--------------------------------------------
                Razix::RZEngine::Get().getWorldRenderer().setFrameGraphFilePath(m_FrameGraphFilePath);
                //--------------------------------------------
                // RESUME ENGINE
                //RZApplication::ready_for_execution = true;
                //RAZIX_INFO("Triggering worker thread to resume execution ::::");
                //RZApplication::halt_execution.notify_one();
                //--------------------------------------------
            }
        }

        void RZEFrameGraphEditor::OnSetAsStandAlonePressed()
        {
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->setIsStandAlonePass(!m_CurrentEditingPassNode->getIsStandAlonePass());
                m_CurrentEditingPassNode->update();
            }
        }

        void RZEFrameGraphEditor::OnAddBreakpointPressed()
        {
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->setHasBreakPoint(!m_CurrentEditingPassNode->getHasBreakPoint());
                m_CurrentEditingPassNode->update();
            }
        }

        void RZEFrameGraphEditor::OnFinalOutputPressed()
        {
            // TODO: Iterate and clear final output status for all other TextureResoruceNodes (maybe use signals and emit one here)
            // For now we will do it manually

            if (m_CurrentEditingTextureNode) {
                m_CurrentEditingTextureNode->setIsFinalOutput(!m_CurrentEditingTextureNode->getIsFinalOutput());
                if (m_CurrentEditingTextureNode->getIsFinalOutput())
                    m_FinalOutputName = m_CurrentEditingTextureNode->getTitle();
                m_CurrentEditingTextureNode->update();
            }
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::initializePassNodePropertiesInspector()
        {
            connect(ui.PassName, SIGNAL(returnPressed()), this, SLOT(OnPassNodeNameChanged()));

            //------------------
            // Pins
            //------------------

            connect(ui.add_ip_pin, SIGNAL(pressed()), this, SLOT(OnAddInputPinClicked()));
            connect(ui.remove_ip_pin, SIGNAL(pressed()), this, SLOT(OnRemoveInputPinClicked()));

            connect(ui.add_op_pin, SIGNAL(pressed()), this, SLOT(OnAddOutputPinClicked()));
            connect(ui.remove_op_pin, SIGNAL(pressed()), this, SLOT(OnRemoveOutputPinClicked()));

            // https://stackoverflow.com/questions/5153157/passing-an-argument-to-a-slot
            m_IpLineEditsSignalMapper          = new QSignalMapper(this);
            m_OpLineEditsSignalMapper          = new QSignalMapper(this);
            m_ColorFormatsComboBoxSignalMapper = new QSignalMapper(this);

            //------------------
            // Shader
            //------------------

            connect(ui.browseShaderFIleBtn, SIGNAL(pressed()), this, SLOT(OnBrowseShaderPressed()));

            //------------------
            // Pipeline
            //------------------

            connect(ui.pipelineName, SIGNAL(returnPressed()), this, SLOT(OnPipelineNameChanged()));

            connect(ui.cullMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineCullModeSelected()));
            // Fill cull mode combo box
            for (u32 i = 0; i < (u32) Graphics::CullMode::COUNT; i++)
                ui.cullMode->addItem(Graphics::CullModeNames[i]);
            ui.cullMode->setCurrentIndex((u32) Graphics::CullMode::Back);

            connect(ui.polygonMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelinePolygonModeSelected()));
            for (u32 i = 0; i < (u32) Graphics::PolygonMode::COUNT; i++)
                ui.polygonMode->addItem(Graphics::PolygonModeNames[i]);
            ui.polygonMode->setCurrentIndex((u32) Graphics::PolygonMode::Fill);

            connect(ui.drawType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineDrawTypeSelected()));
            for (u32 i = 0; i < (u32) Graphics::DrawType::COUNT; i++)
                ui.drawType->addItem(Graphics::DrawTypeNames[i]);
            ui.drawType->setCurrentIndex((u32) Graphics::DrawType::Triangle);

            connect(ui.enableTransparency, SIGNAL(stateChanged(int)), this, SLOT(OnPipelineEnableTransparencyChecked()));
            connect(ui.enableDepthTest, SIGNAL(stateChanged(int)), this, SLOT(OnPipelineEnableDepthTestChecked()));
            connect(ui.enableDepthWrite, SIGNAL(stateChanged(int)), this, SLOT(OnPipelineEnableDepthWriteChecked()));

            connect(ui.depthOperation, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineDepthOperationSelected()));
            for (u32 i = 0; i < (u32) Graphics::CompareOp ::COUNT; i++)
                ui.depthOperation->addItem(Graphics::CompareOpNames[i]);
            ui.depthOperation->setCurrentIndex((u32) Graphics::CompareOp::Less);

            // Blend Factor
            connect(ui.colorSrc, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineColorSrcSelected()));
            connect(ui.colorDst, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineColorDstSelected()));
            connect(ui.alphaSrc, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineAlphaSrcSelected()));
            connect(ui.alphaDst, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineAlphaDstSelected()));
            for (u32 i = 0; i < (u32) Graphics::BlendFactor::COUNT; i++) {
                ui.colorSrc->addItem(Graphics::BlendFactorNames[i]);
                ui.colorDst->addItem(Graphics::BlendFactorNames[i]);
                ui.alphaSrc->addItem(Graphics::BlendFactorNames[i]);
                ui.alphaDst->addItem(Graphics::BlendFactorNames[i]);
            }
            ui.colorSrc->setCurrentIndex((u32) Graphics::BlendFactor::SrcAlpha);
            ui.colorDst->setCurrentIndex((u32) Graphics::BlendFactor::OneMinusSrcAlpha);
            ui.alphaSrc->setCurrentIndex((u32) Graphics::BlendFactor::One);
            ui.alphaDst->setCurrentIndex((u32) Graphics::BlendFactor::One);

            // Blend Op
            connect(ui.colorOp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineColorOperationSelected()));
            connect(ui.alphaOp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineAlphaOperationSelected()));
            for (u32 i = 0; i < (u32) Graphics::BlendOp::COUNT; i++) {
                ui.colorOp->addItem(Graphics::BlendOpNames[i]);
                ui.alphaOp->addItem(Graphics::BlendOpNames[i]);
            }
            ui.colorOp->setCurrentIndex((u32) Graphics::BlendOp::Add);
            ui.alphaOp->setCurrentIndex((u32) Graphics::BlendOp::Add);

            // Fill the formats combobox items
            for (u32 i = 0; i < (u32) Graphics::TextureFormat::COUNT; i++)
                m_FormatsStringList.push_back(Razix::Graphics::TextureFormatNames[i]);

            // Depth format
            connect(ui.depthFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(OnPipelineDepthFormatSelected()));
            ui.depthFormat->addItems(m_FormatsStringList);
            ui.depthFormat->setCurrentIndex((u32) Graphics::TextureFormat::DEPTH32F);

            connect(ui.add_color_format, SIGNAL(pressed()), this, SLOT(OnAddColorFormatClicked()));
            connect(ui.remove_color_format, SIGNAL(pressed()), this, SLOT(OnRemoveColorFormatClicked()));

            //------------------
            // Scene props
            //------------------

            // Fill the geom mode combobox items
            connect(ui.sceneGeometry, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSceneGeometryModeSelected()));
            for (u32 i = 0; i < (u32) Razix::SceneDrawGeometryMode::COUNT; i++)
                ui.sceneGeometry->addItem(Razix::SceneDrawGeometryModeNames[i]);
            ui.sceneGeometry->setCurrentIndex((u32) SceneDrawGeometryMode::SceneGeometry);

            connect(ui.enableResize, SIGNAL(stateChanged(int)), this, SLOT(OnEnableResize()));

            connect(ui.resolution, SIGNAL(currentIndexChanged(int)), this, SLOT(OnResolutionSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::Resolution::COUNT; i++)
                ui.resolution->addItem(Razix::Graphics::ResolutionNames[i]);

            // Extents X & Y
            connect(ui.extentX, SIGNAL(returnPressed()), this, SLOT(OnExtentXChanged()));
            ui.extentX->setValidator(new QIntValidator(-10000, 10000, this));
            connect(ui.extentY, SIGNAL(returnPressed()), this, SLOT(OnExtentYChanged()));
            ui.extentY->setValidator(new QIntValidator(-10000, 10000, this));

            connect(ui.layers, SIGNAL(returnPressed()), this, SLOT(OnLayersChanged()));
            ui.layers->setValidator(new QIntValidator(0, 1024, this));
        }

        void RZEFrameGraphEditor::initializeResourceNodePropertiesInspector()
        {
            // Buffer Resource
            connect(ui.bufferName, SIGNAL(returnPressed()), this, SLOT(OnBufferNameChanged()));
            connect(ui.bufferSize, SIGNAL(returnPressed()), this, SLOT(OnBufferSizeChanged()));
            ui.bufferSize->setValidator(new QIntValidator(0, Gib(1), this));
            connect(ui.bufferUsage, SIGNAL(currentIndexChanged(int)), this, SLOT(OnBufferUsageSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::BufferUsage::COUNT; i++)
                ui.bufferUsage->addItem(Razix::Graphics::BufferUsageNames[i]);
            ui.bufferUsage->setCurrentIndex(0);
            // TODO: enable data reflection check box

            // Texture Resource
            connect(ui.textureName, SIGNAL(returnPressed()), this, SLOT(OnTextureNameChanged()));
            connect(ui.textureWidth, SIGNAL(returnPressed()), this, SLOT(OnTextureWidthChanged()));
            ui.textureWidth->setValidator(new QIntValidator(0, Mib(1), this));

            connect(ui.textureHeight, SIGNAL(returnPressed()), this, SLOT(OnTextureHeightChanged()));
            ui.textureHeight->setValidator(new QIntValidator(0, Mib(1), this));

            connect(ui.textureDepth, SIGNAL(returnPressed()), this, SLOT(OnTextureDepthChanged()));
            ui.textureDepth->setValidator(new QIntValidator(0, Mib(1), this));

            connect(ui.textureLayers, SIGNAL(returnPressed()), this, SLOT(OnTextureLayersChanged()));
            ui.textureLayers->setValidator(new QIntValidator(0, Mib(1), this));

            connect(ui.textureType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTextureTypeSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::TextureType::COUNT; i++)
                ui.textureType->addItem(Razix::Graphics::TextureTypeNames[i]);
            ui.textureType->setCurrentIndex((u32) Razix::Graphics::TextureType::Texture_2D);

            connect(ui.textureFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTextureFormatSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::TextureFormat::COUNT; i++)
                ui.textureFormat->addItem(Razix::Graphics::TextureFormatNames[i]);
            ui.textureFormat->setCurrentIndex((u32) Razix::Graphics::TextureFormat::RGBA32F);

            connect(ui.wrapMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTextureWrapModeSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::Wrapping::COUNT; i++)
                ui.wrapMode->addItem(Razix::Graphics::WrappingNames[i]);
            ui.wrapMode->setCurrentIndex((u32) Razix::Graphics::Wrapping::CLAMP_TO_EDGE);

            connect(ui.filterModeMin, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTextureFilteringMinModeSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::Filtering::Mode::COUNT; i++)
                ui.filterModeMin->addItem(Razix::Graphics::FitleringModeNames[i]);
            ui.filterModeMin->setCurrentIndex((u32) Razix::Graphics::Filtering::Mode::LINEAR);

            connect(ui.filterModeMag, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTextureFilteringMagModeSelected()));
            for (u32 i = 0; i < (u32) Razix::Graphics::Filtering::Mode::COUNT; i++)
                ui.filterModeMag->addItem(Razix::Graphics::FitleringModeNames[i]);
            ui.filterModeMag->setCurrentIndex((u32) Razix::Graphics::Filtering::Mode::LINEAR);

            connect(ui.enableMips, SIGNAL(stateChanged(int)), this, SLOT(OnEnableMips()));
            connect(ui.isHDR, SIGNAL(stateChanged(int)), this, SLOT(OnEnableIsHDR()));
        }

        void RZEFrameGraphEditor::initializeImportNodePropertiesInspector()
        {
            connect(ui.importResourceName, SIGNAL(returnPressed()), this, SLOT(OnImportNameChanged()));
            connect(ui.browseImportTextureBtn, SIGNAL(pressed()), this, SLOT(OnImportTextureBrowsePressed()));
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::populatePopertiesPanelWithPassNode()
        {
            //------------------
            // Pins
            //------------------

            // Clear the widget stuff and set as per this node
            // 1. clear the sockets and set as per this node
            while (ui.ip_pins_layout->rowCount() > 1) {
                ui.ip_pins_layout->removeRow(ui.ip_pins_layout->rowCount() - 1);
                ui.ip_pins_layout->update();
            }

            while (ui.op_pins_layout->rowCount() > 1) {
                ui.op_pins_layout->removeRow(ui.op_pins_layout->rowCount() - 1);
                ui.op_pins_layout->update();
            }

            // Clear the color formats layout
            while (ui.color_formats_layout->rowCount() > 0) {
                ui.color_formats_layout->removeRow(ui.color_formats_layout->rowCount() - 1);
                ui.color_formats_layout->update();
            }

            ui.PassName->setText(m_CurrentEditingPassNode->getTitle().c_str());

            for (i32 i = 0; i < m_CurrentEditingPassNode->getInputSockets().size(); i++) {
                auto pinNameEdit = new QLineEdit();
                pinNameEdit->setPlaceholderText("Enter input socket name");

                m_IpLineEditsSignalMapper->setMapping(pinNameEdit, i + 1);

                connect(pinNameEdit, SIGNAL(returnPressed()), m_IpLineEditsSignalMapper, SLOT(map()), Qt::UniqueConnection);
                connect(m_IpLineEditsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnInputPinNameChanged(int)), Qt::UniqueConnection);

                auto label = "input pin #" + std::to_string(i + 1);
                ui.ip_pins_layout->addRow(label.c_str(), pinNameEdit);

                pinNameEdit->setText(m_CurrentEditingPassNode->getInputSocket(i)->getSocketName().c_str());

                ui.ip_pins_layout->update();
            }

            for (i32 i = 0; i < m_CurrentEditingPassNode->getOutputSockets().size(); i++) {
                auto pinNameEdit = new QLineEdit();
                pinNameEdit->setPlaceholderText("Enter output socket name");

                m_OpLineEditsSignalMapper->setMapping(pinNameEdit, i + 1);

                connect(pinNameEdit, SIGNAL(returnPressed()), m_OpLineEditsSignalMapper, SLOT(map()), Qt::UniqueConnection);
                connect(m_OpLineEditsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnOutputPinNameChanged(int)), Qt::UniqueConnection);

                auto label = "output pin #" + std::to_string(i + 1);
                ui.op_pins_layout->addRow(label.c_str(), pinNameEdit);

                pinNameEdit->setText(m_CurrentEditingPassNode->getOutputSocket(i)->getSocketName().c_str());

                ui.op_pins_layout->update();
            }

            //------------------
            // Shader
            //------------------
            ui.shaderFileLocation->setText(m_CurrentEditingPassNode->getShaderName().c_str());

            //------------------
            // Pipeline
            //------------------
            auto& pipelineSettings = m_CurrentEditingPassNode->getPipelineSettings();
            ui.pipelineName->setText(pipelineSettings.pipelineName.c_str());
            ui.cullMode->setCurrentIndex(ui.cullMode->findText(pipelineSettings.cullMode.c_str()));
            ui.polygonMode->setCurrentIndex(ui.polygonMode->findText(pipelineSettings.polygonMode.c_str()));
            ui.drawType->setCurrentIndex(ui.drawType->findText(pipelineSettings.drawMode.c_str()));
            ui.enableTransparency->setChecked(pipelineSettings.enableTransparencey);
            ui.enableDepthWrite->setChecked(pipelineSettings.enableDepthWrite);
            ui.enableDepthTest->setChecked(pipelineSettings.enableDepthTest);
            ui.depthOperation->setCurrentIndex(ui.depthOperation->findText(pipelineSettings.depthOperation.c_str()));
            ui.colorSrc->setCurrentIndex(ui.colorSrc->findText(pipelineSettings.colorSrc.c_str()));
            ui.colorDst->setCurrentIndex(ui.colorDst->findText(pipelineSettings.colorDst.c_str()));
            ui.colorOp->setCurrentIndex(ui.colorOp->findText(pipelineSettings.colorOp.c_str()));
            ui.alphaSrc->setCurrentIndex(ui.alphaSrc->findText(pipelineSettings.alphaSrc.c_str()));
            ui.alphaDst->setCurrentIndex(ui.alphaDst->findText(pipelineSettings.alphaDst.c_str()));
            ui.alphaOp->setCurrentIndex(ui.alphaOp->findText(pipelineSettings.alphaOp.c_str()));
            ui.depthFormat->setCurrentIndex(ui.depthFormat->findText(pipelineSettings.depthFormat.c_str()));

            for (i32 i = 0; i < pipelineSettings.colorFormats.size(); i++) {
                auto formatsComboBox = new QComboBox();
                formatsComboBox->addItems(m_FormatsStringList);
                formatsComboBox->setCurrentIndex(formatsComboBox->findText(pipelineSettings.colorFormats[i].c_str()));

                m_ColorFormatsComboBoxSignalMapper->setMapping(formatsComboBox, ui.color_formats_layout->rowCount());

                connect(formatsComboBox, SIGNAL(currentIndexChanged(int)), m_ColorFormatsComboBoxSignalMapper, SLOT(map()), Qt::UniqueConnection);
                connect(m_ColorFormatsComboBoxSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnColorFormatChanged(int)), Qt::UniqueConnection);

                auto label = "#" + std::to_string(ui.color_formats_layout->rowCount());
                ui.color_formats_layout->addRow(label.c_str(), formatsComboBox);
            }

            //------------------
            // Scene
            //------------------
            auto& sceneSettings = m_CurrentEditingPassNode->getSceneSettings();
            ui.sceneGeometry->setCurrentIndex(ui.sceneGeometry->findText(sceneSettings.geometryMode.c_str()));
            ui.enableResize->setChecked(sceneSettings.enableResize);
            ui.resolution->setCurrentIndex(ui.resolution->findText(sceneSettings.resolution.c_str()));
            ui.extentX->setText(std::to_string(sceneSettings.extents.x).c_str());
            ui.extentY->setText(std::to_string(sceneSettings.extents.y).c_str());
            ui.layers->setText(std::to_string(sceneSettings.layers).c_str());
        }

        void RZEFrameGraphEditor::populatePopertiesPanelWithBufferNode()
        {
            ui.bufferName->setText(m_CurrentEditingBufferNode->getTitle().c_str());
            ui.bufferSize->setText(std::to_string(m_CurrentEditingBufferNode->getSize()).c_str());
            ui.bufferUsage->setCurrentIndex(ui.bufferUsage->findText(m_CurrentEditingBufferNode->getUsage().c_str()));
        }

        void RZEFrameGraphEditor::populatePopertiesPanelWithTextureNode()
        {
            ui.textureName->setText(m_CurrentEditingTextureNode->getTitle().c_str());
            ui.textureWidth->setText(std::to_string(m_CurrentEditingTextureNode->m_Width).c_str());
            ui.textureHeight->setText(std::to_string(m_CurrentEditingTextureNode->m_Height).c_str());
            ui.textureDepth->setText(std::to_string(m_CurrentEditingTextureNode->m_Depth).c_str());
            ui.textureLayers->setText(std::to_string(m_CurrentEditingTextureNode->m_Layers).c_str());

            ui.textureType->setCurrentIndex(ui.textureType->findText(m_CurrentEditingTextureNode->m_Type.c_str()));
            ui.textureFormat->setCurrentIndex(ui.textureFormat->findText(m_CurrentEditingTextureNode->m_Format.c_str()));
            ui.wrapMode->setCurrentIndex(ui.wrapMode->findText(m_CurrentEditingTextureNode->m_Wrapping.c_str()));
            ui.filterModeMin->setCurrentIndex(ui.filterModeMin->findText(m_CurrentEditingTextureNode->m_FilteringMin.c_str()));
            ui.filterModeMag->setCurrentIndex(ui.filterModeMag->findText(m_CurrentEditingTextureNode->m_FilteringMag.c_str()));

            ui.enableMips->setChecked(m_CurrentEditingTextureNode->m_EnableMips);
            ui.isHDR->setChecked(m_CurrentEditingTextureNode->m_IsHDR);
        }

        void RZEFrameGraphEditor::populatePopertiesPanelWithImportNode()
        {
            ui.importResourceName->setText(m_CurrentEditingImportNode->getTitle().c_str());
            ui.importedTexturePath->setText(m_CurrentEditingImportNode->m_TexturePath.c_str());
        }

        //-----------------------------------------------------------

        void RZEFrameGraphEditor::initializePresetPassNodesList()
        {
        }

        void RZEFrameGraphEditor::initializePresetImportNodesList()
        {
            for (auto& importPreset: ImportNodesPresets) {
                auto label = new QLabel(importPreset.c_str());
                label->setMargin(6);
                label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                QFont font = label->font();
                font.setPointSize(16);
                font.setBold(true);
                label->setFont(font);
                label->setObjectName(importPreset.c_str());
                label->setStyleSheet(ImportNodeStyleSheet);
                label->setGraphicsEffect(DropShadowEffect);

                auto listItem = new QListWidgetItem;
                listItem->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                listItem->setSizeHint(QSize(listItem->sizeHint().width(), 40));
                ui.preset_import_list->addItem(listItem);
                ui.preset_import_list->setItemWidget(listItem, label);
            }

            // Add a button at end to import a resource
            auto ImportButton = new QPushButton;
            ImportButton->setText("Import");
            ImportButton->setStyleSheet("background-color: rgb(66, 66, 66);\nborder-width:2px;\nborder-color: rgb(195, 195, 195);\nborder-radius:8px;\nborder-style:solid;\n font-size:16px;");
            ImportButton->setGraphicsEffect(DropShadowEffect);
            connect(ImportButton, SIGNAL(pressed()), this, SLOT(OnImportPresetButtonClicked()));

            auto listItem = new QListWidgetItem;
            ui.preset_import_list->addItem(listItem);
            ui.preset_import_list->setItemWidget(listItem, ImportButton);
        }

        // TODO: https://stackoverflow.com/questions/46920221/qlistwidget-drag-and-drop-with-a-custom-widget-set-via-setitemwidget

        void RZEFrameGraphEditor::initializePresetResourceNodesList()
        {
            for (auto& resourcePreset: ResourceNodesPresets) {
                auto label = new QLabel(resourcePreset.first.c_str());
                label->setMargin(4);
                label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                QFont font = label->font();
                font.setPointSize(16);
                label->setFont(font);
                label->setObjectName(resourcePreset.first.c_str());
                label->setStyleSheet(resourcePreset.second.c_str());
                label->setGraphicsEffect(DropShadowEffect);

                auto listItem = new QListWidgetItem;
                listItem->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                listItem->setSizeHint(QSize(listItem->sizeHint().width(), 40));
                ui.preset_resources_list->addItem(listItem);
                ui.preset_resources_list->setItemWidget(listItem, label);
            }
        }

        //-----------------------------------------------------------
        // Toolbar

        void RZEFrameGraphEditor::setupToolbar()
        {
            connect(ui.open_fg, SIGNAL(pressed()), this, SLOT(OnOpenPressed()));
            connect(ui.save_fg, SIGNAL(pressed()), this, SLOT(OnSavePressed()));

            connect(ui.compile, SIGNAL(pressed()), this, SLOT(OnCompilePressed()));
            connect(ui.hot_reload, SIGNAL(pressed()), this, SLOT(OnHotReloadPressed()));

            connect(ui.mark_as_standalone, SIGNAL(pressed()), this, SLOT(OnSetAsStandAlonePressed()));
            connect(ui.add_breakpoint, SIGNAL(pressed()), this, SLOT(OnAddBreakpointPressed()));

            connect(ui.final_output, SIGNAL(pressed()), this, SLOT(OnFinalOutputPressed()));
        }

        //-----------------------------------------------------------
        // Other Misc

        void RZEFrameGraphEditor::exportNodeScenetoJSON()
        {
            // Create a file
            std::ofstream fg_file(m_FrameGraphFilePath, std::ofstream::out | std::ofstream::trunc);

            if (fg_file.is_open()) {
                // FG name
                ordered_json fg;

                fg["name"] = Utilities::RemoveFilePathExtension(Utilities::GetFileName(m_FrameGraphFilePath));

                auto passes = ordered_json::array();

                for (auto& node: m_NodeGraphWidget->getScene()->getNodes()) {
                    std::cout << node->getTitle() << std::endl;

                    // Only process if it's a Pass Node and not Resource Node
                    auto passNode = dynamic_cast<RZEPassNodeUI*>(node);

                    if (passNode) {
                        // Export the node as a Pass.json file and give it's file/path name as render_pass
                        std::string passName = "Pass.User." + node->getTitle();

                        // If the node is a Pass Node export it to JSON first
                        exportPassNodetoJSON(dynamic_cast<RZEPassNodeUI*>(node), passName);

                        auto passNode           = ordered_json::object();
                        passNode["render_pass"] = passName;

                        ordered_json inputs  = ordered_json::array();
                        ordered_json outputs = ordered_json::array();

                        /**
                         * 1. FrameData, SceneLightsData & Material descriptor sets are bound by the Scene so showing them in FrameGraph is just 
                         * for visualization and doesn't affect rendering, these resources are not managed by FG
                         * 
                         * 2. As for References we check if the input/out resource has a non-empty sockets if so, we mark it as a Reference
                         * 
                         * 3. Also one doesn't create input nodes, they are always given from someone so they are always references and outputs 
                         * can be ofc both so let's restrict some stuff and generate the JSON as we create more 
                         */

                        // Process the Inputs (PassNode has only one I/p & O/P socket, with multi edge support)
                        auto inputSocket = node->getInputSockets()[0];
                        {
                            // So from each input/output node we get the list of edges and get the nodes that they are connected to as input/output resources
                            auto inputEdges = inputSocket->getEdges();

                            for (u32 j = 0; j < inputEdges.size(); j++) {
                                ordered_json input;

                                auto startSocket = inputSocket->getEdges()[j]->getStartSocket();
                                auto endSocket   = inputSocket->getEdges()[j]->getEndSocket();

                                // Also the edge for this node can have the start and end sockets in any order so we need to check for self and select other other one
                                Socket* fromSocket = nullptr;

                                if (startSocket == inputSocket)
                                    fromSocket = endSocket;
                                else
                                    fromSocket = startSocket;

                                RAZIX_ASSERT(fromSocket, "[Frame Graph Editor] empty socket while exporting");

                                // Get the ResourceNode from which this socket it from and give it's name
                                auto bufferResourceNode  = dynamic_cast<RZEBufferResourceNodeUI*>(fromSocket->getNode());
                                auto textureResourceNode = dynamic_cast<RZETextureResourceNodeUI*>(fromSocket->getNode());
                                auto importResourceNode  = dynamic_cast<RZEImportNodeUI*>(fromSocket->getNode());

                                if (bufferResourceNode || textureResourceNode || importResourceNode) {
                                    input["name"] = fromSocket->getNode()->getTitle();

                                    // Since it's a input so always a reference, if we need add check for if the fromNode has any input nodes
                                    input["type"] = "Reference";

                                    if (importResourceNode)
                                        input["is_imported"] = true;

                                    inputs.push_back(input);
                                }
                            }
                        }

                        // Process the Outputs (PassNode has only one I/p & O/P socket, with multi edge support)
                        auto outputSocket = node->getOutputSockets()[0];
                        {
                            // So from each input/output node we get the list of edges and get the nodes that they are connected to as input/output resources
                            auto outputEdges = outputSocket->getEdges();

                            for (u32 j = 0; j < outputEdges.size(); j++) {
                                ordered_json output;

                                auto startSocket = outputSocket->getEdges()[j]->getStartSocket();
                                auto endSocket   = outputSocket->getEdges()[j]->getEndSocket();

                                // Also the edge for this node can have the start and end sockets in any order so we need to check for self and select other other one
                                Socket* toSocket = nullptr;

                                if (startSocket == outputSocket)
                                    toSocket = endSocket;
                                else
                                    toSocket = startSocket;

                                RAZIX_ASSERT(toSocket, "[Frame Graph Editor] empty socket while exporting");

                                // Get the ResourceNode from which this socket it from and give it's name
                                auto bufferResourceNode  = dynamic_cast<RZEBufferResourceNodeUI*>(toSocket->getNode());
                                auto textureResourceNode = dynamic_cast<RZETextureResourceNodeUI*>(toSocket->getNode());

                                if (bufferResourceNode) {
                                    output["type"]  = "Buffer";
                                    output["name"]  = bufferResourceNode->getTitle();
                                    output["size"]  = bufferResourceNode->getSize();
                                    output["usage"] = bufferResourceNode->getUsage();
                                } else if (textureResourceNode) {
                                    output["type"]             = "Texture";
                                    output["name"]             = textureResourceNode->getTitle();
                                    output["width"]            = textureResourceNode->m_Width;
                                    output["height"]           = textureResourceNode->m_Height;
                                    output["depth"]            = textureResourceNode->m_Depth;
                                    output["layers"]           = textureResourceNode->m_Layers;
                                    output["texture_type"]     = textureResourceNode->m_Type;
                                    output["format"]           = textureResourceNode->m_Format;
                                    output["wrapping"]         = textureResourceNode->m_Wrapping;
                                    output["filtering"]["min"] = textureResourceNode->m_FilteringMin;
                                    output["filtering"]["mag"] = textureResourceNode->m_FilteringMag;
                                    output["enable_mips"]      = textureResourceNode->m_EnableMips;
                                    output["hdr"]              = textureResourceNode->m_IsHDR;
                                }
                                outputs.push_back(output);
                            }
                        }
                        passNode["inputs"]  = inputs;
                        passNode["outputs"] = outputs;

                        passes.push_back(passNode);
                    }
                }

                // Set the Final output to which it will be rendered to
                fg["final_output"] = m_FinalOutputName;

                fg["passes"] = passes;

                // Dump to file
                fg_file << fg.dump(4);
            }
            fg_file.close();
        }

        void RZEFrameGraphEditor::exportPassNodetoJSON(RZEPassNodeUI* passNode, std::string& passNodeName)
        {
            // Create a file to save the render Pass file
            auto          passFilePath = Utilities::GetFileLocation(m_FrameGraphFilePath) + "/../Passes/" + passNodeName + ".json";
            std::ofstream pass_file(passFilePath, std::ofstream::out | std::ofstream::trunc);

            if (pass_file.is_open()) {
                ordered_json pass;

                pass["name"]          = passNodeName;
                pass["shader"]        = passNode->getShaderName();
                pass["is_standalone"] = passNode->getIsStandAlonePass();

                auto& pipeline = passNode->getPipelineSettings();
                {
                    pass["pipeline_info"]["cull_mode"]    = pipeline.cullMode;
                    pass["pipeline_info"]["polygon_mode"] = pipeline.polygonMode;
                    pass["pipeline_info"]["draw_type"]    = pipeline.drawMode;
                    pass["pipeline_info"]["cull_mode"]    = pipeline.cullMode;
                    pass["pipeline_info"]["transparency"] = pipeline.enableTransparencey;

                    ordered_json color_blend = ordered_json::object();
                    color_blend["src"]       = pipeline.colorSrc;
                    color_blend["dst"]       = pipeline.colorDst;
                    color_blend["op"]        = pipeline.colorOp;

                    ordered_json alpha_blend = ordered_json::object();
                    alpha_blend["src"]       = pipeline.alphaSrc;
                    alpha_blend["dst"]       = pipeline.alphaDst;
                    alpha_blend["op"]        = pipeline.alphaOp;

                    pass["pipeline_info"]["color_blend"] = color_blend;
                    pass["pipeline_info"]["alpha_blend"] = alpha_blend;

                    ordered_json depth = ordered_json::object();
                    depth["write"]     = pipeline.enableDepthWrite;
                    depth["test"]      = pipeline.enableDepthTest;
                    depth["op"]        = pipeline.depthOperation;

                    pass["pipeline_info"]["depth"] = depth;

                    pass["pipeline_info"]["depth_format"] = pipeline.depthFormat;

                    ordered_json color_formats = ordered_json::array();
                    for (u32 i = 0; i < pipeline.colorFormats.size(); i++)
                        color_formats.push_back(pipeline.colorFormats[i]);

                    pass["pipeline_info"]["color_formats"] = color_formats;
                }

                auto& sceneSettings                   = passNode->getSceneSettings();
                pass["scene_params"]["geometry_mode"] = sceneSettings.geometryMode;

                pass["rendering_info"]["resolution"] = sceneSettings.resolution;
                ordered_json extents;
                extents["x"]                      = sceneSettings.extents.x;
                extents["y"]                      = sceneSettings.extents.y;
                pass["rendering_info"]["extents"] = extents;

                pass["rendering_info"]["resize"] = sceneSettings.enableResize;
                pass["rendering_info"]["layers"] = sceneSettings.layers;

                // Dump to file
                pass_file << pass.dump(4);
            }
            pass_file.close();
        }

        void RZEFrameGraphEditor::parseJSONtoNodeScene(const std::string& framegraphFilePath)
        {
            //auto        jsonStrData = RZFileSystem::ReadTextFile(framegraphFilePath);

            // BUG: using VFS here to resolve the physical path is causing a thread change kinda issue and adding GUI elements from non-main threads in QT will cause application crash

            if (framegraphFilePath.empty())
                return;

            std::ifstream graphFileStream(framegraphFilePath);
            RAZIX_ASSERT(graphFileStream.is_open(), "[Frame Graph Editor] Cannot open framegraph JSON file, check path again!");
            json fg = json::parse(graphFileStream);

            // Name of Graph
            auto frameGraphName = fg["name"];

            // Iterate thought all the passes and created nodes & edges
            auto passes = fg["passes"];

            glm::vec2 nodePos{m_NodeGraphWidget->getOrigin().x(), m_NodeGraphWidget->getOrigin().y()};

            for (auto& pass: passes) {
                // Get the renderpass and use this to read another JSON file and create a RZEPassNodeUI
                auto render_pass  = pass["render_pass"];
                auto passFilePath = Utilities::GetFileLocation(framegraphFilePath) + "/../Passes/" + std::string(render_pass) + ".json";

                std::ifstream passFileStream(passFilePath);
                RAZIX_ASSERT(passFileStream.is_open(), "[Frame Graph Editor] Cannot open Pass JSON file, check path/name again!");
                json passNode = json::parse(passFileStream);

                std::string passFileName  = passNode["name"];
                auto        splitPassName = Utilities::SplitString(passFileName, ".");
                auto&       passName      = splitPassName[splitPassName.size() - 1];

                // Create a pass node UI
                RZEPassNodeUI* passNodeUI = new RZEPassNodeUI(passName, m_NodeGraphWidget->getScene());
                passNodeUI->setPos(nodePos.x, nodePos.y);

                std::string shaderName = passNode["shader"];
                passNodeUI->setShaderName(shaderName);

                auto& pipelineSettings               = passNodeUI->getPipelineSettings();
                pipelineSettings.cullMode            = passNode["pipeline_info"]["cull_mode"];
                pipelineSettings.polygonMode         = passNode["pipeline_info"]["polygon_mode"];
                pipelineSettings.drawMode            = passNode["pipeline_info"]["draw_type"];
                pipelineSettings.enableTransparencey = passNode["pipeline_info"]["transparency"].get<bool>();

                auto color_blend          = passNode["pipeline_info"]["color_blend"];
                pipelineSettings.colorSrc = color_blend["src"];
                pipelineSettings.colorDst = color_blend["dst"];
                pipelineSettings.colorOp  = color_blend["op"];

                auto alpha_blend          = passNode["pipeline_info"]["alpha_blend"];
                pipelineSettings.alphaSrc = alpha_blend["src"];
                pipelineSettings.alphaDst = alpha_blend["dst"];
                pipelineSettings.alphaOp  = alpha_blend["op"];

                auto depth                        = passNode["pipeline_info"]["depth"];
                pipelineSettings.enableDepthWrite = depth["write"].get<bool>();
                pipelineSettings.enableDepthTest  = depth["test"].get<bool>();
                pipelineSettings.depthOperation   = depth["op"];

                auto depthFormat = passNode["pipeline_info"]["depth_format"];
                if (!depthFormat.empty())
                    pipelineSettings.depthFormat = depthFormat;

                auto color_formats = passNode["pipeline_info"]["color_formats"];
                for (u32 i = 0; i < color_formats.size(); i++)
                    pipelineSettings.colorFormats.push_back(color_formats[i]);

                auto& sceneSettings        = passNodeUI->getSceneSettings();
                sceneSettings.geometryMode = passNode["scene_params"]["geometry_mode"];

                sceneSettings.resolution = passNode["rendering_info"]["resolution"];
                auto extents             = passNode["rendering_info"]["extents"];
                sceneSettings.extents.x  = extents["x"].get<int>();
                sceneSettings.extents.y  = extents["y"].get<int>();

                sceneSettings.enableResize = passNode["rendering_info"]["resize"].get<bool>();
                sceneSettings.layers       = passNode["rendering_info"]["layers"].get<int>();

                auto isStandAlone = passNode["is_standalone"];
                passNodeUI->setIsStandAlonePass(isStandAlone.get<bool>());

                // Created Edges and Resources/Import nodes and connect them and fill their infos
                json inputs  = pass["inputs"];
                json outputs = pass["outputs"];

                nodePos += glm::vec2(-300, -125);

                for (auto& input: inputs) {
                    auto inputName = input["name"];

                    auto isImported = input["is_imported"];
                    if (!isImported.empty()) {
                        // Create a import node instead
                        RZEImportNodeUI* importNodeUI = new RZEImportNodeUI(inputName, m_NodeGraphWidget->getScene());
                        importNodeUI->setPos(nodePos.x, nodePos.y);

                        // Connect to this PassNode via an Edge
                        NodeEdge* edge = new NodeEdge(m_NodeGraphWidget->getScene(), importNodeUI->getOutputSocket(0), passNodeUI->getInputSocket(0));
                    } else {
                        /**
                         * Since these are references, they are pre-existing nodes in the Scene, we can get them by their name and connect the to this node via an edge
                         */
                        auto node = m_NodeGraphWidget->getScene()->getNodeByName(inputName);
                        // Connect to this ResourceNode via an Edge
                        NodeEdge* edge = new NodeEdge(m_NodeGraphWidget->getScene(), node->getOutputSocket(0), passNodeUI->getInputSocket(0));
                    }

                    nodePos.y += 75;
                }

                nodePos += glm::vec2(600, 0);
                nodePos.y = m_NodeGraphWidget->getOrigin().y() - 125;
                for (auto& output: outputs) {
                    auto outputName = output["name"];

                    // Create the Buffer/Texture Resource node
                    auto type = output["type"];

                    if (type == "Buffer") {
                        // Create a Buffer resource node
                        RZEBufferResourceNodeUI* bufferNodeUI = new RZEBufferResourceNodeUI(outputName, m_NodeGraphWidget->getScene());
                        bufferNodeUI->setPos(nodePos.x, nodePos.y);

                        bufferNodeUI->setSize(output["size"].get<int>());
                        bufferNodeUI->setUsage(output["usage"]);

                        // Connect to this ResourceNode via an Edge
                        NodeEdge* edge = new NodeEdge(m_NodeGraphWidget->getScene(), bufferNodeUI->getInputSocket(0), passNodeUI->getOutputSocket(0));

                    } else if (type == "Texture") {
                        // Create a Texture resource node
                        RZETextureResourceNodeUI* textureNodeUI = new RZETextureResourceNodeUI(outputName, m_NodeGraphWidget->getScene());
                        textureNodeUI->setPos(nodePos.x, nodePos.y);

                        textureNodeUI->m_Width        = output["width"].get<int>();
                        textureNodeUI->m_Height       = output["height"].get<int>();
                        textureNodeUI->m_Depth        = output["depth"].get<int>();
                        textureNodeUI->m_Layers       = output["layers"].get<int>();
                        textureNodeUI->m_Type         = output["texture_type"];
                        textureNodeUI->m_Format       = output["format"];
                        textureNodeUI->m_Wrapping     = output["wrapping"];
                        textureNodeUI->m_FilteringMin = output["filtering"]["min"];
                        textureNodeUI->m_FilteringMag = output["filtering"]["mag"];
                        textureNodeUI->m_EnableMips   = output["enable_mips"].get<bool>();
                        textureNodeUI->m_IsHDR        = output["hdr"].get<bool>();

                        // Connect to this ResourceNode via an Edge
                        NodeEdge* edge = new NodeEdge(m_NodeGraphWidget->getScene(), textureNodeUI->getInputSocket(0), passNodeUI->getOutputSocket(0));
                    }
                    nodePos.y += 75;
                }
                nodePos += glm::vec2(600, 0);
            }
        }
    }    // namespace Editor
}    // namespace Razix