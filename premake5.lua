workspace "Razix"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Razix"
	location "Razix"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"RZX_PLATFORM_WINDOWS",
			"RZX_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

		filter "configurations:Debug"
			defines "RZX_DEBUG"
			symbols "On"

		filter "configurations:Release"
			defines "RZX_RELEASE"
			optimize "On"

		filter "configurations:Dist"
			defines "RZX_DIST"
			symbols "Off"
			optimize "Full"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Razix/vendor/spdlog/include",
		"Razix/src"
	}

	links
	{
		"Razix"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"RZX_PLATFORM_WINDOWS"
		}

		filter "configurations:Debug"
			defines "RZX_DEBUG"
			symbols "On"

		filter "configurations:Release"
			defines "RZX_RELEASE"
			optimize "On"

		filter "configurations:Dist"
			defines "RZX_DIST"
			symbols "Off"
			optimize "Full"