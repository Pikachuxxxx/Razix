// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEFrameGraphEditor.h"

#include <QGraphicsDropShadowEffect>
#include <QListWidget>

#include "Nodes/RZEBufferResourceNodeUI.h"
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
        }

        void RZEFrameGraphEditor::OnImportPresetButtonClicked()
        {
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