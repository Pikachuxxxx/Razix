project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

        includedirs
        {
            "./imgui/"
        }
    
        externalincludedirs
        {
            "./imgui/"
        }

    files
    {
        "imgui/imconfig.h",
        "imgui/imgui.h",
        "imgui/imgui.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_internal.h",
        "imgui/imgui_widgets.cpp",
        "imgui/imstb_rectpack.h",
        "imgui/imstb_textedit.h",
        "imgui/imstb_truetype.h",
        "imgui/imgui_demo.cpp",
        "imgui/imgui_tables.cpp",
        "imgui/plugins/**.h",
        "imgui/plugins/**.cpp",
        "imgui/plugins/**.inl"
    }

    filter "system:linux"
        buildoptions
        {
            "-fPIC"
        }

    filter "system:windows"
        systemversion "latest"
        cppdialect (engine_global_config.cpp_dialect)
        staticruntime "off"
