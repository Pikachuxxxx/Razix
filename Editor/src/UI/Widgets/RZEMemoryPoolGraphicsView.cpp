// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEMemoryPoolGraphicsView.h"

#include <QPainter>
#include <QPen>

namespace Razix {
    namespace Editor {

#define Gib_Convert(x) x / (1024 * 1024 * 1024)
#define Mib_Convert(x) x / (1024 * 1024)
#define Kib_Convert(x) x / (1024)

        static std::string MemBytesToString(u32 memoryInBytes)
        {
            if (memoryInBytes / (1024 * 1024 * 1024) > 1)
                return std::to_string(Gib_Convert(memoryInBytes)) + " Gib";
            else if (memoryInBytes / (1024 * 1024) > 1)
                return std::to_string(Mib_Convert(memoryInBytes)) + " Mib";
            if (memoryInBytes / (1024) > 1)
                return std::to_string(Kib_Convert(memoryInBytes)) + " Kib";
            else
                return std::to_string(memoryInBytes) + " bytes";
        }

        RZEMemoryPoolGraphicsView::RZEMemoryPoolGraphicsView(u32 totalSize, u32 capacity, u32 elementSize, const std::string &typeName)
            : m_PoolTypeName(typeName), m_ElementSize(elementSize), m_TotalMemorySize(totalSize), m_Capacity(capacity)
        {
        }

        RZEMemoryPoolGraphicsView::~RZEMemoryPoolGraphicsView()
        {
        }

