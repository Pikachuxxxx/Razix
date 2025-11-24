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