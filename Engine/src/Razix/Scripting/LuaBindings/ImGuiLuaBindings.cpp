// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Scripting/RZLuaScriptHandler.h"

#include <imgui.h>

namespace Razix {
    namespace Scripting {

        void RZLuaScriptHandler::bindImGuiAPI()
        {
            sol::table globals = m_State.globals();
            sol::table imgui   = m_State.create_table();
            globals["imgui"]   = imgui;

            // Functions

            imgui.new_usertype<ImVec2>("ImVec2",
                sol::constructors<ImVec2(float, float)>(),
                "x",
                &ImVec2::x,
                "y",
                &ImVec2::y);

            // TODO: Put a lot more stuff in here
            imgui.new_usertype<ImGuiIO>("IO",
                "new",
                sol::no_constructor,
                "configFlags",
                &ImGuiIO::ConfigFlags,
                "backendFlags",
                &ImGuiIO::BackendFlags,
                "displaySize",
                &ImGuiIO::DisplaySize,
                "deltaTime",
                &ImGuiIO::DeltaTime,
                "iniSavingRate",
                &ImGuiIO::IniSavingRate);

            imgui.new_enum("ComboFlags",
                "None",
                0,
                "PopupAlignLeft",
                1 << 0,
                "HeightSmall",
                1 << 1,
                "HeightRegular",
                1 << 2,
                "HeightLarge",
                1 << 3,
                "HeightLargest",
                1 << 4,
                "NoArrowButton",
                1 << 5,
                "NoPreview",
                1 << 6,
                "HeightMask_",
                ImGuiComboFlags_HeightSmall | ImGuiComboFlags_HeightRegular | ImGuiComboFlags_HeightLarge | ImGuiComboFlags_HeightLargest);

            imgui["getIO"]       = ImGui::GetIO;
            imgui["getStyle"]    = ImGui::GetStyle;
            imgui["newFrame"]    = ImGui::NewFrame;
            imgui["endFrame"]    = ImGui::EndFrame;
            imgui["render"]      = ImGui::Render;
            imgui["getDrawData"] = ImGui::GetDrawData;

            imgui["showDemoWindow"] = []() {
                bool bShow;
                ImGui::ShowDemoWindow(&bShow);
                return bShow;
            };

            imgui["beginWindow"] = sol::overload([](const char* _str) { return ImGui::Begin(_str); },
                [](const char* _str, ImGuiWindowFlags _flags) { return ImGui::Begin(_str, NULL, _flags); });
            imgui["endWindow"]   = ImGui::End;

            imgui["beginCombo"] = sol::overload(
                [](const char* _label, const char* _preview_value) { return ImGui::BeginCombo(_label, _preview_value); });
            imgui["endCombo"]   = ImGui::EndCombo;

            imgui["selectable"] = sol::overload(
                [](const char* _label, bool _bSelected, sol::function _cb) {
                    if (ImGui::Selectable(_label, &_bSelected))
                        _cb(_bSelected);
                },
                [](const char* _label, bool _bSelected, ImGuiSelectableFlags _flags, sol::function _cb) {
                    if (ImGui::Selectable(_label, &_bSelected, _flags))
                        _cb(_bSelected);
                });

            imgui["setItemDefaultFocus"] = ImGui::SetItemDefaultFocus;

            imgui["separator"] = ImGui::Separator;

            imgui["sameLine"] = sol::overload(
                ImGui::SameLine, []() { ImGui::SameLine(); }, [](float _localPos) { ImGui::SameLine(_localPos); });

            imgui["newLine"] = ImGui::NewLine;
            imgui["spacing"] = ImGui::Spacing;
            imgui["dummy"]   = ImGui::Dummy;

            imgui["text"] = [](const char* _text) {
                ImGui::TextUnformatted(_text);
            };

            imgui["button"] = sol::overload(ImGui::Button, [](const char* _label) { return ImGui::Button(_label); });

            imgui["checkbox"] = [](const char* _name, bool _bSelected, sol::function _cb) {
                if (ImGui::Checkbox(_name, &_bSelected)) {
                    _cb(_bSelected);
                }
            };
            imgui["radioButton"] = (bool (*)(const char*, bool)) ImGui::RadioButton;

            imgui["dragFloat"] = sol::overload(
                [](const char* _label, float _currentValue, sol::function _cb) {
                    if (ImGui::DragFloat(_label, &_currentValue))
                        _cb(_currentValue);
                },
                [](const char* _label, float _currentValue, float _v_speed, sol::function _cb) {
                    if (ImGui::DragFloat(_label, &_currentValue, _v_speed))
                        _cb(_currentValue);
                },
                [](const char* _label, float _currentValue, float _v_speed, float _v_min, sol::function _cb) {
                    if (ImGui::DragFloat(_label, &_currentValue, _v_speed, _v_min))
                        _cb(_currentValue);
                },
                [](const char* _label, float _currentValue, float _v_speed, float _v_min, float _v_max, sol::function _cb) {
                    if (ImGui::DragFloat(_label, &_currentValue, _v_speed, _v_min, _v_max))
                        _cb(_currentValue);
                },
                [](const char*    _label,
                    float         _currentValue,
                    float         _v_speed,
                    float         _v_min,
                    float         _v_max,
                    const char*   _fmt,
                    sol::function _cb) {
                    if (ImGui::DragFloat(_label, &_currentValue, _v_speed, _v_min, _v_max, _fmt))
                        _cb(_currentValue);
                },
                [](const char*    _label,
                    float         _currentValue,
                    float         _v_speed,
                    float         _v_min,
                    float         _v_max,
                    const char*   _fmt,
                    float         _power,
                    sol::function _cb) {
                    if (ImGui::DragFloat(_label, &_currentValue, _v_speed, _v_min, _v_max, _fmt, _power))
                        _cb(_currentValue);
                });
        }
    }    // namespace Scripting
}    // namespace Razix