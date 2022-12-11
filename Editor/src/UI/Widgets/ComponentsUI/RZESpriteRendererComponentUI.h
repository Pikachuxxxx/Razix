#pragma once

#include <QWidget>

#include "generated/ui_RZESpriteRendererComponentUI.h"

namespace Razix {
    namespace Editor {
        class RZESpriteRendererComponentUI : public QWidget
        {
            Q_OBJECT

        public:
            RZESpriteRendererComponentUI(QWidget *parent = nullptr);
            ~RZESpriteRendererComponentUI();

        public slots:
            void on_sprite_select_pressed();

        private:
            Ui::SpriteRendererComponent ui;
            std::string                 m_SpriteAbsolutePath;
        };

    }    // namespace Editor
}    // namespace Razix