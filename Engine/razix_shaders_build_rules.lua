-- Shaders build HLSL

-- https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst

-- Engine distributed DXC location
dxcLocation = ""
VulkanSDK = ""
if os.host() == "windows" then
    dxcLocation = "%{wks.location}../Engine/Content/Shaders/Tools/dxc/bin/x64/"
elseif os.host() == "macosx" or os.host() == "linux" then
    VulkanSDK = os.getenv("VULKAN_SDK")
    dxcLocation = "%{VulkanSDK}/bin/"
end
print("Loading dxc from: " .. dxcLocation)
-- Note: All shaders are built using SM6

-- TODO: Add as rules, every shader file type will have it's own rule
-- TODO: customize per build config, release/distr shaders will need signing and extra optimization flags (-O3 etc.)
-- Don't build the these shader types, they are compiled by the engine once and cached/include only
filter { "files:**.glsl or **.hlsl or **.pssl or **.cg or **.rzsf"}
    flags { "ExcludeFromBuild" }
filter {}

-- Signing shaders, DXIL.dll needs to be present in the same directoy as we are executing dxc.exe to sign shaders

-- Build GLSL files based on their extension
-------------------
-- VERTEX SHADER
-------------------
filter { "files:**.vert.hlsl" }
    removeflags "ExcludeFromBuild"
    buildmessage 'Compiling HLSL Vertex shader : %{file.name}'

    buildcommands
    {
        -- Compile CSO binary
        "echo [Compiling] CSO for DX12 backend...",
        '"%{dxcLocation}/dxc" -D __HLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E VS_MAIN -T vs_6_0  "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename}.cso"',
        -- Compile SPIRV binary
        "echo [Compiling] SPIRV for VK backend",
        '"%{dxcLocation}/dxc" -spirv -D __GLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E VS_MAIN -T vs_6_0 -fspv-target-env=vulkan1.3 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv"',
        -- Generate GLSL (for reference only)???
        "echo [Generating] GLSL from SPIRV",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage vert --entry VS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage vert --entry VS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
        -- Generate Assembly
        "echo [Generating] exporting shader assembly listing",
        '"%{dxcLocation}/dxc" -D __HLSL__ -E VS_MAIN -T vs_6_0 -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" "%{file.directory}/%{file.name}" -Fc "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename}.isa"'
    }
    buildoutputs
    {
        "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename }.cso",
        "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv",
        -----------------------------------------------------------------------------
        "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename }.glsl",
        "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename }.json",
        "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename }.isa"
    }
-------------------
-- PIXEL SHADER
-------------------
filter { "files:**.frag.hlsl" }
    removeflags "ExcludeFromBuild"
    buildmessage 'Compiling HLSL Pixel shader : %{file.name}'
    buildcommands
    {
        -- Compile CSO binary
        "echo [Compiling] CSO for DX12 backend...",
        '"%{dxcLocation}/dxc" -D __HLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E PS_MAIN -T ps_6_0 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename}.cso"',
        -- Compile SPIRV binary
        "echo [Compiling] SPIRV for VK backend",
        '"%{dxcLocation}/dxc" -spirv -D __GLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E PS_MAIN -T ps_6_0 -fspv-target-env=vulkan1.3 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv"',
        -- Generate GLSL (for reference only)???
        "echo [Generating] GLSL from SPIRV",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage frag --entry PS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage frag --entry PS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
        -- Generate Assembly
        "echo [Generating] exporting shader assembly listing",
        '"%{dxcLocation}/dxc" -D __HLSL__ -E PS_MAIN -T ps_6_0 -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" "%{file.directory}/%{file.name}" -Fc "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename}.isa"'
    }
    buildoutputs
    {
        "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename }.cso",
        "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv",
        -----------------------------------------------------------------------------
        "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename }.glsl",
        "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename }.json",
        "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename }.isa"
    }
-------------------
-- COMPUTE SHADER
-------------------
filter { "files:**.comp.hlsl" }
    removeflags "ExcludeFromBuild"
    buildmessage 'Compiling HLSL Compute shader : %{file.name}'
    buildcommands
    {
        -- Compile CSO binary
        "echo [Compiling] CSO for DX12 backend...",
        '"%{dxcLocation}/dxc" -D __HLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E CS_MAIN -T cs_6_0  "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename}.cso"',
        -- Compile SPIRV binary
        "echo [Compiling] SPIRV for VK backend",
        '"%{dxcLocation}/dxc" -spirv -D __GLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E CS_MAIN -T cs_6_0 -fspv-target-env=vulkan1.3 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv"',
        -- Generate GLSL (for reference only)???
        "echo [Generating] GLSL from SPIRV",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage comp --entry CS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage comp --entry CS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
        -- Generate Assembly
        "echo [Generating] exporting shader assembly listing",
        '"%{dxcLocation}/dxc" -D __HLSL__ -E CS_MAIN -T cs_6_0 -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" "%{file.directory}/%{file.name}" -Fc "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename}.isa"'
    }
    buildoutputs
    {
        "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename }.cso",
        "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv",
        -----------------------------------------------------------------------------
        "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename }.glsl",
        "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename }.json",
        "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename }.isa"
    }
-------------------
-- GEOMETRY SHADER
-------------------
filter { "files:**.geom.hlsl" }
    removeflags "ExcludeFromBuild"
    buildmessage 'Compiling HLSL Geometry shader : %{file.name}'
    buildcommands
    {
        -- Compile CSO binary
        "echo [Compiling] CSO for DX12 backend...",
        '"%{dxcLocation}/dxc" -D __HLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E GS_MAIN -T gs_6_0  "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename}.cso"',
        -- Compile SPIRV binary
        "echo [Compiling] SPIRV for VK backend",
        '"%{dxcLocation}/dxc" -spirv -D __GLSL__ -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" -E GS_MAIN -T gs_6_0 -fspv-target-env=vulkan1.3 "%{file.directory}/%{file.name}" -Fo "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv"',
        -- Generate GLSL (for reference only)???
        "echo [Generating] GLSL from SPIRV",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage geom --entry GS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        'spirv-cross "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage geom --entry GS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
        -- Generate Assembly
        "echo [Generating] exporting shader assembly listing",
        '"%{dxcLocation}/dxc" -D __HLSL__ -E GS_MAIN -T gs_6_0 -I "%{wks.location}/../Engine/content/Shaders/HLSL" -I "%{wks.location}/../Engine/content/Shaders/ShaderCommon" -I "%{wks.location}/../Engine/src" "%{file.directory}/%{file.name}" -Fc "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename}.isa"'
    }
    buildoutputs
    {
        "%{wks.location}/../Engine/content/Shaders/Compiled/CSO/%{file.basename }.cso",
        "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename }.spv",

        -----------------------------------------------------------------------------
        "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename }.glsl",
        "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename }.json",
        "%{wks.location}/../Engine/content/Shaders/Generated/Assembly/%{file.basename }.isa"
    }
group""
------------------------------------------------------------------------------