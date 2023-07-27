#pragma once

#include "generated/ui_RZEResourceViewer.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>

#include "UI/Widgets/RZEMemoryPoolGraphicsView.h"

namespace Razix {
    namespace Editor {

        class MemoryGraphicsScene : public QGraphicsScene
        {
        public:
            MemoryGraphicsScene()
            {
                m_BGColor    = QColor("#393939");
                m_LightColor = QColor("#1F1F1F");
                m_LightPen.setColor(m_LightColor);
                m_LightPen.setWidth(1.0f);
                //setSceneRect(0, 0, 200, 400);
                setBackgroundBrush(QBrush(QColor("#292929")));
            }
            ~MemoryGraphicsScene() {}
            void drawBackground(QPainter *painter, const QRectF &rect) override
            {
                QGraphicsScene::drawBackground(painter, rect);
                std::vector<QLine> lines_light;
                std::vector<QLine> lines_dark;
                int32_t            top        = int32_t(floor(rect.top()));
                int32_t            bottom     = int32_t(ceil(rect.bottom()));
                int32_t            left       = int32_t(floor(rect.left()));
                int32_t            right      = int32_t(ceil(rect.right()));
                int32_t            first_left = left - (left % m_GridSize);
                int32_t            first_top  = top - (top % m_GridSize);
                for (int32_t x = first_left; x < right; x += m_GridSize) {
                    if (x % (m_GridSize * m_GridSize) != 0) lines_light.push_back(QLine(x, top, x, bottom));
                    else
                        lines_dark.push_back(QLine(x, top, x, bottom));
                }
                for (int32_t y = first_top; y < bottom; y += m_GridSize) {
                    if (y % (m_GridSize * m_GridSize) != 0) lines_light.push_back(QLine(left, y, right, y));
                    else
                        lines_dark.push_back(QLine(left, y, right, y));
                }
                painter->setPen(m_LightPen);
                painter->drawLines(lines_light.data(), (int) lines_light.size());
            }

        private:
            QColor   m_BGColor;
            QColor   m_LightColor;
            QPen     m_LightPen;
            uint32_t m_GridSize = 25;
        };

        class MemoryPoolView
        {
        public:
            RZEMemoryPoolGraphicsView *m_View;
            QGraphicsView             *m_GraphicsView;
            MemoryGraphicsScene       *m_MemoryPoolScene;

            MemoryPoolView(u32 totalSize, u32 capacity, u32 elementSize, const std::string &typeName);
        };

        enum ResourceTableColumns : u32
        {
            UUID = 0,
            Pool,
            ID,
            Name,
            Type,
            CPUMemory,
            GPUMemory,
            AssetLocation,
            Status,
            References,
            COUNT
        };

        class RZEResourceViewer : public QWidget
        {
            Q_OBJECT

        public:
            RZEResourceViewer(QWidget *parent = nullptr);
            ~RZEResourceViewer();

            void updateView();

        private:
            Ui::ResourceViewer ui;

            void addPools();
            void initTable();
            void addRow(const std::string &uuid, const std::string &poolName, u32 index, const std::string &name, const std::string &typeName, u32 cpuMem, u32 gpuMem, const std::string &location, u32 status, u32 refs);
        };
    }    // namespace Editor
}    // namespace Razix
