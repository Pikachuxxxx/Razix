project "OpenFBX"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

    files { "*.h", "*.cpp" }

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
