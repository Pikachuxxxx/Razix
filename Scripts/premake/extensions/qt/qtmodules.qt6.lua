
--
-- The available modules' definitions.
-- Use the keys with qtmodule. For instance `qtmodule { "core", "gui" }` to use QtCore and QtGui
--
premake.extensions.qt.modules.qt6 = {
	core = {
		name = "Core",
		include = "QtCore",
		defines = { "QT_CORE_LIB" }
	},
	entrypoint = {
		name = "EntryPoint",
		defines = { "QT_ENTRY_POINT_LIB" }
	},
	gui = {
		name = "Gui",
		include = "QtGui",
		defines = { "QT_GUI_LIB" }
	},
	multimedia = {
		name = "Multimedia",
		include = "QtMultimedia",
		defines = { "QT_MULTIMEDIA_LIB" }
	},
	network = {
		name = "Network",
		include = "QtNetwork",
		defines = { "QT_NETWORK_LIB" }
	},
	opengl = {
		name = "OpenGL",
		include = "QtOpenGL",
		links = { "OpenGL32" },
		defines = { "QT_OPENGL_LIB" }
	},
	positioning = {
		name = "Positioning",
		include = "QtPositioning",
		defines = { "QT_POSITIONING_LIB" }
	},
	printsupport = {
		name = "PrintSupport",
		include = "QtPrintSupport",
		defines = { "QT_PRINTSUPPORT_LIB" }
	},
	qml = {
		name = "Qml",
		include = "QtQml",
		defines = { "QT_QML_LIB" }
	},
	quick = {
		name = "Quick",
		include = "QtQuick",
		defines = { "QT_QUICK_LIB" }
	},
	quickcontrols2 = {
		name = "QuickControls2",
		include = "QtQuickControls2",
		defines = { "QT_QUICKCONTROLS2_LIB" }
	},
	sensors = {
		name = "Sensors",
		include = "QtSensors",
		defines = { "QT_SENSORS_LIB" }
	},
	sql = {
		name = "Sql",
		include = "QtSql",
		defines = { "QT_SQL_LIB" }
	},
	svg = {
		name = "Svg",
		include = "QtSvg",
		defines = { "QT_SVG_LIB" }
	},
	testlib = {
		name = "Test",
		include = "QtTest",
		defines = { "QT_TESTLIB_LIB" }
	},
	websockets = {
		name = "WebSockets",
		include = "QtWebSockets",
		defines = { "QT_WEBSOCKETS_LIB" }
	},
	widgets = {
		name = "Widgets",
		include = "QtWidgets",
		defines = { "QT_WIDGETS_LIB" }
	},
	xml = {
		name = "Xml",
		include = "QtXml",
		defines = { "QT_XML_LIB" }
	}
}
