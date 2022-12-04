#pragma once

#include <QObject>

#include <Razix/Core/RZApplication.h>
#include <Razix/Core/EntryPoint.h>

namespace Razix {
    namespace Editor {
        class RZEEngineLoop : public QObject
        {
            Q_OBJECT

        public:
            RZEEngineLoop(QObject *parent = nullptr);
            ~RZEEngineLoop();

        public slots:

            void launch()
            {
                tickNext();
            }

        private slots:

            void tick()
            {
                // Continue ticking
                tickNext();

                // Do the work Here
                while (Razix::RZApplication::Get().RenderFrame()) {}

                Razix::RZApplication::Get().Quit();
                Razix::RZApplication::Get().SaveApp();

                EngineExit();
            }

        private:
            void tickNext()
            {
                // Trigger the tick() invokation when the event loop runs next time
                QMetaObject::invokeMethod(this, "tick", Qt::QueuedConnection);
            }
        };
    }    // namespace Editor
}    // namespace Razix
