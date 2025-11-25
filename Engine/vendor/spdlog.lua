project "spdlog"
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
		"spdlog/src/**.cpp",
		"spdlog/include/**.h",
	}

	externalincludedirs
	{
		"spdlog/include",
	}
	
	warnings "off"