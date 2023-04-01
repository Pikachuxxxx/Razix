--Hello World Lua Script for Razix Engine
-- This is a simple hello world srcipt that utilizes the Razix Engine's Logging system to log information by the client
-- Copyright Razix Engine 2022 by Pikachuxxxx (Phani Srikar)

function OnStart()
    RZLog.Info("Hello Razix Engine!")
    RZLog.Trace("OnStart called!")
end


function OnUpdate(dt)
    --RZLog.Warn("OnUpdate called! :" .. dt)
    
    -- Test drawing a debug point
    local p = vec3.new(5, 5, 5)
    local c = vec3.new(0.25, 0.85, 0.2)
    DebugDraw.DrawPoint(p, 0.1, c)
end
