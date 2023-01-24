project "Atf.Gui.WinForms"
    kind "SharedLib"
    language "C#"
    location"./"
        framework "4.0"

    targetname "Atf.Gui.WinForms"
    namespace ("Sce.Atf")

    files
    {
        "**.cs",
        "**.rtf",
        "**.resx"
    }

    removefiles
    {
        "obj/**"
    }

    links
    {
        "Atf.Core",
        "Atf.Gui",
        "System",
        "System.ComponentModel.Composition",
        "System.Core",
        "System.Design",
        "System.Drawing",
        "System.Drawing.Design",
        "System.Runtime.Remoting",
        "System.Web.Services",
        "System.Windows.Forms",
        "System.Xml",
        "Microsoft.CSharp",
        "WindowsBase",
        "WeifenLuo.WinFormsUI.Docking"
    }

    libdirs
    {
        "../../ThirdParty",
        "../../ThirdParty/DockPanelSuite/"
    }

    filter "files:**.rtf or **.resx"
            buildaction "Embed"
    filter ""
