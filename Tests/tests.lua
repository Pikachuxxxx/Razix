include 'Scripts/premake/common/internal_includes.lua'

group "Tests"
    
    project "GfxTestRunner"
        kind "Utility"
        language "C++" -- or just omit if not compiling anything
        location "GfxTestRunner"

        postbuildcommands {
            "python ../../Scripts/test_runner.py Debug"
        }

    -- TODO: Hello Triangle Test - SS test buy comparing to Golden Imags
    --include "HelloTriangle/test_hello_triangle.lua"
    -- Engine Tests
    group "Tests/EngineTests"
        include "EngineTests/engine_tests.lua"
    group ""

    group "Tests/GfxTests"
        include "GfxTests/gfx_tests.lua"
    group ""

    -- Tests Vendor
    group "Tests/vendor"
        include "vendor/googletest/googletest.lua"
    group ""

group ""

