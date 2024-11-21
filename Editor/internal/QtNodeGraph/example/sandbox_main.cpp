#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QPainter>
#include<QGraphicsTextItem>

#include <vector>

#include <QtNodeGraph.h>


class MemoryGraphicsScene : public QGraphicsScene
{
public:
    MemoryGraphicsScene()
    {
        m_BGColor = QColor("#393939");
        m_LightColor = QColor("#1f1f1f");

        m_LightPen.setColor(m_LightColor);
        m_LightPen.setWidth(1.0f);

        // setSceneRect(0, 0, 100, 100);
        setBackgroundBrush(QBrush(QColor("#292929")));
    }
    ~MemoryGraphicsScene() {}

    void drawBackground(QPainter *painter, const QRectF &rect) override
    {
        QGraphicsScene::drawBackground(painter, rect);

        std::vector<QLine> lines_light;
        std::vector<QLine> lines_dark;

        int32_t top = int32_t(floor(rect.top()));
        int32_t bottom = int32_t(ceil(rect.bottom()));
        int32_t left = int32_t(floor(rect.left()));
        int32_t right = int32_t(ceil(rect.right()));

        int32_t first_left = left - (left % m_GridSize);
        int32_t first_top = top - (top % m_GridSize);

        for (int32_t x = first_left; x < right; x += m_GridSize){
            if(x % (m_GridSize * m_GridSquares) != 0) lines_light.push_back(QLine(x, top, x, bottom));
            else lines_dark.push_back(QLine(x, top, x, bottom));
        }

        for (int32_t y = first_top; y < bottom; y += m_GridSize){
            if(y % (m_GridSize * m_GridSquares) != 0) lines_light.push_back(QLine(left, y, right, y));
            else lines_dark.push_back(QLine(left, y, right, y));
        }

        painter->setPen(m_LightPen);
        painter->drawLines(lines_light.data(), lines_light.size());
    }
private:
    QColor m_BGColor;
    QColor m_LightColor;

    QPen m_LightPen;

    uint32_t m_GridSize = 25;
    uint32_t m_GridSquares = 25;

};


class NodeEditor : public NodeGraphWidget
{
public:
    NodeEditor()
        : NodeGraphWidget()
    {}
    ~NodeEditor(){}
};

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Node Editor App");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    //--------------------------------
    // Node Editor Main WIdget
    NodeEditor* ne = new NodeEditor;
    layout->addWidget(ne);
    //--------------------------------

#if 0
    //--------------------------------
    // Testing Memory View kind of widget
    QGraphicsView* view = new QGraphicsView;
    MemoryGraphicsScene* scene = new MemoryGraphicsScene;
    view->setScene(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Add a rctange to the scene at the center
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, 50));
    // Green Gradient
    // linearGrad.setColorAt(0, QColor("#c9de96"));
    // linearGrad.setColorAt(0.44, QColor("#8ab66b"));
    // linearGrad.setColorAt(0.8, QColor("#398235"));
    // Orange Gradient
    linearGrad.setColorAt(0.2, QColor("#C79712"));
    linearGrad.setColorAt(0.98, QColor("#D14707"));
    // XXXXX linearGrad.setColorAt(0.44, QColor("#FF6200"));

    scene->addRect(0, 0, 250, 50, QPen(QColor("#121212")), QBrush(linearGrad));
    auto txt = scene->addText("Capacity : 48 Mib");
    // txt->setPos(125, 25);
    txt->setDefaultTextColor(QColor("#000000"));

    QGraphicsRectItem* memoryBar = scene->addRect(0, 0, 5, 60, QPen(QColor("#2f2f2f")), QBrush(QColor("#ff11ff")));
    memoryBar->setPos(100, -5);
    QPolygonF Triangle;
    Triangle.append(QPointF(10.,0));
    Triangle.append(QPointF(0.,-10));
    Triangle.append(QPointF(-10.,0));
    Triangle.append(QPointF(10.,0));
    Triangle.translate(2.5, 65);
    QGraphicsPolygonItem* pTriangleItem = new QGraphicsPolygonItem(Triangle, memoryBar);
    pTriangleItem->setBrush(QColor("#ff11ff"));
    pTriangleItem->setPen(QColor("#2f2f2f"));
    pTriangleItem->setZValue(2.0f);

    layout->addWidget(view);
    //--------------------------------
#endif

    QMainWindow w;
    w.resize(1280, 720);
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    w.setCentralWidget(widget);
    w.setWindowTitle("Node Editor");
    w.show();

    return a.exec();
}
