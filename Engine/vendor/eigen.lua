project "Eigen"
    kind "None"
    language "C++"
    cppdialect (engine_global_config.cpp_dialect)
    staticruntime "off"

    files 
    {
    "eigen/Eigen/**",
            "eigen/Eigen/src/**.h"
    }

    includedirs
    {
        "eigen/Eigen",
    }

    externalincludedirs
    {
            "eigen/Eigen",
    }
