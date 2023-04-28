// clang-format off
#include "rzepch.h"
// clang-format on

#include "QtImGui.h"

#include "QtImGuiRenderer.h"
#include <QWindow>
#ifdef QT_WIDGETS_LIB
#include <QWidget>
#endif

namespace QtImGui {

class QWindowWrapper : public WindowWrapper
{
public:
  QWindowWrapper(QtImGuiRenderer* r) 
    : r(r)
  {}
  ~QWindowWrapper() {
    if (r && (r != QtImGuiRenderer::instance())) {
      delete r;
    }
  }

public:
  void newFrame() { r->newFrame(); }

  void render() { r->render(); }
private:
  QtImGuiRenderer* r;
};

#ifdef QT_WIDGETS_LIB

namespace {

class QWidgetWindowWrapper : public QWindowWrapper {
public:
    QWidgetWindowWrapper(QWidget *w, QtImGuiRenderer* r) 
      : QWindowWrapper(r), w(w)
    {}
    void installEventFilter(QObject *object) override {
        return w->installEventFilter(object);
    }
    QSize size() const override {
        return w->size();
    }
    qreal devicePixelRatio() const override {
        return w->devicePixelRatioF();
    }
    bool isActive() const override {
        return w->isActiveWindow();
    }
    QPoint mapFromGlobal(const QPoint &p) const override {
        return w->mapFromGlobal(p);
    }
    QObject* object() override {
        return w;
    }
    
    void setCursorShape(Qt::CursorShape shape) override
    {
        #ifndef QT_NO_CURSOR
            w->setCursor(shape);
        #else
            Q_UNUSED(shape);
        #endif
    }
    
    void setCursorPos(const QPoint& local_pos) override
    {
        #ifndef QT_NO_CURSOR
            // Convert position from widget-space into screen-space
            const QPoint global_pos = w->mapToGlobal(local_pos);

            QCursor cursor = w->cursor();
            cursor.setPos(global_pos);
            w->setCursor(cursor);
        #else
            Q_UNUSED(local_pos);
        #endif
    }
    
private:
    QWidget *w;
};
  
} // namespace

RenderRef initialize(QWidget *window, bool defaultRender) {
  if (defaultRender) {
    auto* wrapper = new QWidgetWindowWrapper(window, QtImGuiRenderer::instance());
    QtImGuiRenderer::instance()->initialize(wrapper);
    return reinterpret_cast<RenderRef>(dynamic_cast<QWindowWrapper*>(wrapper));
  } else {
    auto* render = new QtImGuiRenderer();
    auto* wrapper = new QWidgetWindowWrapper(window, render);
    render->initialize(wrapper);
    return reinterpret_cast<RenderRef>(dynamic_cast<QWindowWrapper*>(wrapper));
  }
}

#endif // QT_WIDGETS_LIB

namespace {

class QWindowWindowWrapper : public QWindowWrapper {
public:
    QWindowWindowWrapper(QWindow *w, QtImGuiRenderer* r) 
      : QWindowWrapper(r), w(w)
    {}
    void installEventFilter(QObject *object) override {
        return w->installEventFilter(object);
    }
    QSize size() const override {
        return w->size();
    }
    qreal devicePixelRatio() const override {
        return w->devicePixelRatio();
    }
    bool isActive() const override {
        return w->isActive();
    }
    QPoint mapFromGlobal(const QPoint &p) const override {
        return w->mapFromGlobal(p);
    }
    QObject* object() override {
        return w;
    }
    
    void setCursorShape(Qt::CursorShape shape) override
    {
        #ifndef QT_NO_CURSOR
            w->setCursor(shape);
        #else
            Q_UNUSED(shape);
        #endif
    }
    
    void setCursorPos(const QPoint& local_pos) override
    {
        #ifndef QT_NO_CURSOR
            // Convert position from window-space into screen-space
            const QPoint global_pos = w->mapToGlobal(local_pos);

            QCursor cursor = w->cursor();
            cursor.setPos(global_pos);
            w->setCursor(cursor);
        #else
            Q_UNUSED(local_pos);
        #endif
    }

private:
    QWindow *w;
};

} // namespace

RenderRef initialize(QWindow* window, bool defaultRender) {
  if (defaultRender) {
    auto* wrapper = new QWindowWindowWrapper(window, QtImGuiRenderer::instance());
    QtImGuiRenderer::instance()->initialize(wrapper);
    return reinterpret_cast<RenderRef>(dynamic_cast<QWindowWrapper*>(wrapper));
  }
  else {
    auto* render = new QtImGuiRenderer();
    auto* wrapper = new QWindowWindowWrapper(window, render);
    render->initialize(wrapper);
    return reinterpret_cast<RenderRef>(dynamic_cast<QWindowWrapper*>(wrapper));
  }
}

void newFrame(RenderRef ref) {
  if (!ref) {
    QtImGuiRenderer::instance()->newFrame();
  } else {
    auto wrapper = reinterpret_cast<QWindowWrapper*>(ref);
    wrapper->newFrame();
  }
}

void render(RenderRef ref)
{
  if (!ref) {
    QtImGuiRenderer::instance()->render();
  } else {
    auto wrapper = reinterpret_cast<QWindowWrapper*>(ref);
    wrapper->render();
  }
}

} // namespace QtImGui
