// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEEngineLoop.h"

namespace Razix {
    namespace Editor {
        RZEEngineLoop::RZEEngineLoop(int argc, char** argv, QObject* parent)
            : QObject(parent)
        {
            this->argc = argc;
            this->argv = argv;
        }

        RZEEngineLoop::~RZEEngineLoop()
        {
        }
    }    // namespace Editor
}    // namespace Razix
