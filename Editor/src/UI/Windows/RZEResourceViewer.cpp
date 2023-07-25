// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEResourceViewer.h"

#include "UI/Widgets/RZECollapsingHeader.h"

#include "ui/Widgets/RZEMemoryPoolView.h"

namespace Razix {
    namespace Editor {

        RZEResourceViewer::RZEResourceViewer(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            setObjectName("Resource Viewer");

            QGraphicsView*       m_PoolView        = new QGraphicsView;
            MemoryGraphicsScene* m_MemoryPoolScene = new MemoryGraphicsScene;
            m_PoolView->setScene(m_MemoryPoolScene);
            m_PoolView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
            m_PoolView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
            m_PoolView->setMaximumHeight(200);
            m_PoolView->setMinimumHeight(200);

            // Add a Item for Pool_0 to the scene
            auto poolItem = new RZEMemoryPoolView;
            m_MemoryPoolScene->addItem(poolItem);

            auto testCollapsingHeaderPool = new RZECollapsingHeader(QString("Pool_0 - Texture2D"), m_PoolView, new QIcon(":/rzeditor/memory_pool_icon.png"));
            testCollapsingHeaderPool->setMaximumHeight(200);
            testCollapsingHeaderPool->setMinimumHeight(200);
            ui.PoolGroupVLayout->addWidget(testCollapsingHeaderPool);
        }

        RZEResourceViewer::~RZEResourceViewer()
        {
        }

    }    // namespace Editor
}    // namespace Razix
