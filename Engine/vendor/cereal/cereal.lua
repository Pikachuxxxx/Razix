project "cereal"
	kind "None"
	language "C++"
	cppdialect (engine_global_config.cpp_dialect)
	staticruntime "off"

	files 
    {
		"include/**.h",
        "include/**.hpp",
	}

    includedirs
    {
        "include",
    }

	externalincludedirs
	{
		"include",
	}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:GoldMaster"
        runtime "Release"
        symbols "Off"
        optimize "Speed"  -- Changed from "Full" to "Speed"
