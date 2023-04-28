#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QSurfaceFormat>
#include <QTimer>
#include <QWindow>

QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
QT_END_NAMESPACE

namespace Razix {
    namespace Editor {
        class RZEFrameGraphEditor : public QOpenGLWindow, private QOpenGLExtraFunctions
        {
            Q_OBJECT
        public:
            RZEFrameGraphEditor();
            ~RZEFrameGraphEditor() {}

        protected:
            void initializeGL() override;

            void paintGL() override;

        private:
        };
    }    // namespace Editor
}    // namespace Razix
