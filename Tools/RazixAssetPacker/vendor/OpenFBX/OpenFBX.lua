project "OpenFBX"
	kind "StaticLib"
	language "C++"
	cppdialect (engine_global_config.cpp_dialect)
	staticruntime "off"

    files { "*.h", "*.cpp", "*.c" }

    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:GoldMaster"
        defines { "RAZIX_GOLD_MASTER", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
