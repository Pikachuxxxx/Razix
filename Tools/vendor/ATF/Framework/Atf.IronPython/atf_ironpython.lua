project "Atf.IronPython"
    kind "SharedLib"
    language "C#"
    location"./"
        framework "4.0"

    targetname "Atf.IronPython"
    namespace ("Sce.Atf")

    files
    {
        "**.cs"
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
        "System",
        "System.ComponentModel.Composition",
        "System.Windows.Forms",
        "Microsoft.Dynamic",
        "Microsoft.Scripting",
        "IronPython",
        "IronPython.Modules"
    }

    libdirs
    {
        "../../ThirdParty",
        "../../ThirdParty/IronPython/"
    }
