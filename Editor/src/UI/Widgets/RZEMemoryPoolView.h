#pragma once

#include <QGraphicsItem>

namespace Razix {
    namespace Editor {
        class RZEMemoryPoolView : public QGraphicsItem
        {
            // Note: No need to have Q_OBJECT because this is not a widget or a derivate of QObject
        public:
            RZEMemoryPoolView();
            ~RZEMemoryPoolView();

            QRectF boundingRect() const override { return QRectF(0, 0, m_Width, m_Height); }
            void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

        private:
            u32 m_Width       = 500;
            u32 m_Height      = 75;
            u32 m_SliderValue = 100;

        private:
            void paintBorderRectWithText(QPainter *painter, QString string, QColor bgColor, QColor borderColor, float x, float y);
        };
    }    // namespace Editor
}    // namespace Razix
