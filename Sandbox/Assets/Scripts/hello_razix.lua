--Hello World Lua Script for Razix Engine
-- This is a simple hello world srcipt that utilizes the Razix Engine's Logging system to log information by the client
-- Copyright Razix Engine 2022 by Pikachuxxxx (Phani Srikar)

function OnStart()
    RZLog.Info("Hello Razix Engine!")
    RZLog.Trace("OnStart called!")
end

position = {
    x = 2.0,
    y = 3.0,
    z = 4.0,
}

function OnUpdate(dt)
    --RZLog.Warn("OnUpdate called! :" .. dt)

    -- Test drawing a debug point
    DebugDraw.DrawPoint(position, 0.5, position)

end
