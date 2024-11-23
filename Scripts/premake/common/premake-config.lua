newoption
{
   trigger     = "renderer",
   value       = "API",
   description = "Choose a renderer",
   allowed =
   {
      { "dx12",  "DirectX 12 (Windows only)" },
      { "vulkan", "Vulkan (Windows, linux, iOS, macOS)" }
   }
}

newoption
{
	trigger = "arch",
	value   = "arch",
	description = "Choose architecture",
	allowed =
	{
		{"x64", "x64 arhcitexture for PC like devices" },
		{"arm", "arm for android/PSVita devices" },
		{"arm64", "arm64 for Apple Silicon devices"}
	}
}

newoption
{
	trigger     = "teamid",
	value	    = "id",
	description = "development team id for apple developers"
}

newaction
{
	trigger     = "clean",
	description = "clean the software",
	execute     = function ()
		print("clean the build...")
		os.rmdir("./build")
		os.rmdir("./bin")
		os.rmdir("./bin-int")
		print("done.")
	end
}
