-- Shaders build HLSL

-- https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst

-- Engine distributed DXC location
filter "system:windows"
    -- For Windows: Always use engine DXC, but use VulkanSDK SPIRV-Cross
    dxcLocation = "%{wks.location}../Engine/Content/Shaders/Tools/dxc/bin/x64/"
    VulkanSDK = os.getenv("VULKAN_SDK")
    if VulkanSDK and VulkanSDK ~= "" then
        spirvCrossLocation = VulkanSDK .. "/Bin/spirv-cross"
    else
        -- Fallback to system PATH if VulkanSDK not available
        spirvCrossLocation = "spirv-cross"
    end
filter "system:macosx"
    VulkanSDK = os.getenv("VULKAN_SDK")
    dxcLocation = "%{VulkanSDK}/bin/"
    spirvCrossLocation = "spirv-cross"
filter {}
-- Note: All shaders are built using SM6

-- TODO: Add as rules, every shader file type will have it's own rule
-- TODO: customize per build config, release/distr shaders will need signing and extra optimization flags (-O3 etc.)
-- Don't build the these shader types, they are compiled by the engine once and cached/include only
filter { "files:**.glsl or **.hlsl or **.pssl or **.cg or **.rzsf"}
    flags { "ExcludeFromBuild" }
filter {}

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
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage vert --entry VS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage vert --entry VS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
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
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage frag --entry PS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage frag --entry PS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
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
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage comp --entry CS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage comp --entry CS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
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
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --stage geom --entry GS_MAIN -V --output "%{wks.location}/../Engine/content/Shaders/Generated/GLSL/%{file.basename}.glsl"',
        -- Generate Reflection Data JSON
        "echo [Generating] exporting shader Reflection data",
        '"%{spirvCrossLocation}" "%{wks.location}/../Engine/content/Shaders/Compiled/SPIRV/%{file.basename}.spv" --reflect --stage geom --entry GS_MAIN --output "%{wks.location}/../Engine/content/Shaders/Generated/ReflectionData/%{file.basename}.json"',
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