;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  !include "FileAssociation.nsh"


;--------------------------------
;General

  ;Name and file
  Name "eePe"
  OutFile "eePeInstall.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\eePe"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\er9x-eePe" ""
  
  ;Compressor options
  SetCompressor /FINAL /SOLID lzma
  SetCompressorDictSize 64

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\er9x-eePe" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "eePe" SecDummy

  SetOutPath "$INSTDIR"
  
  File "license.txt"
  File "eepe.exe"
  File "libgcc_s_dw2-1.dll"
  File "mingwm10.dll"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "avrdude.exe"
  File "avrdude.conf"
  
  CreateDirectory "$INSTDIR\lang"
  SetOutPath "$INSTDIR\lang"
  File "*.qm"
  
  
  SetOutPath "$INSTDIR"
  ;Store installation folder
  WriteRegStr HKCU "Software\er9x-eePe" "" $INSTDIR
  
  ;Associate with extentions ,bin and .hex
  ${registerExtension} "$INSTDIR\eepe.exe" ".bin" "BIN_File"
  ${registerExtension} "$INSTDIR\eepe.exe" ".hex" "HEX_File"

  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\eePe.lnk" "$INSTDIR\eePe.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "eePe er9x EEPROM editor."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\eepe.exe"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\avrdude.exe"
  Delete "$INSTDIR\avrdude.conf"
  Delete "$INSTDIR\Uninstall.exe"
  
  Delete "$INSTDIR\lang\*.*"

  RMDir "$INSTDIR\lang"
  RMDir "$INSTDIR"
  
  ${unregisterExtension} ".bin" "BIN File"
  ${unregisterExtension} ".hex" "HEX File"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\eePe.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\er9x-eePe"

SectionEnd