-- PS4 and PS5 system override and VS settings

local p = premake
local vs = require("vstudio")

-- system defines the sub-systems that the architecture will use for SDK and idk what the fuck it even means at this point
p.api.addAllowed("system", "Prospero")  -- PS5
p.api.addAllowed("system", "ORBIS")     -- PS4
p.api.addAllowed("system", "Durango")   -- XBox One X

-- Not sure about architecture, shouldn't system attribute be sufficient for project??? Apparently not motherfucker!
p.api.addAllowed("architecture", {"Prospero"})
p.api.addAllowed("architecture", {"ORBIS"})
p.api.addAllowed("architecture", {"Durango"})

-- Use platforms for filetering and define the sub-system and architecture the appropirate toolset and settings
-- consoled need custom toolset to be specified ex. PS5 needs custom clang tool set with the system set to Prospero and architecture also set to Prospero
-- Hence we override these settinfg for premake to generate them
-- platforms is the attribute that will define what is supported for a given solution 
local function archFromConfig_cb(base, cfg, win32)
    if cfg.system == "Prospero" or cfg.architecture  == "Prospero" then
        return "Prospero"
    elseif cfg.system == "ORBIS" or cfg.architecture  == "ORBIS" then
        return "ORBIS"
    elseif cfg.system == "Durango" or cfg.architecture  == "Durango" then
        return "Durango"
    else
        return base( cfg, win32 )
    end
end

p.override( vs, "archFromConfig", archFromConfig_cb )