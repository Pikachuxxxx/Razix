#ifndef _RZ_LUA_SCRIPT_ASSET_H_
#define _RZ_LUA_SCRIPT_ASSET_H_

#include "Razix/AssetSystem/RZAssetBase.h"

#include "Razix/Core/Containers/string.h"

#include "Razix/Core/Reflection/RZReflection.h"

namespace Razix {

    /**
     * RZLuaScriptAsset represents a Lua script file.
     * 
     * Storage Type: kMemoryBacked
     * 
     * The script source or compiled bytecode is loaded into memory. 
     * Multiple entities can reference the same script (e.g., AI behavior).
     * 
     * Serialization:
     * - Metadata (path, compilation status) in registry.
     * - Source code or bytecode in separate .lua or .luac file.
     */
    struct RZLuaScriptAsset
    {
        RAZIX_ASSET;

        RZString SourceCode;
        u32      ScriptPath;
        u64      LastModifiedTime;
        bool     bIsCompiled;
        bool     bAutoReload;
        u8       _pad[6];
    };

    RAZIX_REFLECT_TYPE_START(RZLuaScriptAsset)
    RAZIX_REFLECT_MEMBER(ScriptPath)
    RAZIX_REFLECT_MEMBER(LastModifiedTime)
    RAZIX_REFLECT_MEMBER(bIsCompiled)
    RAZIX_REFLECT_MEMBER(bAutoReload)
    RAZIX_REFLECT_TYPE_END(RZLuaScriptAsset)

}    // namespace Razix

#endif    // _RZ_LUA_SCRIPT_ASSET_H_
