--Hello World Lua Script for Razix Engine
-- This is a simple hello world srcipt that utilizes the Razix Engine's Logging system to log information by the client
-- Copyright Razix Engine 2022 by Pikachuxxxx (Phani Srikar)

function OnStart()
    RZLog.Info("Hello Razix Engine!")
    RZLog.Trace("OnStart called!")
end

function OnUpdate(dt)
    --RZLog.Warn("OnUpdate called! :" .. dt)
end
