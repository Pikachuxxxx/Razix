project "lua"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

	defines
	{
		"LUA_COMPAT_MATHLIB=1"
	}

	local lua_src  = 'src/'

	filter "configurations:debug"
		defines "LUA_USE_APICHECK=1"
	filter {}

	files
	{
		lua_src .. '*.h',
		lua_src .. '*.c'
	}

	removefiles
	{
		lua_src .. 'luac.c',
		lua_src .. 'lua.c'
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
