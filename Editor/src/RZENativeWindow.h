
#pragma once

#include "Razix/Core/OS/RZWindow.h"

namespace Razix {
    namespace Editor {

        class RZENativeWindow : public RZWindow
        {
        public:
            RZENativeWindow(void* windowHandle, const WindowProperties& properties);
            ~RZENativeWindow() {}

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

            void setWidth(uint32_t val) { m_Data.Width = val; }
            void setHeight(uint32_t val) { m_Data.Height = val; }
            void setTitle(const char* title) override;

            EventCallbackFn& getEventCallbackFunc() { return EventCallback; }

            void OnEventDefaultBind(RZEvent& event) {}

        private:
            WindowProperties m_Data;
            void*            m_NativeHandle;
            EventCallbackFn  EventCallback;
        };
    }    // namespace Editor
}    // namespace Razix
