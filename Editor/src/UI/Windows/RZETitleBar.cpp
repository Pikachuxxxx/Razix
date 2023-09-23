// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZETitleBar.h"

namespace Razix {
    namespace Editor {
        RZETitleBar::RZETitleBar(QWidget* child, QWidget* parent)
            : QFrame(parent)
        {
            ui.setupUi(this);
            setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
            setAttribute(Qt::WA_TranslucentBackground);
            if (child != nullptr) {
                ui.body->layout()->addWidget(child);
                m_MainBody = child;
                m_MainBody->installEventFilter(this);
                resize(child->size());
            }
        }

        RZETitleBar::~RZETitleBar()
        {
            int subBodies = ui.body->layout()->count();
            if (subBodies > 0) {
                for (int i = 0; i < subBodies; i++) {
                    QWidget* subBody = ui.body->layout()->itemAt(i)->widget();
                    delete subBody;
                }
            }
        }
    }    // namespace Editor
}    // namespace Razix