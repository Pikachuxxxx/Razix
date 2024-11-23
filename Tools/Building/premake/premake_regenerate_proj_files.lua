project "Premake Re-Generate Project Files"
	kind "Utility"

	excludes { "%{wks.location}../../../.modules/**premake5.lua" }

	files
	{
		"**.lua",
		"**premake5.lua",
		"%{wks.location}../../**premake5.lua",
		"%{wks.location}../../**.lua" -- This works for different names
	}

	postbuildmessage "Regenerating project files with Premake5!"

	postbuildcommands
	{
		"\"%{wks.location}../Tools/Building/premake/premake5\" %{_ACTION} --file=\"%{wks.location}../premake5.lua\""
	}
