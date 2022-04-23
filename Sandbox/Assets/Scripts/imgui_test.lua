-- ImGui test Lua Script for Razix Engine
-- This is a test for imgui scripting from lua, currently onle a few imgui elemts and types are available for use the API exposure needs to be improved later
-- Copyright Razix Engine 2022 by Pikachuxxxx (Phani Srikar)

function OnStart()
    RZLog.Info("ImGui Test!")
end

local value = false
local range = 0.0

function get_value(res)
    value = res
    RZLog.Warn("isChecked : " .. tostring(res))
end

function get_dragFLoatVal(val)
    range = val
end

function OnUpdate(dt)
    imgui.beginWindow("ImGui Lua Window")
        imgui.text("Delta Time : " .. dt)
        imgui.checkbox("Label", value, get_value)
        imgui.button("somebutton")
        imgui.dragFloat("Range", range, get_dragFLoatVal)
    imgui.endWindow();
end
