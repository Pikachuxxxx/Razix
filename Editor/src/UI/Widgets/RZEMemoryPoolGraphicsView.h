#pragma once

#include <QGraphicsItem>

namespace Razix {
    namespace Editor {
        class RZEMemoryPoolGraphicsView : public QGraphicsItem
        {
            // Note: No need to have Q_OBJECT because this is not a widget or a derivate of QObject
        public:
            RZEMemoryPoolGraphicsView(u32 totalSize, u32 capacity, u32 elementSize, const std::string &typeName);
            ~RZEMemoryPoolGraphicsView();

            void updatePoolView();

            QRectF boundingRect() const override { return QRectF(0, 0, m_Width, m_Height); }
            void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

            inline void setTotalSize(u32 size) { m_TotalMemorySize = size; }
            inline void setCapacity(u32 size) { m_TotalMemorySize = size; }
            inline void setOccupiedElements(u32 count)
            {
                m_OccupiedElements = count;
                updatePoolView();
            }

        private:
            //-----------------------
            std::string m_PoolTypeName              = "NONE"; /* Type of the Pool (name)                                                        */
            u32         m_ElementSize               = 0;      /* Size of each element in the Pool                                               */
            u32         m_MemoryOccupanySliderValue = 0;      /* Slider Value for current no. of occupied elements (same as m_OccupiedElements) */
            u32         m_TotalMemorySize           = 0;      /* Total memory size of the Pool in bytes                                         */
            u32         m_OccupiedMemory            = 0;      /* Total memory occupied the elements                                             */
            u32         m_OccupiedGPUMemory         = 0;      /* Total GPU memory occupied by the pool elements                                 */
            u32         m_Capacity                  = 0;      /* Total capacity of the pool                                                     */
            u32         m_OccupiedElements          = 0;      /* Current no. of occupied elements                                               */
            //-----------------------
            u32 m_Width  = 500;
            u32 m_Height = 75;

        private:
            void paintBorderRectWithText(QPainter *painter, QString string, QColor bgColor, QColor borderColor, float x, float y);
        };
    }    // namespace Editor
}    // namespace Razix
