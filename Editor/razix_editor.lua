project "RazixEditor"
    kind "WindowedApp"
    language "C#"
    location"./"

    framework "4.0"

    targetname "RazixEditor"
    namespace "Razix"

    files
    {
        -- C++ source files
        "src/*.cpp",
        "src/*.h",
        -- C# Source file for interop
        "Properties/*.cs",
        "src/**.cs",
        "**.rtf",
        "**.resx",
        "**.png",
        "**.ico",
        "**.xml"
    }

    removefiles
    {
        "obj/**"
    }

    links
    {
        "Atf.Core",
        "Atf.Gui",
        "Atf.Gui.WinForms",
        "Atf.IronPython",
        "Atf.SyntaxEditorControl",
        "System",
        "System.ComponentModel.Composition",
        "System.configuration",
        "System.Core",
        "System.Data",
        "System.Drawing",
        "System.Windows.Forms",
        "System.XML",
        "System.XML.Linq"
    }

    libdirs
    {
        "bin/%{outputdir}/"
    }

    filter "files:**.rtf or **.resx or **.png or **.ico or **.xml"
            buildaction "Embed"
    filter ""