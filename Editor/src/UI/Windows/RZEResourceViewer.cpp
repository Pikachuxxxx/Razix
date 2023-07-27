// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEResourceViewer.h"

#include "UI/Widgets/RZECollapsingHeader.h"

#include "ui/Widgets/RZEMemoryPoolGraphicsView.h"

namespace Razix {
    namespace Editor {

        RZEResourceViewer::RZEResourceViewer(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            setObjectName("Resource Viewer");

            auto pool_0 = new MemoryPoolView(448 * 1024 * 1024, 128, 48, "Texture2D");
            pool_0->m_View->setOccupiedElements(64);
            auto pool_1 = new MemoryPoolView(128 * 1024 * 1024, 64, 32, "VertexBuffer");
            pool_1->m_View->setOccupiedElements(16);

            auto testCollapsingHeaderPool_0 = new RZECollapsingHeader(QString("Pool_0 - Texture2D"), pool_0->m_GraphicsView, new QIcon(":/rzeditor/memory_pool_icon.png"));
            ui.PoolGroupVLayout->insertWidget(0, testCollapsingHeaderPool_0);

            auto testCollapsingHeaderPool_1 = new RZECollapsingHeader(QString("Pool_1 - VertexBuffer"), pool_1->m_GraphicsView, new QIcon(":/rzeditor/memory_pool_icon.png"));
            ui.PoolGroupVLayout->insertWidget(1, testCollapsingHeaderPool_1);
        }

        RZEResourceViewer::~RZEResourceViewer()
        {
        }

        MemoryPoolView::MemoryPoolView(u32 totalSize, u32 capacity, u32 elementSize, const std::string& typeName)
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
    }    // namespace Editor
}    // namespace Razix
