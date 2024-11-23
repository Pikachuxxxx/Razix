project "RazixVersion"
	kind "ConsoleApp"
	language "C++"

	excludes { "%{wks.location}../../../.modules/**premake5.lua" }

	files
	{
        "version.cpp"
	}

    -- Include paths
    includedirs
    {
        "../../../Engine/src"
    }

    -- Macos include paths
    externalincludedirs
    {
        "../../../Engine/src"
    }