project "Premake"
	kind "Utility"

	excludes { "%{wks.location}../../../.modules/**premake5.lua" }

	files
	{
		"**.lua",
		"**premake5.lua",
		"%{wks.location}../../**premake5.lua"
	}

	postbuildmessage "Regenerating project files with Premake5!"

	postbuildcommands 
	{
		"\"%{wks.location}../Tools/Build/premake/premake5\" %{_ACTION} --file=\"%{wks.location}../premake5.lua\""
	}