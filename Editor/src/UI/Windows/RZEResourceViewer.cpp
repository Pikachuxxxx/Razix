// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEResourceViewer.h"

#include "UI/Widgets/RZECollapsingHeader.h"

#include "ui/Widgets/RZEMemoryPoolGraphicsView.h"

namespace Razix {
    namespace Editor {

        MemoryPoolView::MemoryPoolView(u32 totalSize, u32 capacity, u32 elementSize, const std::string &typeName)
        {
            m_GraphicsView    = new QGraphicsView;
            m_MemoryPoolScene = new MemoryGraphicsScene;
            m_GraphicsView->setScene(m_MemoryPoolScene);
            m_GraphicsView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
            m_GraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
            //m_GraphicsView->setMaximumHeight(400);
            m_GraphicsView->setMinimumHeight(200);

            // Add a Item for Pool_0 to the scene
            m_View = new RZEMemoryPoolGraphicsView(totalSize, capacity, elementSize, typeName);
            m_MemoryPoolScene->addItem(m_View);
        }

        RZEResourceViewer::RZEResourceViewer(QWidget *parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

            setObjectName("Resource Viewer");

            addPools();
             
            ui.ResourcesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            initTable();
        }

        RZEResourceViewer::~RZEResourceViewer()
        {
        }

        void RZEResourceViewer::updateView()
        {
        }

        void RZEResourceViewer::addPools()
        {
            auto pool_0 = new MemoryPoolView(448 * 1024 * 1024, 128, 48, "Texture2D");
            pool_0->m_View->setOccupiedElements(64);
            auto pool_1 = new MemoryPoolView(128 * 1024 * 1024, 64, 32, "VertexBuffer");
            pool_1->m_View->setOccupiedElements(16);

            auto testCollapsingHeaderPool_0 = new RZECollapsingHeader(QString("Pool_0 - RZTexture2D"), pool_0->m_GraphicsView, new QIcon(":/rzeditor/memory_pool_icon.png"), false);
            testCollapsingHeaderPool_0->toggleCollapse();
            ui.PoolGroupVLayout->insertWidget(0, testCollapsingHeaderPool_0);

            auto testCollapsingHeaderPool_1 = new RZECollapsingHeader(QString("Pool_1 - RZVertexBuffer"), pool_1->m_GraphicsView, new QIcon(":/rzeditor/memory_pool_icon.png"), false);
            ui.PoolGroupVLayout->insertWidget(1, testCollapsingHeaderPool_1);
        }

        void RZEResourceViewer::initTable()
        {
            addRow("550e8400-e29b-41d4-a716-446655440000", "Pool_0 - RZTexture2D", 0, "Default Texture", "VKTexture2D", 24, 128, "//RazixContent/Textures/Default_pink_texture.png", 1, 28);
            addRow("550e8400-e29b-41d4-a716-446655440000", "Pool_0 - RZTexture2D", 0, "Default Texture", "VKTexture2D", 24, 128, "//RazixContent/Textures/Default_pink_texture.png", 1, 28);
            addRow("550e8400-e29b-41d4-a716-446655440000", "Pool_0 - RZTexture2D", 0, "Default Texture", "VKTexture2D", 24, 128, "//RazixContent/Textures/Default_pink_texture.png", 1, 28);
            addRow("550e8400-e29b-41d4-a716-446655440000", "Pool_0 - RZTexture2D", 0, "Default Texture", "VKTexture2D", 24, 128, "//RazixContent/Textures/Default_pink_texture.png", 1, 28);
            addRow("550e8400-e29b-41d4-a716-446655440000", "Pool_0 - RZTexture2D", 0, "Default Texture", "VKTexture2D", 24, 128, "//RazixContent/Textures/Default_pink_texture.png", 1, 28);
            addRow("550e8400-e29b-41d4-a716-446655440000", "Pool_0 - RZTexture2D", 0, "Default Texture", "VKTexture2D", 24, 128, "//RazixContent/Textures/Default_pink_texture.png", 1, 28);
        }

        void RZEResourceViewer::addRow(const std::string &uuid, const std::string &poolName, u32 index, const std::string &name, const std::string &typeName, u32 cpuMem, u32 gpuMem, const std::string &location, u32 status, u32 refs)
        {
            auto row = ui.ResourcesTable->rowCount();
            ui.ResourcesTable->insertRow(ui.ResourcesTable->rowCount());
            ui.ResourcesTable->setItem(row, ResourceTableColumns::UUID, new QTableWidgetItem(uuid.c_str()));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::Pool, new QTableWidgetItem(poolName.c_str()));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::ID, new QTableWidgetItem(std::to_string(index).c_str()));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::Name, new QTableWidgetItem(name.c_str()));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::Type, new QTableWidgetItem(typeName.c_str()));

            auto cpuMemSizeStr = std::to_string(cpuMem) + " Mib";
            auto cpuMemItem    = new QTableWidgetItem(cpuMemSizeStr.c_str());
            cpuMemItem->setTextAlignment(Qt::AlignCenter);
            cpuMemItem->setForeground(QBrush(QColor(0, 255, 0)));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::CPUMemory, cpuMemItem);

            auto gpuMemSizeStr = std::to_string(gpuMem) + " Gib";
            auto gpuMemItem    = new QTableWidgetItem(gpuMemSizeStr.c_str());
            gpuMemItem->setTextAlignment(Qt::AlignCenter);
            gpuMemItem->setForeground(QBrush(QColor("#FFA500")));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::GPUMemory, gpuMemItem);

            auto assetLocItem = new QTableWidgetItem(location.c_str());
            assetLocItem->setForeground(QBrush(QColor("#ffffe0")));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::AssetLocation, assetLocItem);

            ui.ResourcesTable->setItem(row, ResourceTableColumns::Status, new QTableWidgetItem(std::to_string(status).c_str()));
            ui.ResourcesTable->setItem(row, ResourceTableColumns::References, new QTableWidgetItem(std::to_string(refs).c_str()));
        }
    }    // namespace Editor
}    // namespace Razix
