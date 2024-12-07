project "Eigen"
	kind "StaticLib"
	language "C++"
	cppdialect (engine_global_config.cpp_dialect)
	staticruntime "off"

	files 
    {
        "Eigen/**",
		"Eigen/src/**.h"
	}

    includedirs
    {
        "Eigen",
    }

	externalincludedirs
	{
		"Eigen",
	}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Distribution"
        runtime "Release"
        symbols "Off"
        optimize "Full"
