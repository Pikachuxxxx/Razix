#pragma once

#include <QFrame>
#include <QVBoxLayout>

#include "generated/ui_RZEInspectorWindow.h"

#include "UI/Windows/RZESceneHierarchyPanel.h"

#include "UI/Widgets/RZECollapsingHeader.h"

#include "UI/Widgets/ComponentsUI/RZECameraComponentUI.h"
#include "UI/Widgets/ComponentsUI/RZELightComponentUI.h"
#include "UI/Widgets/ComponentsUI/RZELuaScriptComponentUI.h"
#include "UI/Widgets/ComponentsUI/RZEMeshRendererComponentUI.h"
#include "UI/Widgets/ComponentsUI/RZESpriteRendererComponentUI.h"
#include "UI/Widgets/ComponentsUI/RZETransformComponentUI.h"

enum ComponentsFlag : uint32_t
{
    RZ_FLAG_COMPONENT_ID              = 0,
    RZ_FLAG_COMPONENT_TAG             = 1,
    RZ_FLAG_COMPONENT_TRANSFORM       = 2,
    RZ_FLAG_COMPONENT_CAMERA          = 3,
    RZ_FLAG_COMPONENT_LIGHT           = 4,
    RZ_FLAG_COMPONENT_LUA_SCRIPT      = 5,
    RZ_FLAG_COMPONENT_MESH_RENDERER   = 6,
    RZ_FLAG_COMPONENT_SPRITE_RENDERER = 7,
    RZ_FLAG_COMPONENT_ACTIVE          = 8,
    RZ_FLAG_COMPONENT_HIERARCHY       = 9,
    // Variable to keep track of the components count
    RZ_FLAG_COMPONENTS_COUNT
};

// https://stackoverflow.com/questions/47981/how-do-i-set-clear-and-toggle-a-single-bit
// For setting use flag |= flag_1 | flag_2;
// To remove flag_1 use flag &= ~flag_1
typedef uint32_t ComponentMask;

// All components will be added to this window and will be enabled on a need to basis; We can remove or add (only single isntance for now)
// This way code and design get's easier, but the Component widgets will have seperate UI and class files for ease of management;
// Will deriving from a common RZComponent and RZEComponentUI will benefit with plugin development and reduce code duplication? If so Implement it
namespace Razix {
    namespace Editor {
        class RZEInspectorWindow : public QFrame
        {
            Q_OBJECT

        public:
            RZEInspectorWindow(RZESceneHierarchyPanel* hierarchyPanel, QFrame* parent = nullptr);
            ~RZEInspectorWindow();

            QVBoxLayout& getBoxLayout() { return *(ui.scrollLayout); }

        signals:
            void InspectorPropertyChanged();

        public slots:
            void OnNameEdit();
            void OnEntitySelected(RZEntity entity);

        private:
            Ui::InspectorWindow ui;
            // TODO: Use a custom bitmask type with enums
            ComponentMask m_ComponentsMask;
            RZEntity      m_InspectingEntity;
            // Hold the components here
            RZETransformComponentUI*      m_TrasformComponentUI;
            RZECameraComponentUI*         m_CameraComponentUI;
            RZEMeshRendererComponentUI*   m_MeshRendererComponentUI;
            RZELuaScriptComponentUI*      m_LuaScriptComponentUI;
            RZELightComponentUI*          m_LightComponentUI;
            RZESpriteRendererComponentUI* m_SpriteRendererComponentUI;

            RZECollapsingHeader* m_TrasformComponentSection;
            RZECollapsingHeader* m_CameraComponentSection;
            RZECollapsingHeader* m_LightComponentSection;
            RZECollapsingHeader* m_LuaScriptComponentSection;
            RZECollapsingHeader* m_MeshRendererComponentSection;
            RZECollapsingHeader* m_SpriteRendererComponentSection;

        private:
            void initComponents();
            void destroyComponents();
        };
    }    // namespace Editor
}    // namespace Razix
