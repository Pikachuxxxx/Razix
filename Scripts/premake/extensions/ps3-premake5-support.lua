function apply_base_settings()
   kind "ConsoleApp"
   language "C++"
   files { "**.h", "**.cpp", "External/*.c", "External/*.h", "External/*.cpp", "External/json/**" }
   excludes { "External/libRocket/**" }
   includedirs {".", "./External", "./External/libRocket/Include"}
end

function apply_spu_settings(dir, symbol_name)
   kind "ConsoleApp"
   language "C++"
   defines { "SN_TARGET_PS3_SPU","_DEBUG","__GCC__","SPU","__CELL_ASSERT__" }
   targetdir "../Build/Spu/"
   -- Apparently we need to build with Symbols because otherwise premake adds a "-s" flag to the linker
   flags {"Symbols"}
   files { dir.."/**.h", dir.."/**.cpp", dir.."/**.c" }
   includedirs {
      ".",
      "$(SN_PS3_PATH)/spu/include/sn",
      "$(SCE_PS3_ROOT)/target/spu/include",
      "$(SCE_PS3_ROOT)/target/common/include",
   }
   libdirs {
      "$(SN_PS3_PATH)/spu/lib/sn",
      "$(SCE_PS3_ROOT)/target/spu/lib/pic/",
   }
   links {
      "-ldma",
      "-lspurs_jq",
      "-latomic",
      "-lsync2",
      "-lfiber"
   }
   linkoptions {
      "-fpic",
      "-mspurs-job-initialize",
      "-Wl,-q",
      "-Wl,--gc-sections",
      "-fstack-check"
   }
   buildoptions {
      "-g",
      "-Wall",
      "-fno-exceptions",
      "-fno-rtti",
      "-ffunction-sections",
      "-fdata-sections",
      "-fpic",
      "-mspurs-job-initialize"
   }
   postbuildcommands {
      "spu_elf-to-ppu_obj --verbose --symbol-name="..symbol_name.." --format=jobbin2 \"$(TargetPath)\" \"$(OutDir)/$(TargetName).ppu.obj\""
   }
end

spu_jobs = {}

function link_spu(name, symbol_name)
   links {
      name..".SPU",
      "Spu/"..name..".SPU.ppu.obj"
   }

   --spu_jobs:Append({name, symbol_name})
   table.insert(spu_jobs, {n=name, s=symbol_name})
end

function projects_for_spus()
   table.foreach(spu_jobs, function(k, v) project(v.n .. ".SPU") apply_spu_settings("Sunshine.PS3/" .. v.n, v.s) end)
end

-- create a new SPU toolchain as subset of the gcc toolchain
function newplatform(plf)
    local name = plf.name
    local description = plf.description

    -- Register new platform
    premake.platforms[name] = {
        cfgsuffix = "_"..name,
        iscrosscompiler = true
    }

    -- Allow use of new platform in --platfroms
    table.insert(premake.option.list["platform"].allowed, { name, description })
    table.insert(premake.fields.platforms.allowed, name)

    -- Add compiler support
    -- gcc
    premake.gcc.platforms[name] = plf.gcc
    --other compilers (?)
end

function newgcctoolchain(toolchain)
    newplatform {
        name = toolchain.name,
        description = toolchain.description,
        gcc = {
            cc = toolchain.prefix .. "g++",
            cxx = toolchain.prefix .. "g++",
            ar = toolchain.prefix .. "ar",
            cppflags = "-MMD " .. toolchain.cppflags,
        }
    }
end

newgcctoolchain {
   name = "spu",
   description = "SPU",
   prefix = "spu-lv2-",
   cppflags = ""
}

-- Windows builds
solution "Sunshine.Win32"
   configurations { "Debug", "Release" }
   platforms {"native", "x32"}
   location "../Build/"

   project "Sunshine.Win32"
      apply_base_settings()

      buildoptions {"/wd4324", "/wd4127", "/wd4800", "/wd4232", "/wd4996"}
      flags {"ExtraWarnings", "FatalWarnings", "Unicode"}
      excludes {"Sunshine.PS3/**", "**/PS3/**"}
      defines {"_WIN32", "_CONSOLE", "NOMINMAX", "_UNICODE", "UNICODE"}
      libdirs {"External/libRocket/bin"}
      links { "kernel32", "RocketCore", "RocketControls", "RocketDebugger" }

      configuration "Debug"
         defines { "_DEBUG" }
         flags { "Symbols" }
         targetsuffix "_d"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "Symbols", "EnableSSE", "EnableSSE2", "FloatFast" }

-- Playstation 3 builds
solution "Sunshine.PS3"
   configurations { "Debug", "Release" }
   platforms {"ps3" }
   location "../Build/"

   project "Sunshine.PS3"
      apply_base_settings()

      includedirs {
         "External/libRocket/Include/",
         "External/Fmod/inc",
         "Sunshine.PS3",
         "$(SN_PS3_PATH)/ppu/include/sn",
         "$(SCE_PS3_ROOT)/target/ppu/include",
         "$(SCE_PS3_ROOT)/target/common/include",
      }
      excludes {"Sunshine.Win32/**", "Sunshine.PS3/**", "**/Win32/**"}
      defines { "SN_TARGET_PS3", "_DEBUG", "__GCC__", "__CELL_ASSERT__"}
      libdirs {
         "External/Fmod/lib", "$(SN_PS3_PATH)/ppu/lib/sn",
         "$(SCE_PS3_ROOT)/target/ppu/lib", "External/libRocket/bin",
      }
      links {
         "-lfmodexL", "-lsn", "-lm", "-lio_stub", "-lfs_stub", "-lgcm_pm",
         "-lnet_stub", "-lrtc_stub", "-lnetctl_stub", "-lspurs_stub",
         "-lsysutil_stub", "-laudio_stub", "-lsysutil_avconf_ext_stub",
         "-lfios", "-lsysutil_np_stub", "-lmic_stub", "-lpadfilter",
         "-lgcm_cmd", "-lgcm_sys_stub", "-lsysmodule_stub", "-ldbgfont_gcm",
         "-lcgc", "-lgcm_gpad_stub", "-lRocketps3", "-lRocketControlsps3",
         "-lRocketDebuggerps3", "-lstdc++", "-lfreetype", "-lspurs_stub",
         "-lspurs_jq_stub", "-lsntuner", "-lsync2_stub",
      }

      link_spu("OcclusionTriangleRasterizerJob", "occlusion_triangle_setup_job")
      link_spu("SoftwareOcclusionCullingJob", "occlusion_triangle_rasterizer_job")

   -- Setup projects for SPU jobs
   projects_for_spus()
