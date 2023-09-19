--Hello World Lua Script for Razix Engine
-- This is a simple hello world srcipt that utilizes the Razix Engine's Logging system to log information by the client
-- Copyright Razix Engine 2022 by Pikachuxxxx (Phani Srikar)

local elapse = 0.0

function OnStart(entity)
    RZLog.Info("Hello Razix Engine!")
    RZLog.Trace("OnStart called!")
    print(type(glm.vec3.new(5, 5, 5)))
    print(type(entity:getPosition()))

    RZLog.Warn("Entity Position X :" .. tostring(entity:getPosition().x))
    RZLog.Warn("Entity Position " .. tostring(entity:getPosition()))

    --local c = glm.vec3.new(0.25, 0.85, 0.2)
    --DebugDraw.DrawPoint(Entity.getPosition(), 0.1, c)
end


function OnUpdate(entity, dt)
    --RZLog.Warn("Entity Position :" .. Entity.getPosition())
    RZLog.Warn("Position Test" .. dt)
    RZLog.Warn("Entity Position " .. tostring(entity:getPosition()))

    elapse = elapse + dt

    local x = math.sin(elapse * 0.005) * 5.0
    local y = math.cos(elapse * 0.005) * 5.0

    entity:setPosition(glm.vec3.new(x, y, 6))

    -- Test drawing a debug point
    local p = glm.vec3.new(5, 5, 5)
    local c = glm.vec3.new(0.25, 0.85, 0.2)
    DebugDraw.DrawPoint(p, 0.1, c)
end
