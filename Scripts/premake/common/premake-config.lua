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
    trigger     = "memtrack",
    value       = "state",
    description = "Enable Razix memory allocation tracking",
    allowed     =
    {
        { "on",  "Enable memory allocation tracking" },
        { "off", "Disable memory allocation tracking" }
    }
}

newoption
{
	trigger     = "sanitizer",
	value       = "sanitizer",
	description = "Enable Razix sanitizers (Razix project only)",
	allowed     =
	{
		{ "off",        "Disable sanitizers" },
		{ "asan",       "AddressSanitizer" },
		{ "ubsan",      "UndefinedBehaviorSanitizer" },
		{ "tsan",       "ThreadSanitizer" },
		{ "asan-ubsan", "AddressSanitizer + UndefinedBehaviorSanitizer" }
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

newaction {
    trigger     = "rungfxtests",
    description = "Runs all Gfx test executables",
    execute     = function ()
        os.execute("python Scripts/run_gfx_tests.py")
    end
}

newoption
{
    trigger     = "buildmode",
    value       = "buildmode",
    description = "Choose a build mode for shipping the game/engine (Development, Shipping)",
    allowed     =
    {
        { "development", "Development build with debug symbols, devutils and optimizations" },
        { "shipping", "Shipping build with optimizations and no debug symbols" }
    }
}