        void RZEMemoryPoolGraphicsView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr*/)
        {
            //--------------------------------------------------------------------------------------------------------------
            // Memory Block - CPU
            // Total Size of the Pool Memory Block
            auto poolSizeStr = "CPU : " + QString(MemBytesToString(m_TotalMemorySize).c_str());

            auto BlockPath = QPainterPath();
            BlockPath.addRect(25, 0, m_Width - 50, m_Height - 25);

            painter->setPen(QPen(QColor("#121212")));

            QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 50));
            linearGrad.setColorAt(0, QColor("#C9DE96"));
            linearGrad.setColorAt(0.44, QColor("#8AB66B"));
            linearGrad.setColorAt(0.8, QColor("#398235"));
            painter->setBrush(linearGrad);

            painter->drawPath(BlockPath.simplified());

            auto SizeTextPath = QPainterPath();

            QFont poolMemoryFont;
            poolMemoryFont.setWeight(QFont::ExtraLight);
            poolMemoryFont.setStyleHint(QFont::Courier, QFont::PreferAntialias);
            poolMemoryFont.setPointSize(20);
            //poolMemoryFont.setBold(true);
            QFontMetrics fm(poolMemoryFont);
            painter->setPen(QPen(QColor("#000000")));
            painter->setBrush(QBrush(QColor("#000000")));
            SizeTextPath.addText(m_Width / 2 - fm.width(poolSizeStr) / 2, fm.height(), poolMemoryFont, poolSizeStr);

            painter->drawPath(SizeTextPath.simplified());
            //--------------------------------------------------------------------------------------------------------------
            // Memory Block - GPU
            auto GPUpoolSizeStr = "GPU : " + QString(MemBytesToString(m_OccupiedGPUMemory).c_str());

            // Draw it only until the Slider with some text in it
            auto GPUBlockPath = QPainterPath();
            GPUBlockPath.addRect(25, m_Height - 20, m_MemoryOccupanySliderValue, (m_Height - 25) / 2);

            painter->setPen(QPen(QColor("#121212")));

            // TODO: FIXME: Fix the gradient coloring later
            QLinearGradient linearGradOrange(QPointF(0, 0), QPointF(0, (m_Height - 25) / 2));
            linearGradOrange.setColorAt(0.7, QColor("#ffe9ba"));
            linearGradOrange.setColorAt(0.4, QColor("#ffc438"));
            linearGradOrange.setColorAt(0.8, QColor("#e49c37"));
            painter->setBrush(linearGradOrange);

            painter->drawPath(GPUBlockPath.simplified());

            auto GPUSizeTextPath = QPainterPath();

            poolMemoryFont.setPointSize(10);
            painter->setPen(QPen(QColor("#000000")));
            painter->setBrush(QBrush(QColor("#000000")));
            GPUSizeTextPath.addText(30, m_Height, poolMemoryFont, GPUpoolSizeStr);

            painter->drawPath(GPUSizeTextPath.simplified());

            //--------------------------------------------------------------------------------------------------------------
            // Type
            paintBorderRectWithText(painter, QString("Type : ") + QString(m_PoolTypeName.c_str()), QColor("#504b35"), QColor("#ddc988"), 5, -25);
            //--------------------------------------------------------------------------------------------------------------
            // Used slots
            paintBorderRectWithText(painter, QString("Occupancy : ") + QString(std::to_string(m_OccupiedElements).c_str()), QColor("#8d1439"), QColor("#e75380"), m_MemoryOccupanySliderValue, 110);
            //--------------------------------------------------------------------------------------------------------------
            // Occupancy Slider
            auto OccupancyBarPath = new QPainterPath;
            OccupancyBarPath->addRect(m_MemoryOccupanySliderValue + 25, 0, 5, 75);
            painter->setBrush(QColor("#FF11FF"));
            painter->setPen(QColor("#FF11FF"));
            QPolygonF Triangle;
            Triangle.append(QPointF(10., 0));
            Triangle.append(QPointF(0., -10));
            Triangle.append(QPointF(-10., 0));
            Triangle.append(QPointF(10., 0));
            Triangle.translate(m_MemoryOccupanySliderValue + 2.5 + 25, 82);
            OccupancyBarPath->addPolygon(Triangle);

            painter->drawPath(OccupancyBarPath->simplified());
            //--------------------------------------------------------------------------------------------------------------
            // Occupancy Slider Memory
            auto SliderMemoryPath = new QPainterPath;
            painter->setPen(Qt::white);
            painter->setBrush(QColor("#FFFFFF"));

            QFont occupancyMemoryFont;
            occupancyMemoryFont.setWeight(QFont::ExtraLight);
            occupancyMemoryFont.setStyleHint(QFont::Courier, QFont::PreferAntialias);
            std::string CPU_occupancyStats = "[CPU] Used : " + MemBytesToString(m_OccupiedMemory) + "/" + MemBytesToString(m_TotalMemorySize) + " | Free : " + MemBytesToString(m_TotalMemorySize - m_OccupiedMemory);
            SliderMemoryPath->addText(m_MemoryOccupanySliderValue + 50, 65, occupancyMemoryFont, CPU_occupancyStats.c_str());
            std::string GPU_occupancyStats = "[GPU] Used : " + MemBytesToString(m_OccupiedGPUMemory);
            SliderMemoryPath->addText(m_MemoryOccupanySliderValue + 50, 75, occupancyMemoryFont, GPU_occupancyStats.c_str());

            painter->drawPath(SliderMemoryPath->simplified());
            //--------------------------------------------------------------------------------------------------------------
            // Capacity
            paintBorderRectWithText(painter, QString("Capacity : ") + QString(std::to_string(m_Capacity).c_str()), QColor("#673800"), QColor("#ff8c01"), m_Width, 25);
        }

        void RZEMemoryPoolGraphicsView::paintBorderRectWithText(QPainter *painter, QString string, QColor bgColor, QColor borderColor, float x, float y)
        {
            QFont typeFont;
            typeFont.setWeight(QFont::Light);
            typeFont.setStyleHint(QFont::Courier, QFont::PreferAntialias);
            typeFont.setPointSize(10);

            // Text BG Box
            auto TypeTextBoxPath = new QPainterPath();
            QPen rr(borderColor);
            rr.setWidth(2);
            painter->setPen(rr);
            painter->setBrush(QBrush(bgColor));
            QFontMetrics fm(typeFont);
            painter->drawRoundedRect(QRect(x, -fm.height() + y, fm.width(string) + 10, fm.height() + 10), 5, 5);

            painter->drawPath(TypeTextBoxPath->simplified());

            // Text for Type
            auto TypeTextPath = new QPainterPath();
            painter->setPen(Qt::white);
            painter->setBrush(QColor("#FFFFFF"));
            TypeTextPath->addText(x + 5, y, typeFont, string);

            painter->drawPath(TypeTextPath->simplified());
        }

        void RZEMemoryPoolGraphicsView::updatePoolView()
        {
            // Based on the Total Size and Total memory divide and set the slider
            m_OccupiedMemory = m_ElementSize * m_OccupiedElements;

            // Update the slider position
            // Slide will span from +25 to m_Width - 25
            float deltaSlider           = float((m_Width - 50)) / (float) m_Capacity;
            m_MemoryOccupanySliderValue = deltaSlider * m_OccupiedElements;
        }
    }    // namespace Editor
}    // namespace Razix
