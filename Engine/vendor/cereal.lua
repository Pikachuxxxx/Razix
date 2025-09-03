project "cereal"
	kind "None"
	language "C++"
	cppdialect (engine_global_config.cpp_dialect)
	staticruntime "off"

	files 
    {
		"cereal/include/**.h",
        "cereal/include/**.hpp",
	}

    includedirs
    {
        "cereal/include",
    }

	externalincludedirs
	{
		"cereal/include",
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
        optimize "Full"
