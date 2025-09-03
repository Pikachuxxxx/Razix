project "GLFW"
    kind "StaticLib"
    language "C"
    systemversion "latest"
    staticruntime "off"
    removeplatforms "emscripten"

    files
    {
	    "glfw/src/**.h",
	    "glfw/src/**.c",
	    "glfw/include/**.h",
	    "glfw/include/**.c"
    }
    
    filter "system:linux"
        defines
        {
            "_GLFW_X11"
        }
    
    filter "system:macosx"
        systemversion "14.0"
        defines { "_GLFW_COCOA" }
        files
    	{
		    "glfw/src/**.m"
	    }
    
    filter "system:windows"
        defines
        {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:GoldMaster"
        runtime "Release"
        symbols "Off"
        optimize "Full"
    filter {}