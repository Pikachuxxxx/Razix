project "cereal"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	files 
    {
		"include/**.h",
        "include/**.hpp",
	}

    includedirs
    {
        "include",
    }

	sysincludedirs
	{
		"include",
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
