; Helper defines
!define PRODUCT_NAME "Scorched3D"
!define PRODUCT_VERSION "44.2"
!define PRODUCT_PUBLISHER "Scorched"
!define PRODUCT_WEB_SITE "http://www.scorched3d.co.uk"
!define PRODUCT_DONATE_WEB_SITE "https://www.paypal.com/xclick/business=donations%40scorched3d.co.uk&item_name=Scorched3D&no_note=1&tax=0&currency_code=GBP"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

RequestExecutionLevel admin

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI2.nsh"
!include "scripts\FileAssociation.nsh"

!include "${NSISDIR}\Contrib\Modern UI\System.nsh"

; MUI Settings
!define MUI_ICON "data\images\tank2.ico"
!define MUI_UNICON "data\images\tank2.ico"

;Modern UI Configuration
!define MUI_WELCOMEPAGE  
!define MUI_DIRECTORYPAGE
!define MUI_ABORTWARNING
!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
!define MUI_FINISHPAGE  
!define MUI_FINISHPAGE_TEXT "Thank you for installing Scorched3D.  If you like Scorched3D please visit our website."
!define MUI_FINISHPAGE_LINK "www.scorched3d.co.uk"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.scorched3d.co.uk"
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\scorched-dep-win32\installer\wizard.bmp"
!define MUI_HEADERIMAGE 
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp"


!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!define MUI_PAGE_CUSTOMFUNCTION_SHOW un.ModifyUnWelcome
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE un.LeaveUnWelcome

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
ShowInstDetails "nevershow"
ShowUnInstDetails "nevershow"

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try

  RMDir /r "$INSTDIR\data"

  File /r /x ".svn" /x "*.db" "data"
  File /r /x ".svn" /x "*.db" "documentation"
  File "*.dll"
  File "*.manifest"
  File "README"
  File "CHANGELOG"
  File "Release\scorched.exe"
  File "Release\scorchedc.exe"
  File "Release\scorcheds.exe"

  ${registerExtension} "$INSTDIR\scorchedc.exe" ".s3l" "Scorched3D_Launch"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}-docs.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}\wiki"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}-donate.url" "InternetShortcut" "URL" "${PRODUCT_DONATE_WEB_SITE}"

  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"

  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall Scorched3D.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Scorched3D.lnk" "$INSTDIR\scorched.exe" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Scorched3D Documentation.lnk" "$INSTDIR\${PRODUCT_NAME}-docs.url" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Scorched3D Homepage.lnk" "$INSTDIR\${PRODUCT_NAME}.url" "" "$INSTDIR\data\images\tank2.ico"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Scorched3D Donations.lnk" "$INSTDIR\${PRODUCT_NAME}-donate.url" "" "$INSTDIR\data\images\tank2.ico"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Var mycheckbox ; You could just store the HWND in $1 etc if you don't want this extra v
Function un.ModifyUnWelcome
${NSD_CreateCheckbox} 120u -30u 50% 24u "Remove all user data.  Warning: this includes STATS, saved games, mods etc?"
Pop $mycheckbox
SetCtlColors $mycheckbox "" ${MUI_BGCOLOR}
FunctionEnd

Var DEL_USER
Function un.LeaveUnWelcome
${NSD_GetState} $mycheckbox $0
StrCpy $DEL_USER "FALSE"
${If} $0 <> 0
  StrCpy $DEL_USER "TRUE"
${EndIf}
FunctionEnd

Section Uninstall

  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\documentation"
  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true

  ${unregisterExtension} ".s3l" "Scorched3D_Launch"

  StrCmp $DEL_USER "FALSE" nodel
  RMDir /r "$INSTDIR\.${PRODUCT_NAME}"
  RMDir /r "$PROFILE\.${PRODUCT_NAME}"
nodel:

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

SectionEnd
