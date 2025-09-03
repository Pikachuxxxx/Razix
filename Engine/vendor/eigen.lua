project "Eigen"
	kind "None"
	language "C++"
	cppdialect (engine_global_config.cpp_dialect)
	staticruntime "off"

	files 
    {
        "eigen/Eigen/**",
		"eigen/Eigen/src/**.h"
	}

    includedirs
    {
        "eigen/Eigen",
    }

	externalincludedirs
	{
		"eigen/Eigen",
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
