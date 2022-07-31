#pragma once

#include "Razix/Core/OS/RZWindow.h"

namespace Razix {
    namespace Editor {

        class RZENativeWindow : public RZWindow
        {
            RZENativeWindow(void* windowHandle, const WindowProperties& properties);
            ~RZENativeWindow() {}

        public:
            void         OnWindowUpdate() override;
            void         Destroy() override;
            unsigned int getWidth() const override;
            unsigned int getHeight() const override;
            void         SetEventCallback(const EventCallbackFn& callback) override;
            void         SetVSync(bool enabled) override;
            bool         IsVSync() const override;
            void         SetWindowIcon() override;
            void*        GetNativeWindow() const override;

            static RZWindow* NativeConstructionFunc(void* windowHandle, const WindowProperties& properties);

            static void Construct();

        private:
            WindowProperties m_Data;
            void*            m_NativeHandle;
            EventCallbackFn  EventCallback;

        };
    }    // namespace Editor
}    // namespace Razix
