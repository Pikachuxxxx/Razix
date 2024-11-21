workspace "QtNodeGraph"
    location "build"
    startproject "example"
    flags 'MultiProcessorCompile'
    architecture "x86_64"

    -- Output directory path based on build config
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    -- Binaries Output directory
    targetdir ("bin/%{outputdir}/")
    -- Intermediate files Output directory
    objdir ("bin-int/%{outputdir}/obj/")
    -- Debugging directory = where the main premake5.lua is located
    debugdir "%{wks.location}../"

    -- Various build configuration for the engine
    configurations
    {
        "Debug",
        "Release"
    }

    buildoptions
    {
        "-stdlib=libc++"
    }

    include "QtNodeGraph.lua"
    include "example/example.lua"
