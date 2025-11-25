project "lua"
    kind "StaticLib"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    defines
    {
            "LUA_COMPAT_MATHLIB=1"
    }

    local lua_src  = 'lua/src/'

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
