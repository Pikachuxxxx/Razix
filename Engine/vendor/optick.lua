project "Optick"
    kind "StaticLib"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    files {
        "optick/src/optick_capi.cpp",
        "optick/src/optick_core.cpp",
        "optick/src/optick_gpu.cpp",
        "optick/src/optick_message.cpp",
        "optick/src/optick_miniz.cpp",
        "optick/src/optick_serialization.cpp",
        "optick/src/optick_server.cpp",

        "optick/src/optick.config.h",
        "optick/src/optick_capi.h",
        "optick/src/optick_common.h",
        "optick/src/optick_core.h",
        "optick/src/optick_core.platform.h",
        "optick/src/optick_gpu.h",
        "optick/src/optick_memory.h",
        "optick/src/optick_message.h",
        "optick/src/optick_miniz.h",
        "optick/src/optick_serialization.h",
        "optick/src/optick_server.h",
        "optick/src/optick.h"
    }

    filter "system:windows"
        files {
            "optick/src/optick_core.win.h",
            "optick/src/optick_gpu.d3d12.cpp"
        }

    filter "system:macosx"
        files {
            "optick/src/optick_core.macos.h"
        }

    filter "system:linux"
        files {
            "optick/src/optick_core.linux.h"
        }

    filter "system:android"
        files {
            "optick/src/optick_core.linux.h"
        }

    filter {}
        files {
            "optick/src/optick_core.freebsd.h", -- optional if needed
            "optick/src/optick_gpu.vulkan.cpp"
        }

   filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:GoldMaster"
        runtime "Release"
        symbols "Off"
        optimize "Full"
