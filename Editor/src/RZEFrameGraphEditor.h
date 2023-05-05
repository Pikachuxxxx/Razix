#pragma once

#include <QtNodeGraph.h>

namespace Razix {
    namespace Editor {
        class RZEFrameGraphEditor
        {
        public:
            RZEFrameGraphEditor();
            ~RZEFrameGraphEditor() {}

            NodeGraphWidget* getWidget() { return m_NodeGraphWidget; }

        private:
            NodeGraphWidget* m_NodeGraphWidget;
        };
    }    // namespace Editor
}    // namespace Razix
