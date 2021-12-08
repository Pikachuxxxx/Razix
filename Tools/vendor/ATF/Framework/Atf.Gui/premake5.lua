project "Atf.Gui"
    kind "SharedLib"
    language "C#"
    location"./"
        framework "4.0"

    targetname "Atf.Gui"
    namespace ("Sce.Atf")

    files
    {
        "**.cs",
        "Resources/**.png",
        "Resources/**.ico",
        "Resources/**.xml",
        "Resources/**.db",
        "Resources/**.CUR"
    }

    removefiles
    {
        "Applications/Listers/AssetLister.cs",
        "Applications/Listers/GridControlAdapter.cs",
        "Applications/VersionControl/ReconcileForm.Designer.cs",
        "Applications/IReplaceUI.cs",
        "Applications/IResultsUI.cs",
        "Applications/ISearchableContextUI.cs",
        "Applications/ISearchUI.cs",
        "Applications/PluginManagerForm.cs",
        "Applications/PluginManagerForm.designer.cs",
        "Applications/PluginManagerService.cs",
        "Applications/StatusService.cs",
        "Controls/SearchResultsListView.cs",
        "Controls/Adaptable/ColumnHeadersAdapter.cs",
        "Dom/DomNodeQueryRoot.cs",
        "Dom/DomNodeReplaceToolStrip.cs",
        "Dom/DomNodeSearchToolStrip.cs",
        "Dom/CustomTypeDescriptorNodeAdapter.cs",
        "obj/**"
    }

    links
    {
        "Atf.Core",
        "System",
        "System.ComponentModel.Composition",
        "System.Core",
        "System.Data",
        "System.Design",
        "System.Drawing",
        "System.Drawing.Design",
        "System.Web.Services",
        "System.Xml",
        "Microsoft.Scripting",
        "SharpDX",
        "SharpDX.Direct2D1",
        "SharpDX.DXGI"
    }

    libdirs
    {
        "../../ThirdParty",
        "../../ThirdParty/IronPython/",
        "../../ThirdParty/SharpDX/"
    }

    filter "files:**.rtf or **.resx or **.png or **.ico or **.CUR or **.xml or **.db"
            buildaction "Embed"
    filter ""
