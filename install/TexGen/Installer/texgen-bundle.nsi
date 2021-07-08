; Set the current version, this should change at each release
!define VERSION "3.12.0"
!include "LogicLib.nsh"
!include "MUI.nsh"

; The name of the installer
Name "TexGen ${VERSION}"

; The file to write
OutFile "texgen-bundle-${VERSION}.exe"

; TexGen version
VIProductVersion "${VERSION}"

; License file
; LicenseData "..\gpl.txt"

; The default installation directory
InstallDir $PROGRAMFILES\TexGen

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\TexGen" "Install_Dir"


; Pages

!insertmacro MUI_PAGE_LICENSE "..\gpl.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
;--------------------------------

; The stuff to install
Section "TexGen (required)" ;No components page, name is not important

  SectionIn RO
  
  SetOutPath $INSTDIR

  File TexGenGUI.exe
  File TexGenCore.dll
  File msvcp71.dll
  File msvcr71.dll
  File python25.dll
  File ..\GUI\TexGen.xrc
  File ..\Docs\TexGen.chm

  SetOutPath $INSTDIR\Python\libstd

  ; This should contain all the standard python libraries (from the python package)
  File /r Lib\*.*

  SetOutPath $INSTDIR\Python\libxtra\TexGen

  File ..\Python\__init__.py
  File ..\Python\Lib\Abaqus.py
  File ..\Python\Lib\Ansys.py
  File ..\Python\Lib\Completer.py
  File ..\Python\Lib\FlowTex.py
  File ..\Python\Lib\GridFile.py
  File ..\Python\Lib\TexGenv2.py
  File ..\Python\Lib\WiseTex.py
  File ..\Python\Core.py
  File ..\Python\Renderer.py
  File ..\Python\Export.py
  File _Core.pyd
  File _Renderer.pyd
  File _Export.pyd
  File TexGenCore.dll
  File TKBO.DLL
  File TKBOOL.DLL
  File TKBREP.DLL
  File TKERNEL.DLL
  File TKG2D.DLL
  File TKG3D.DLL
  File TKGEOMALGO.DLL
  File TKGEOMBASE.DLL
  File TKIGES.DLL
  File TKMATH.DLL
  File TKOFFSET.DLL
  File TKPRIM.DLL
  File TKSHHEALING.DLL
  File TKSTEP.DLL
  File TKSTEP209.DLL
  File TKSTEPATTR.DLL
  File TKSTEPBASE.DLL
  File TKTOPALGO.DLL
  File TKXSBASE.DLL
  File msvcp71.dll
  File msvcr71.dll


  SetOutPath $INSTDIR\Scripts

  File ..\Python\Scripts\2dweave.py
  File ..\Python\Scripts\3dweave.py
  File ..\Python\Scripts\cotton.py
  File ..\Python\Scripts\example.py
  File ..\Python\Scripts\polyester.py

  SetOutPath $INSTDIR\Data

  File Data\2dweave.tg3
  File Data\3dweave.tg3
  File Data\cotton.tg3
  File Data\polyester.tg3

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\TexGen "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TexGen" "DisplayName" "TexGen"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TexGen" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TexGen" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TexGen" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR   ; Sets the working directory for the links

  CreateDirectory "$SMPROGRAMS\TexGen"
  CreateShortCut "$SMPROGRAMS\TexGen\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\TexGen\TexGen.lnk" "$INSTDIR\TexGenGUI.exe" "" "$INSTDIR\TexGenGUI.exe" 0
  CreateShortCut "$SMPROGRAMS\TexGen\TexGen API Documentation.lnk" "$INSTDIR\TexGen.chm" "" "$INSTDIR\TexGen.chm" 0

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Find out where the TexGen extensions where installed
  ReadRegStr $INSTDIR HKLM SOFTWARE\TexGen "Install_Dir"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TexGen"
  DeleteRegKey HKLM "SOFTWARE\TexGen"

  ; Remove files and uninstaller
  Delete $INSTDIR\Python\libstd\*.*

  Delete $INSTDIR\Python\libxtra\TexGen\__init__.py
  Delete $INSTDIR\Python\libxtra\TexGen\Abaqus.py
  Delete $INSTDIR\Python\libxtra\TexGen\Ansys.py
  Delete $INSTDIR\Python\libxtra\TexGen\Completer.py
  Delete $INSTDIR\Python\libxtra\TexGen\FlowTex.py
  Delete $INSTDIR\Python\libxtra\TexGen\GridFile.py
  Delete $INSTDIR\Python\libxtra\TexGen\TexGenv2.py
  Delete $INSTDIR\Python\libxtra\TexGen\WiseTex.py
  Delete $INSTDIR\Python\libxtra\TexGen\Core.py
  Delete $INSTDIR\Python\libxtra\TexGen\Renderer.py
  Delete $INSTDIR\Python\libxtra\TexGen\Export.py

  Delete $INSTDIR\Python\libxtra\TexGen\Abaqus.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\Ansys.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\Completer.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\FlowTex.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\GridFile.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\TexGenv2.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\WiseTex.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\Core.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\Renderer.pyc
  Delete $INSTDIR\Python\libxtra\TexGen\Export.pyc
        
  Delete $INSTDIR\Python\libxtra\TexGen\_Core.pyd
  Delete $INSTDIR\Python\libxtra\TexGen\_Renderer.pyd
  Delete $INSTDIR\Python\libxtra\TexGen\_Export.pyd
  Delete $INSTDIR\Python\libxtra\TexGen\TexGenCore.dll
        
  Delete $INSTDIR\Python\libxtra\TexGen\TKBO.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKBOOL.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKBREP.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKERNEL.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKG2D.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKG3D.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKGEOMALGO.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKGEOMBASE.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKIGES.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKMATH.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKOFFSET.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKPRIM.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKSHHEALING.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKSTEP.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKSTEP209.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKSTEPATTR.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKSTEPBASE.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKTOPALGO.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\TKXSBASE.DLL
  Delete $INSTDIR\Python\libxtra\TexGen\msvcp71.dll
  Delete $INSTDIR\Python\libxtra\TexGen\msvcr71.dll

  Delete $INSTDIR\TexGenGUI.exe
  Delete $INSTDIR\TexGenCore.dll
  Delete $INSTDIR\TexGen.xrc
  Delete $INSTDIR\msvcp71.dll
  Delete $INSTDIR\msvcr71.dll

  Delete $INSTDIR\Scripts\2dweave.py
  Delete $INSTDIR\Scripts\3dweave.py
  Delete $INSTDIR\Scripts\cotton.py
  Delete $INSTDIR\Scripts\example.py
  Delete $INSTDIR\Scripts\polyester.py
  
  Delete $INSTDIR\Data\2dweave.tg3
  Delete $INSTDIR\Data\3dweave.tg3
  Delete $INSTDIR\Data\cotton.tg3
  Delete $INSTDIR\Data\polyester.tg3

  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\TexGen\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\TexGen"
  RMDir "$INSTDIR\Python\libxtra\TexGen"
  RMDir "$INSTDIR\Python\libstd"
  RMDir "$INSTDIR\Python\libxtra"
  RMDir "$INSTDIR\Python"
  RMDir "$INSTDIR\Scripts"
  RMDir "$INSTDIR\Data"
  RMDir "$INSTDIR"

SectionEnd
