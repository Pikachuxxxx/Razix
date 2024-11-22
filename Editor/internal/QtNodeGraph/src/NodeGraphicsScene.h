#pragma once

#include <QGraphicsScene>
#include <QPainter>
#include <QUndoStack>
#include <QUndoView>
#include <QKeyEvent>

class NodeGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    NodeGraphicsScene();
    ~NodeGraphicsScene();

    QPoint getOrigin();
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    inline QUndoStack* getUndoStack() { return undoStack; }

    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_Z && event->modifiers() & Qt::ControlModifier)
            undoStack->undo();

        if (event->key() == Qt::Key_U && event->modifiers() & Qt::ControlModifier)
            undoStack->redo();
    }

public slots:
    void OnClipboardChanged();

    //void OnCopy();
    //void OnPaste();
    //void OnCut();

private:
    QColor m_BGColor;
    QColor m_LightColor;
    QColor m_DarkColor;

    uint32_t m_SceneWidth = 10000;
    uint32_t m_SceneHeight = 10000;
    QPen m_LightPen;
    QPen m_DarkPen;

    uint32_t m_GridSize = 20;
    uint32_t m_GridSquares = 5;

    QUndoStack* undoStack = nullptr;
    //QUndoView* undoView = nullptr;
};
