project "Atf.Core"
    kind "SharedLib"
    language "C#"
    location"./"
    framework "4.0"

    targetname "Atf.Core"
    namespace ("Sce.Atf")

    files
    {
        "**.cs"
    }

    removefiles
    {
        "Dom/DomNodeQueryable.cs",
        "Dom/DomNodeQueryMatch.cs",
        "Dom/XmlPersister.cs",
        "IQueryableContext.cs",
        "IQueryableReplaceContext.cs",
        "IQueryableResultContext.cs",
        "IQueryMatch.cs",
        "IQueryPredicate.cs",
        "IReplacingQueryPattern.cs",
        "obj/**"
    }

    links
    {
        "System",
        "System.Core",
        "System.ComponentModel.Composition",
        "System.Data",
        "System.Drawing",
        "System.Management",
        "System.Xml",
        "Wws.LiveConnect",
        "libcrashreport_net"
    }

    libdirs
    {
        "../../ThirdParty",
        "../../ThirdParty/libcrashreport_net/",
        "../../ThirdParty/Wws.LiveConnect/clr4/x64/"
    }
