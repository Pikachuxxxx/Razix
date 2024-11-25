include 'Scripts/premake/common/internal_includes.lua'

group "Tests"
    -- TODO: Hello Triangle Test - SS test buy comparing to Golden Imags
    --include "HelloTriangle/test_hello_triangle.lua"
    -- Engine Tests
    group "Tests/EngineTests"
        include "EngineTests/engine_tests.lua"
    group ""

    -- Tests Vendor
    group "Tests/vendor"
        include "vendor/googletest/googletest.lua"
    group ""

group ""