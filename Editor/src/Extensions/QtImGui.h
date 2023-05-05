#pragma once

class QWidget;
class QWindow;

namespace QtImGui {

typedef void* RenderRef;

#ifdef QT_WIDGETS_LIB
RenderRef initialize(QWidget *window, bool defaultRender = true);
#endif

RenderRef initialize(QWindow *window, bool defaultRender = true);
void newFrame(RenderRef ref = nullptr);
void render(RenderRef ref = nullptr);

}
