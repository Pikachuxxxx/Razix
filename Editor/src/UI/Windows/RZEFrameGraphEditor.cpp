// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEFrameGraphEditor.h"

#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QListWidget>

#include "Nodes/RZEBufferResourceNodeUI.h"
#include "Nodes/RZEImportNodeUI.h"
#include "Nodes/RZEPassNodeUI.h"
#include "Nodes/RZETextureResourceNodeUI.h"

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
    {"Texture", "color:#000000;\n background-color: #9AD0FC;\nborder-color: rgb(0, 0, 0);\nborder-width:1px;\nborder-style:dashed;\nborder-radius:4px;"}};

namespace Razix {
    namespace Editor {

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

            populatePresetImportNodesList();
            populatePresetResourceNodesList();

            // TEST:
            connect(ui.add_ip_pin, SIGNAL(pressed()), this, SLOT(OnAddInputPinClicked()));
            connect(ui.remove_ip_pin, SIGNAL(pressed()), this, SLOT(OnRemoveInputPinClicked()));

            connect(ui.add_op_pin, SIGNAL(pressed()), this, SLOT(OnAddOutputPinClicked()));
            connect(ui.remove_op_pin, SIGNAL(pressed()), this, SLOT(OnRemoveOutputPinClicked()));

            // https://stackoverflow.com/questions/5153157/passing-an-argument-to-a-slot
            m_IpLineEditsSignalMapper = new QSignalMapper(this);
            //m_IpButtonsSignalMapper   = new QSignalMapper(this);

            m_OpLineEditsSignalMapper = new QSignalMapper(this);

            connect(frameGraphGraphicsView, SIGNAL(OnNodeSelected(Node*)), this, SLOT(OnNodeSelected(Node*)));
        }

        void RZEFrameGraphEditor::OnImportPresetButtonClicked()
        {
        }

        void RZEFrameGraphEditor::OnNodeSelected(Node* node)
        {
            if (node) {
                if (dynamic_cast<RZEPassNodeUI*>(node)) {
                    ui.stackedWidget->setCurrentIndex(1);
                    m_CurrentEditingPassNode = dynamic_cast<RZEPassNodeUI*>(node);
                } else if (dynamic_cast<RZEBufferResourceNodeUI*>(node)) {
                    ui.stackedWidget->setCurrentIndex(2);
                    //m_CurrentEditingPassNode = dynamic_cast<RZEPassNodeUI*>(node);
                }
            } else {
                ui.stackedWidget->setCurrentIndex(0);
                m_CurrentEditingPassNode = nullptr;
            }
        }

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

            // Add input names after have successfully given names!
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->addInputSocket(lineEdit->text().toStdString());
            }
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
        }

        void RZEFrameGraphEditor::OnAddOutputPinClicked()
        {
            auto pinNameEdit = new QLineEdit();
            pinNameEdit->setPlaceholderText("Enter output socket name");

            m_OpLineEditsSignalMapper->setMapping(pinNameEdit, ui.op_pins_layout->rowCount());

            connect(pinNameEdit, SIGNAL(returnPressed()), m_OpLineEditsSignalMapper, SLOT(map()), Qt::UniqueConnection);
            connect(m_OpLineEditsSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnOutputPinNameChanged(int)), Qt::UniqueConnection);

            auto label = "output pin #" + std::to_string(ui.op_pins_layout->rowCount());
            ui.op_pins_layout->addRow(label.c_str(), pinNameEdit);
        }

        void RZEFrameGraphEditor::OnOutputPinNameChanged(int idx)
        {
            auto FieldLayoutItem = ui.op_pins_layout->itemAt(idx, QFormLayout::ItemRole::FieldRole);

            QWidget*   widget   = qobject_cast<QWidget*>(FieldLayoutItem->widget());
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);

            std::cout << lineEdit->text().toStdString() << std::endl;

            // Add output names after have successfully given names!
            if (m_CurrentEditingPassNode) {
                m_CurrentEditingPassNode->addOutputSocket(lineEdit->text().toStdString());
            }
        }

        void RZEFrameGraphEditor::OnRemoveOutputPinClicked()
        {
            // Always remove the last pin
            if (ui.op_pins_layout->rowCount() > 1) {
                ui.op_pins_layout->removeRow(ui.op_pins_layout->rowCount() - 1);
                ui.op_pins_layout->update();
            }
        }

        void RZEFrameGraphEditor::populatePresetImportNodesList()
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

        void RZEFrameGraphEditor::populatePresetResourceNodesList()
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
    }    // namespace Editor
}    // namespace Razix