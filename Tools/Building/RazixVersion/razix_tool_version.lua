project "RazixVersion"
    kind "ConsoleApp"
    language "C++"

    excludes { "%{wks.location}../../../.modules/**premake5.lua" }

    files
    {
        "version.cpp",
        "../../../Engine/src/Razix/Core/std/RZsprintf.c"
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
