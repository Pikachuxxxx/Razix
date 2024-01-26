#pragma once

#include <QObject>

#include <Razix.h>
#include <Razix/Core/EntryPoint.h>
#include <Razix/Core/RZApplication.h>

namespace Razix {
    namespace Editor {
        class RZEEngineLoop : public QObject
        {
            Q_OBJECT

        public:
            RZEEngineLoop(int argc, char** argv, QObject* parent = nullptr);
            ~RZEEngineLoop();

            //public slots:
            //
            //    void launch()
            //    {
            //        EngineMain(argc, argv);
            //
            //        tickNext();
            //    }
            //
            //public slots:
            //
            //    void tick()
            //    {
            //        // Continue ticking
            //        tickNext();
            //
            //        // Do the work Here
            //        Razix::RZApplication::Get().RenderFrame();
            //    }
            //
            //private:
            //    void tickNext()
            //    {
            //        // Trigger the tick() invokation when the event loop runs next time
            //        QMetaObject::invokeMethod(this, "tick", Qt::QueuedConnection);
            //    }
            //
        private:
            int    argc;
            char** argv;
        };
    }    // namespace Editor
}    // namespace Razix