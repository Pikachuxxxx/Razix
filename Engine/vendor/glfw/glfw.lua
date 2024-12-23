project "GLFW"
    kind "StaticLib"
    language "C"
    systemversion "latest"

    files
    {
	"src/**.h",
	"src/**.c",
	"include/**.h",
	"include/**.c"
    }
    
    filter "system:linux"
        defines
        {
            "_GLFW_X11"
        }
    
    filter "system:macosx"
        defines { "_GLFW_COCOA" }
         files
    	{
		"src/**.m"
	}
    
    filter "system:windows"
        defines
        {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }
