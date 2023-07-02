;NSIS Modern User Interface
; Razix Engine and Tools Installer
;--------------------------------
;Include Modern UI

    !include "MUI2.nsh"

;--------------------------------
;General

    ;Name and file
    Name "Razix Engine"
    OutFile "RazixEngineInstaller-V.0.42.0.Dev.exe"
    Unicode True
    ShowInstDetails show

    ;Default installation folder
    InstallDir "$PROGRAMFILES64\Razix"

    ;Request application privileges for Windows 11
    RequestExecutionLevel admin
;--------------------------------
;Interface Settings

    !define MUI_ABORTWARNING
    !define MUI_ICON "../../../Engine/content/Logos/RazixLogo32.ico"
    !define MUI_FINISHPAGE_SHOWREADME ""
    !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
    !define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
    !define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction
;--------------------------------
;Pages

    Function finishpageaction
    CreateShortcut "$desktop\RazixEditor.lnk" "$INSTDIR\RazixEditor.exe"
    FunctionEnd

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_LICENSE "../../../LICENSE"
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES
    !insertmacro MUI_PAGE_FINISH

    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES
    !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Razix Engine Components" EngineInstallSec

    SetOutPath $INSTDIR

    FILE /a /r "..\..\..\bin\Distribution-windows-x86_64\Razix.dll"
    SetOutPath $INSTDIR
    FILE /a /r "..\..\..\bin\Distribution-windows-x86_64\RazixEditor.exe"
    FILE /a /r "..\..\..\bin\Distribution-windows-x86_64\Qt5Core.dll"
    FILE /a /r "..\..\..\bin\Distribution-windows-x86_64\Qt5Gui.dll"
    FILE /a /r "..\..\..\bin\Distribution-windows-x86_64\Qt5Widgets.dll"
    SetOutPath $INSTDIR\plugins
    FILE /nonfatal /a /r "..\..\..\bin\Distribution-windows-x86_64\plugins\"
    SetOutPath $INSTDIR\Engine\content
    FILE /nonfatal /a /r "..\..\..\Engine\content\"

    SetOutPath $INSTDIR\Engine\content\config
    FileOpen $0 "$INSTDIR\Engine\content\config\razix_engine.config" w
    FileWrite $0 "installation_dir=$INSTDIR"
    FileClose $0
    SetOutPath $INSTDIR

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Razix Uninstaller.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.exe"

  Delete "$INSTDIR\Razix Uninstaller.exe"
  Delete "$DESKTOP\RazixEditor.lnk"
  RMDir /r "$INSTDIR\plugins\"
  RMDir /r "$INSTDIR\Engine\"

  RMDir "$INSTDIR"

SectionEnd
