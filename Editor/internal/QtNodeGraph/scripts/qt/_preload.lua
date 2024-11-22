
--
-- To avoid qt.lua re-including _preload
--
premake.extensions.qt = true


--
-- Set the path where Qt is installed
--
premake.api.register {
	name = "qtpath",
	scope = "config",
	kind = "path",
	tokens = true
}

--
-- Set the binary path. By default, its `qtpath .. "/bin"`. Use
-- this command to override it.
--
premake.api.register {
	name = "qtbinpath",
	scope = "config",
	kind = "path",
	tokens = true
}

--
-- Set the include path. By default, its `qtpath .. "/include"`. Use
-- this command to override it.
--
premake.api.register {
	name = "qtincludepath",
	scope = "config",
	kind = "path",
	tokens = true
}

--
-- Set the library path. By default, its `qtpath .. "/lib"`. Use
-- this command to override it.
--
premake.api.register {
	name = "qtlibpath",
	scope = "config",
	kind = "path",
	tokens = true
}

--
-- Set the prefix of the libraries ("Qt4" or "Qt5" usually)
--
-- Note: now by default it's created from the major_version string passed to the enable
-- function. So if you don't use qtprefix, by default it'll be "Qt5", and if you used
-- premake.extensions.qt.enable("6") then it'll be "Qt6". This is left here for backward
-- compatibility and corner cases, but should no longer be needed in most cases.
--
premake.api.register {
	name = "qtprefix",
	scope = "config",
	kind = "string"
}

--
-- Set a suffix for the libraries ("d" usually when using Debug Qt libs)
--
premake.api.register {
	name = "qtsuffix",
	scope = "config",
	kind = "string"
}

--
-- Link the qtmain lib on Windows.
--
-- Note: On Qt6, this will no longer work. Instead, the "entrypoint" module should be added.
-- Using this will automatically add said module (for backward compatibility)
--
premake.api.register {
	name = "qtmain",
	scope = "config",
	kind = "boolean"
}

--
-- Specify the modules to use (will handle include paths, links, etc.)
-- See premake.extensions.qt.modules for a list of available modules.
--
premake.api.register {
	name = "qtmodules",
	scope = "config",
	kind = "string-list"
}

--
-- Specify the path, relative to the current script, where the files generated
-- by Qt will be created. If this command is not used, the default behavior
-- is to generate those files in the objdir.
--
premake.api.register {
	name = "qtgenerateddir",
	scope = "config",
	kind = "path",
	tokens = true
}

--
-- Specify a list of custom options to send to the Qt moc command line.
--
premake.api.register {
	name = "qtmocargs",
	scope = "config",
	kind = "string-list"
}

--
-- Specify a list of custom options to send to the Qt uic command line.
--
premake.api.register {
	name = "qtuicargs",
	scope = "config",
	kind = "string-list"
}

--
-- Specify a list of custom options to send to the Qt rcc command line.
--
premake.api.register {
	name = "qtrccargs",
	scope = "config",
	kind = "string-list"
}

--
-- Specify the version of Qt.
-- This is used to determine the private header path when adding private modules.
-- If unspecified, the addon will scan `qtincludepath .. "/QtCore/qconfig.h"` and `qglobal.h`
-- for the version string.
--
premake.api.register {
	name = "qtversion",
	scope = "config",
	kind = "string"
}

--
-- This command is used to tell Qt tools to store their command line arguments
-- in a file if the size of the command line is greater than the limit
--
premake.api.register {
	name = "qtcommandlinesizelimit",
	scope = "config",
	kind = "integer"
}

--
-- Private use only : used by the addon to know if qt has already been enabled or not
--
premake.api.register {
	name = "qtenabled",
	scope = "project",
	kind = "boolean"
}


--
-- Always load
--
return function () return true end
