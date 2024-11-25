project "meshoptimizer"
	kind "StaticLib"
	language "C++"
	cppdialect (engine_global_config.cpp_dialect)
	staticruntime "off"

	disablewarnings {
		"4251",
		"4275",
		"4996"
	}

	defines {
		"SPDLOG_COMPILED_LIB",
	}

	files {
		"src/**.cpp",
		"include/**.h",
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

    filter "configurations:Distribution"
        runtime "Release"
        symbols "Off"
        optimize "Full"
