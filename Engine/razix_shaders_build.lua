------------------------------------------------------------------------------
-- Shaders build HLSL

  -- Engine distributed DXC location
dxcLocation = "%{wks.location}../Engine/Content/Shaders/Tools/dxc/bin/x64/"

group "Engine/content"
    project "Shaders"
        kind "Utility"

        files
        { 
            -- Shader files
            -- HLSL - primary language for all platforms shader gen
            "content/Shaders/HLSL/**.h",
            "content/Shaders/HLSL/**.hlsl",
            "content/Shaders/HLSL/**.hlsli",
            "content/Shaders/HLSL/**.vert",
            "content/Shaders/HLSL/**.geom",
            "content/Shaders/HLSL/**.frag",
            "content/Shaders/HLSL/**.comp",
            -- GLSL (Deprecated)
            "content/Shaders/GLSL/**",
            -- PSSL (PlayStation 5)
            "content/Shaders/PSSL/**.pssl",
            "content/Shaders/PSSL/**.h",
            "content/Shaders/PSSL/**.hs",
            -- MSL (Apple)
            -- Razix Shader File
            "content/Shaders/Razix/**.rzsf",
            -- Generated Reflectiond data and other platform shaders.
            "content/Shaders/Generated/**"
        }

    filter "system:windows"
        -- TODO: Add as rules, every shader file type will have it's own rule
        -- TODO: customize per build config, release/distr shaders will need signing and extra optimization flags (-O3 etc.)
        -- Don't build the these shader types, they are compiled by the engine once and cached/include only
        filter { "files:**.glsl or **.hlsl or **.pssl or **.cg or **.rzsf"}
            flags { "ExcludeFromBuild" }

        -- Build GLSL files based on their extension
        -------------------
        -- VERTEX SHADER
        -------------------
        filter { "files:**.vert" }
            removeflags "ExcludeFromBuild"
            buildmessage 'Compiling HLSL Vertex shader : %{file.name}'
            buildcommands 
            {
                -- Compile CSO binary
                "echo [Compiling] CSO for DX12 backend...",
                '"%{dxcLocation}/dxc.exe" -D __HLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -E VS_MAIN -T vs_6_0  "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.name }.cso"',
                -- Compile SPIRV binary
                "echo [Compiling] SPIRV for VK backend",
                '"%{dxcLocation}/dxc.exe" -spirv -D __GLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -E VS_MAIN -T vs_6_0  -fspv-reflect -fspv-target-env=vulkan1.3 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.name }.spv"',
                -- Generate GLSL (for reference only)???
                --"echo [WIP] [Generating] GLSL from SPIRV",
                -- Generate Reflection Data JSON
                --"echo [Generating] exporting shader Reflection data",
                --'"%{dxcLocation}/dxc.exe" -E VS_MAIN -T vs_6_0 "%{file.directory}/%{file.name}" -Fre "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.name }.json"',
                -- Generate RootSig desc
                --"echo [Generating] exporting root signature info",
                --'"%{dxcLocation}/dxc.exe" -E VS_MAIN -T vs_6_0 "%{file.directory}/%{file.name}" -Frs "%{wks.location}/../Engine/content/Shaders/Generated/RootSignature/%{file.name }.rootsig"',
                -- Generate Assembly
                "echo [Generating] exporting shader assembly listing",
                '"%{dxcLocation}/dxc.exe" -E VS_MAIN -T vs_6_0 "%{file.directory}/%{file.name}" -Fc "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.name }.isa"'
            }
            buildoutputs 
            {
                "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.name }.cso",
                "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.name }.spv",
                -----------------------------------------------------------------------------
                "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.name }.json",
                "%{wks.location}/../Engine/content/Shaders/Generated/RootSignature/%{file.name }.rootsig",
                "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.name }.isa"
            }
        -------------------
        -- PIXEL SHADER
        -------------------
        filter { "files:**.frag" }
            removeflags "ExcludeFromBuild"
            buildmessage 'Compiling HLSL Pixel shader : %{file.name}'
            buildcommands 
            {
                -- Compile CSO binary
                "echo [Compiling] CSO for DX12 backend...",
                '"%{dxcLocation}/dxc.exe" -D __HLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -E PS_MAIN -T ps_6_0 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.name }.cso"',
                -- Compile SPIRV binary
                "echo [Compiling] SPIRV for VK backend",
                '"%{dxcLocation}/dxc.exe" -spirv -D __GLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -E PS_MAIN -T ps_6_0  -fspv-reflect -fspv-target-env=vulkan1.3 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.name }.spv"',
                -- Generate GLSL (for reference only)???
                --"echo [WIP] [Generating] GLSL from SPIRV",
                -- Generate Reflection Data JSON
                --"echo [Generating] exporting shader Reflection data",
                --'"%{dxcLocation}/dxc.exe" -E PS_MAIN -T ps_6_0 "%{file.directory}/%{file.name}" -Fre "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.name }.json"',
                -- Generate RootSig desc
                --"echo [Generating] exporting root signature info",
                --'"%{dxcLocation}/dxc.exe" -E PS_MAIN -T ps_6_0 "%{file.directory}/%{file.name}" -Frs "%{wks.location}/../Engine/content/Shaders/Generated/RootSignature/%{file.name }.rootsig"',
                -- Generate Assembly
                "echo [Generating] exporting shader assembly listing",
                '"%{dxcLocation}/dxc.exe" -E PS_MAIN -T ps_6_0 "%{file.directory}/%{file.name}" -Fc "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.name }.isa"'
            }
            buildoutputs 
            {
                "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.name }.cso",
                "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.name }.spv",
                -----------------------------------------------------------------------------
                --"%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.name }.json",
                --"%{wks.location}/../Engine/content/Shaders/Generated/RootSignature/%{file.name }.rootsig",
                "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.name }.isa"
            }
group""
------------------------------------------------------------------------------