group "Tests"
    
    -- Enable this on demand for ease of use
    --project "GfxTestRunner"
    --    kind "Utility"
    --    language "C++" -- or just omit if not compiling anything
    --
    --    postbuildcommands {
    --        'cd /d "%{wks.location}/../"',
    --        'python "%{wks.location}/../Scripts/test_runner.py" %{cfg.buildcfg}'
    --    }
    --
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

