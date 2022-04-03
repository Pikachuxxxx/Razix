-- PS4 and PS5 system override and VS settings

local vstudio = premake.vstudio

if vstudio.vs2010_architectures ~= nil then
    vstudio.vs2010_architectures.ORBIS   = "ORBIS"
    vstudio.vs2010_architectures.Prospero = "Prospero"
    vsstudio.vs2010_architectures.PSVita = "PSVita"
    vsstudio.vs2010_architectures.PS3 = "PS3"


    premake.api.addAllowed("system", "ORBIS")
	premake.api.addAllowed("system", "Prospero")
    premake.api.addAllowed("system", "PSVita")
	premake.api.addAllowed("system", "PS3")
end