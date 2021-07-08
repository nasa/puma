; Set the current version, this should change at each release
!define VERSION "3.12.0"
!define PYTHONDLL python25.dll
!define PYTHONVER "2.5"
!include "LogicLib.nsh"
!include "MUI.nsh"

Var PythonDir
Var PythonDLLPath

; The name of the installer
Name "TexGen ${VERSION}"

; The file to write
OutFile "texgen-${VERSION}.exe"

; TexGen version
VIProductVersion "${VERSION}"

; License file
; LicenseData "..\gpl.txt"

; The default installation directory
InstallDir $PROGRAMFILES\TexGen

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\TexGen" "Install_Dir"

; Check that the python dll is present in the system path
Function CheckPython
  ClearErrors
  ReadRegStr $PythonDir HKLM SOFTWARE\Python\PythonCore\${PYTHONVER}\InstallPath ""
  ${If} ${Errors}
    ClearErrors
    ReadRegStr $PythonDir HKCU SOFTWARE\Python\PythonCore\${PYTHONVER}\InstallPath ""
  ${EndIf}

  ${If} ${Errors}
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "Python ${PYTHONVER} doesn't seem to be installed, it is require for TexGen to run! \
                     $\nPlease download it from http://www.python.org/ \
                     $\n$\nContinue with installation anyway?" IDYES yes IDNO no
    no:
      Quit
    yes:
  ${EndIf}
FunctionEnd

Function CopyPythonDLLIfNeeded
  StrCpy $PythonDLLPath "$PythonDir\${PYTHONDLL}"
  ${If} ${FileExists} "$PythonDLLPath"
    CopyFiles /SILENT /FILESONLY "$PythonDLLPath" "$INSTDIR"
  ${Else}
    ClearErrors
    SearchPath $0 ${PYTHONDLL}
    ${If} ${Errors}
      MessageBox MB_YESNO|MB_ICONEXCLAMATION "Unable to find ${PYTHONDLL} in the system path or in $PythonDir. \
                       $\nTexGen is unlikely to function after installation. You may try and locate ${PYTHONDLL} manually \
                       $\nand copy it into the TexGen installation folder. \
                       $\n$\nContinue with installation anyway?" IDYES yes IDNO no
      no:
        Quit
      yes:
    ${EndIf}
  ${EndIf}
FunctionEnd

LangString TEXT_PYTHON_TITLE ${LANG_ENGLISH} "Python Extension Installation"
LangString TEXT_PYTHON_SUBTITLE ${LANG_ENGLISH} "Please locate the directory where Python is installed."

Function PythonPage
  Call CheckPython
  !insertmacro MUI_HEADER_TEXT "$(TEXT_PYTHON_TITLE)" "$(TEXT_PYTHON_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "PythonPage.ini" "Field 2" "State" "$PythonDir"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "PythonPage.ini"
FunctionEnd

; Check on startup
Function .onInit
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "PythonPage.ini"
FunctionEnd

;--------------------------------

; Pages

!insertmacro MUI_PAGE_LICENSE "..\gpl.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Page custom PythonPage ;Python custom page
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
;Page license
;Page components
;Page custom PythonPage
;Page directory
;Page instfiles

;UninstPage uninstConfirm
;UninstPage instfiles

!insertmacro MUI_LANGUAGE "English"
;--------------------------------

; The stuff to install
Section "TexGen (required)" ;No components page, name is not important

  SectionIn RO
  
  !insertmacro MUI_INSTALLOPTIONS_READ $PythonDir "PythonPage.ini" "Field 2" "State"

  Call CopyPythonDLLIfNeeded

  SetOutPath $PythonDir\Lib\site-packages\TexGen
  
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

  SetOutPath $INSTDIR

  File TexGenGUI.exe
  File TexGenCore.dll
  File ..\GUI\TexGen.xrc
  File msvcp71.dll
  File msvcr71.dll
  File ..\Docs\TexGen.chm

  SetOutPath $INSTDIR\Scripts

  File ..\Python\Scripts\2dweave.py
  File ..\Python\Scripts\3dweave.py
  File ..\Python\Scripts\cotton.py
  File ..\Python\Scripts\example.py
  File ..\Python\Scripts\polyester.py

  SetOutPath $INSTDIR\Data

  File ..\Data\2dweave.tg3
  File ..\Data\3dweave.tg3
  File ..\Data\cotton.tg3
  File ..\Data\polyester.tg3

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\TexGen "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM SOFTWARE\TexGen "Python_Dir" "$PythonDir"

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
  ReadRegStr $PythonDir HKLM SOFTWARE\TexGen "Python_Dir"
  ReadRegStr $INSTDIR HKLM SOFTWARE\TexGen "Install_Dir"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TexGen"
  DeleteRegKey HKLM SOFTWARE\TexGen

  ; Remove files and uninstaller
  Delete $PythonDir\Lib\site-packages\TexGen\__init__.py
  Delete $PythonDir\Lib\site-packages\TexGen\Abaqus.py
  Delete $PythonDir\Lib\site-packages\TexGen\Ansys.py
  Delete $PythonDir\Lib\site-packages\TexGen\Completer.py
  Delete $PythonDir\Lib\site-packages\TexGen\FlowTex.py
  Delete $PythonDir\Lib\site-packages\TexGen\GridFile.py
  Delete $PythonDir\Lib\site-packages\TexGen\TexGenv2.py
  Delete $PythonDir\Lib\site-packages\TexGen\WiseTex.py
  Delete $PythonDir\Lib\site-packages\TexGen\Core.py
  Delete $PythonDir\Lib\site-packages\TexGen\Renderer.py
  Delete $PythonDir\Lib\site-packages\TexGen\Export.py

  Delete $PythonDir\Lib\site-packages\TexGen\Abaqus.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\Ansys.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\Completer.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\FlowTex.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\GridFile.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\TexGenv2.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\WiseTex.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\Core.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\Renderer.pyc
  Delete $PythonDir\Lib\site-packages\TexGen\Export.pyc

  Delete $PythonDir\Lib\site-packages\TexGen\_Core.pyd
  Delete $PythonDir\Lib\site-packages\TexGen\_Renderer.pyd
  Delete $PythonDir\Lib\site-packages\TexGen\_Export.pyd
  Delete $PythonDir\Lib\site-packages\TexGen\TexGenCore.dll

  Delete $PythonDir\Lib\site-packages\TexGen\TKBO.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKBOOL.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKBREP.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKERNEL.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKG2D.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKG3D.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKGEOMALGO.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKGEOMBASE.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKIGES.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKMATH.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKOFFSET.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKPRIM.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKSHHEALING.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKSTEP.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKSTEP209.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKSTEPATTR.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKSTEPBASE.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKTOPALGO.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\TKXSBASE.DLL
  Delete $PythonDir\Lib\site-packages\TexGen\msvcp71.dll
  Delete $PythonDir\Lib\site-packages\TexGen\msvcr71.dll

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
  Delete $INSTDIR\${PYTHONDLL}

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\TexGen\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\TexGen"
  RMDir "$PythonDir\Lib\site-packages\TexGen"
  RMDir "$INSTDIR\Scripts"
  RMDir "$INSTDIR\Data"
  RMDir "$INSTDIR"

SectionEnd
