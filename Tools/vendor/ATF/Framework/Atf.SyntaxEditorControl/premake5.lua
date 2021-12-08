project "Atf.SyntaxEditorControl"
    kind "SharedLib"
    language "C#"
    location"./"
        framework "4.0"

    targetname "Atf.SyntaxEditorControl"
    namespace ("Sce.Atf")

    files
    {
        "**.cs",
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
        "System",
        "System.Drawing",
        "System.Windows.Forms",
        "System.Xml",
        "ActiproSoftware.Shared.Net20",
        "ActiproSoftware.SyntaxEditor.Addons.DotNet.Net20",
        "ActiproSoftware.SyntaxEditor.Net20",
        "ActiproSoftware.WinUICore.Net20"

    }

    libdirs
    {
        "../../ThirdParty",
        "../../ThirdParty/ActiPro/",
        "../../ThirdParty/Wws.LiveConnect/clr4/x64/"
    }

    filter "files:**.rtf or **.resx or **.png or **.ico or **.xml"
        buildaction "Embed"
    filter ""