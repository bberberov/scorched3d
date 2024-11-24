; Helper defines
!define PRODUCT_NAME "Scorched3D"
!define PRODUCT_VERSION "44"
!define PRODUCT_PUBLISHER "Scorched"
!define PRODUCT_WEB_SITE "http://www.scorched3d.co.uk"
!define PRODUCT_DONATE_WEB_SITE "https://www.paypal.com/xclick/business=donations%40scorched3d.co.uk&item_name=Scorched3D&no_note=1&tax=0&currency_code=GBP"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

RequestExecutionLevel admin

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "scripts\FileAssociation.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "data\images\tank2.ico"
!define MUI_UNICON "data\images\tank2.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "COPYING"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
!insertmacro MUI_RESERVEFILE_LANGDLL

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Scorched3D-${PRODUCT_VERSION}-patch.exe"
InstallDir "$PROGRAMFILES\Scorched3D"
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  ;Language selection dialog
  Push ""
  Push ${LANG_ENGLISH}
  Push English
  Push ${LANG_RUSSIAN}
  Push Russian
  Push A ; A means auto count languages
         ; for the auto count to work the first empty push (Push "") must remain
  LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

  Pop $LANGUAGE
  StrCmp $LANGUAGE "cancel" 0 +2
    Abort

  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" 0 done
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "${PRODUCT_NAME} is not already installed, do you wish to patch anyway?" IDOK done
  Abort
done:

FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try

File "/oname=$INSTDIR\.\data\globalmods\apoc\data\landscapes\ambientsoundhelicopter.xml" ".\data\globalmods\apoc\data\landscapes\ambientsoundhelicopter.xml"
File "/oname=$INSTDIR\.\data\globalmods\apoc\data\modinfo.xml" ".\data\globalmods\apoc\data\modinfo.xml"
File "/oname=$INSTDIR\.\data\globalmods\none\data\landscapes\boidsbats.xml" ".\data\globalmods\none\data\landscapes\boidsbats.xml"
File "/oname=$INSTDIR\.\data\globalmods\none\data\landscapes.xml" ".\data\globalmods\none\data\landscapes.xml"
File "/oname=$INSTDIR\.\data\globalmods\none\data\modinfo.xml" ".\data\globalmods\none\data\modinfo.xml"
File "/oname=$INSTDIR\.\data\keys.xml" ".\data\keys.xml"
File "/oname=$INSTDIR\.\scorched.exe" ".\scorched.exe"
File "/oname=$INSTDIR\.\scorchedc.exe" ".\scorchedc.exe"
File "/oname=$INSTDIR\.\scorcheds.exe" ".\scorcheds.exe"
File "/oname=$INSTDIR\.\uninst.exe" ".\uninst.exe"

  ${registerExtension} "$INSTDIR\scorchedc.exe" ".s3l" "Scorched3D_Launch"

  FileOpen $9 "$INSTDIR\data\lang\language.ini" w
  StrCmp $LANGUAGE ${LANG_ENGLISH} 0 +2
    FileWrite $9 "EN"
  StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +2
    FileWrite $9 "RU"
  FileClose $9
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}-docs.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}\wiki"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}-donate.url" "InternetShortcut" "URL" "${PRODUCT_DONATE_WEB_SITE}"

  CreateDirectory "$SMPROGRAMS\Scorched3D"

  CreateShortCut "$SMPROGRAMS\Scorched3D\Uninstall Scorched3D.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D.lnk" "$INSTDIR\scorched.exe" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D Documentation.lnk" "$INSTDIR\${PRODUCT_NAME}-docs.url" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D Homepage.lnk" "$INSTDIR\${PRODUCT_NAME}.url" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\Scorched3D\Scorched3D Donations.lnk" "$INSTDIR\${PRODUCT_NAME}-donate.url" "" "$INSTDIR\data\images\tank2.ico"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Var DEL_USER
Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES remove
  Abort
remove:

  StrCpy $DEL_USER "FALSE"
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you want to remove all $(^Name) user data.  Warning: this includes STATS, saved games, mods etc?" IDNO nodel
  StrCpy $DEL_USER "TRUE";
nodel:
FunctionEnd

Section Uninstall

  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\documentation"
  RMDir /r "$SMPROGRAMS\Scorched3D"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true

  ${unregisterExtension} ".s3l" "Scorched3D_Launch"

  StrCmp $DEL_USER "FALSE" nodel
  RMDir /r "$INSTDIR\.scorched3d"
  RMDir /r "$PROFILE\.scorched3d"
nodel:

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

SectionEnd
