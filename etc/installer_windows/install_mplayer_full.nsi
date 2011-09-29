!define PRODUCT_NAME "MPlayer WW for Windows"
!define PRODUCT_VERSION "0.1.0.0"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_PUBLISHER "MPlayer"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\MPlayer.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

SetCompressor /SOLID lzma
SetCompressorDictSize 32
SetDatablockOptimize on
SetCompress auto

VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey "CompanyName" "MPlayer"
VIAddVersionKey "FileDescription" "MPlayer Installer"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2007-2010"
VIAddVersionKey "FileVersion" ${PRODUCT_VERSION}

ReserveFile "install_files\install_mplayer_full.ini"
ReserveFile "${NSISDIR}\Plugins\LangDLL.dll"
ReserveFile "${NSISDIR}\Plugins\FindProcDLL.dll"

!include "MUI.nsh"
!include "LogicLib.nsh"

!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "${PRODUCT_NAME}"
!define MUI_WELCOMEFINISHPAGE_BITMAP "install_files\Res\wizard.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "install_files\Res\header-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "install_files\Res\header-r-un.bmp"
!define MUI_ICON "install_files\Res\installer.ico"
!define MUI_UNICON "install_files\Res\uninstaller.ico"

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_DIRECTORY

Page custom io goto

!insertmacro MUI_PAGE_INSTFILES

!define MUI_PAGE_CUSTOMFUNCTION_SHOW FinishPageShow
!define MUI_FINISHPAGE_TITLE "${PRODUCT_NAME}"

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\help\changelog.html"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Changelog"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "SimpChinese"
!include "install_files\install_mplayer_lang.nsh"

Name "${PRODUCT_NAME}"
OutFile "mplayer_setup_full.exe"
InstallDir "$PROGRAMFILES\MPlayer"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" "Path"
BrandingText "MPlayer WW Project"
ShowInstDetails show
ShowUnInstDetails show

Section "Section0" SEC0
SetDetailsPrint textonly
DetailPrint $(LANG_IN_CORE_TEXT)
SetDetailsPrint listonly
	SetOutPath "$INSTDIR"
	SetOverwrite on

	IfFileExists "$INSTDIR\mplayer.ini" testMP copyMP
	testMP:
		ReadINIStr $1 "$PLUGINSDIR\install_mplayer_full.ini" "Field 8" "State"
		StrCmp $1 0 0 +2
		File "mplayer\mplayer.ini"
		goto nocpoy0
	copyMP:
		File "mplayer\mplayer.ini"
	nocpoy0:

	IfFileExists "$INSTDIR\kk.ini" testKK copyKK
	testKK:
		ReadINIStr $1 "$PLUGINSDIR\install_mplayer_full.ini" "Field 8" "State"
		StrCmp $1 0 0 +2
		File "mplayer\kk.ini"
		goto nocpoy1
	copyKK:
		File "mplayer\kk.ini"
	nocpoy1:

	IfFileExists "$INSTDIR\input.ini" testIN copyIN
	testIN:
		ReadINIStr $1 "$PLUGINSDIR\install_mplayer_full.ini" "Field 8" "State"
		StrCmp $1 0 0 +2
		File "mplayer\input.ini"
		goto nocpoy2
	copyIN:
		File "mplayer\input.ini"
	nocpoy2:

	File "mplayer\mplayer.exe"
	File "mplayer\avcodec-53.dll"
	File "mplayer\avformat-53.dll"
	File "mplayer\avutil-51.dll"
	File "mplayer\postproc-51.dll"
	File "mplayer\dshownative.dll"
	File "mplayer\pthreadGC2.dll"
	File "mplayer\meditor2.exe"
	File "mplayer\meditor.exe"
	File "mplayer\micons.dll"
	File "mplayer\unrar.dll"
	File "mplayer\swscale-2.dll"

SectionEnd

Section "Section1" SEC01
SetDetailsPrint textonly
DetailPrint $(LANG_IN_CODECS_TEXT)
SetDetailsPrint listonly
SetOverwrite on
	SetOutPath "$INSTDIR\codecs"
	File "mplayer\codecs\*.*"
	File "mplayer_extra_package\codecs\*.*"
SectionEnd

Section "Section00" SEC00
SetDetailsPrint textonly
DetailPrint $(LANG_IN_OTHER_TEXT)
SetDetailsPrint listonly
SetOverwrite on
	SetOutPath "$INSTDIR\codecs"
	File /r "mplayer\codecs\plugins"
	SetOutPath "$INSTDIR"
	File /r "mplayer\tools"
	File /r "mplayer_extra_package\tools"
	File /r "mplayer\skin"
	File /r "mplayer\fonts"
	File /r "mplayer\help"
SectionEnd

Section "Section2" SEC02
SetDetailsPrint textonly
DetailPrint $(LANG_IN_REAL_TEXT)
SetDetailsPrint listonly
	SetOutPath "$INSTDIR\codecs"
	File /r "mplayer_extra_package\codecs\Real"
SetDetailsPrint none
	ReadINIStr $1 "$PLUGINSDIR\install_mplayer_full.ini" "Field 3" "State"
	StrCmp $1 1 0 +2
	ExecWait '"$INSTDIR\meditor.exe" --real-online-reg --real-online-silent'
SectionEnd

Section "Section8" SEC08
SetDetailsPrint both
DetailPrint $(LANG_IN_LINK_TEXT)
SetDetailsPrint none
SetOverwrite on
	ReadINIStr $1 "$PLUGINSDIR\install_mplayer_full.ini" "Field 1" "State"
	StrCmp $1 1 0 +2
	CreateShortCut "$DESKTOP\MPlayer.lnk" "$INSTDIR\MPlayer.exe" "" "" "" "" "" "${PRODUCT_NAME}"

	ReadINIStr $2 "$PLUGINSDIR\install_mplayer_full.ini" "Field 2" "State"
	StrCmp $2 1 0 +2
	CreateShortCut "$QUICKLAUNCH\MPlayer.lnk" "$INSTDIR\MPlayer.exe" "" "" "" "" "" "${PRODUCT_NAME}"
SectionEnd

Section -Post
SetDetailsPrint both
DetailPrint $(LANG_IN_UNIN_TEXT)
SetDetailsPrint none
SetOverwrite on
	WriteUninstaller "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\MPlayer.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "Path" "$INSTDIR"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "UN" "$INSTDIR"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\MPlayer.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Section -AdditionalIcons
SetDetailsPrint both
DetailPrint $(LANG_IN_START_TEXT)
SetDetailsPrint none
SetOverwrite on
SetShellVarContext ALL
	CreateDirectory "$SMPROGRAMS\${PRODUCT_PUBLISHER}"

	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\$(LANG_STR_UNIN_TEXT) MPlayer.lnk" "$INSTDIR\uninstall.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\MPlayer.lnk" "$INSTDIR\MPlayer.exe" "" "" "" "" "" "${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\$(LANG_STR_MEDITOR_TEXT).lnk" "$INSTDIR\meditor2.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\$(LANG_STR_HELP_TEXT).lnk" "$INSTDIR\help\default.html"

	SetOutPath "$INSTDIR\tools"

	CreateDirectory "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools"

	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\MPC-HC.lnk" "$INSTDIR\tools\mplayerc.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\MediaInfo.lnk" "$INSTDIR\tools\MediaInfo.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\$(LANG_STR_SUB_TEXT).lnk" "$INSTDIR\tools\SubDownloader.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\FLV $(LANG_STR_FIX_TEXT).lnk" "$INSTDIR\tools\flvmdigui.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\FLVExtract.lnk" "$INSTDIR\tools\FLVExtract.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\TSSplitter.lnk" "$INSTDIR\tools\TSSplitter.exe"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_PUBLISHER}\Tools\TSMuxer.lnk" "$INSTDIR\tools\tsMuxerGUI.exe"

SectionEnd

Section Uninstall
SetDetailsPrint both
DetailPrint $(LANG_UN_UNIN_TEXT)
SetDetailsPrint none
	Delete "$DESKTOP\MPlayer.lnk"
	Delete "$QUICKLAUNCH\MPlayer.lnk"
	ReadRegDWORD $0 HKLM "SOFTWARE\Classes\rmocx.RealPlayer G2 Control" ""
	ReadRegDWORD $1 HKLM "SOFTWARE\Classes\rmocx.RealPlayer Download Handler" ""
	${if} $1 != ""
	${andif} $0 != ""
	ExecWait '"$INSTDIR\meditor.exe" --real-online-dreg --real-online-silent'
	${endif}

	RMdir /r "$INSTDIR\codecs\Real"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"

SetShellVarContext ALL
  
SetDetailsPrint listonly
  RMDir /r "$SMPROGRAMS\MPlayer\Tools"
  RMDir /r "$SMPROGRAMS\MPlayer"
  RMdir /r "$INSTDIR\codecs\Real"
  RMdir /r "$INSTDIR\codecs"
  RMdir /r "$INSTDIR\tools"
  RMdir /r "$INSTDIR\fonts"
  RMdir /r "$INSTDIR\skin"
  RMdir /r "$INSTDIR\help"
  RMDir /r "$INSTDIR"
SetDetailsPrint none

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true

SectionEnd

Function un.onInit
FindProcDLL::FindProc "MPlayer.exe"
StrCmp $R0 0 +3
   MessageBox MB_YesNo $(LANG_UN_RUN_TEXT) IDYES NoAbort
Abort
NoAbort:

MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 $(LANG_UN_FULL_TEXT) IDYES +2
Abort
;ExecWait "$INSTDIR\codecs\unassos.exe"
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK " ${PRODUCT_NAME} $(LANG_UN_FIN_TEXT)"
  RMDir /r "$PLUGINSDIR"
FunctionEnd

Function FinishPageShow
         GetDlgItem $0 $HWNDPARENT 2
         ShowWindow $0 ${SW_HIDE}
         GetDlgItem $0 $HWNDPARENT 3
         ShowWindow $0 ${SW_HIDE}
FunctionEnd

Function io
InstallOptions::initDialog /NOUNLOAD "$PLUGINSDIR\install_mplayer_full.ini"

   !insertmacro MUI_HEADER_TEXT $(LANG_IN_OPT_TEXT) $(LANG_IN_OPTS_TEXT)
   InstallOptions::show
FunctionEnd

Function Goto
FindProcDLL::FindProc "MPlayer.exe"
StrCmp $R0 0 +3
   MessageBox MB_YesNo $(LANG_IN_RUN_TEXT) IDYES NoAbort
Abort
NoAbort:
FunctionEnd

Function .onInit
InitPluginsDir
File /oname=$PLUGINSDIR\install_mplayer_full.ini "install_files\install_mplayer_full.ini"

WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 1" "Text" "$(LANG_STR_DESK_TEXT)"
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 2" "Text" "$(LANG_STR_QUICK_TEXT)"
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 3" "Text" "$(LANG_STR_REAL_TEXT)"
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 4" "Text" "$(LANG_STR_SHORT_TEXT)"
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 7" "Text" "$(LANG_STR_ASSOC_TEXT)"
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 8" "Text" "$(LANG_STR_INI_TEXT)"

ReadRegDWORD $0 HKLM "SOFTWARE\Classes\rmocx.RealPlayer G2 Control" ""
ReadRegDWORD $1 HKLM "SOFTWARE\Classes\rmocx.RealPlayer Download Handler" ""
${if} $1 == ""
${andif} $0 == ""
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 3" "State" "1"
${else}
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 5" "Text" "$(LANG_STR_REALD_TEXT)"
WriteINIStr "$PLUGINSDIR\install_mplayer_full.ini" "Field 3" "State" "0"
${endif}

FunctionEnd

Function .onInstSuccess
  hidewindow
  SetOutPath "$INSTDIR"
  ReadINIStr $1 "$PLUGINSDIR\install_mplayer_full.ini" "Field 7" "State"
  StrCmp $1 1 0 +2
  Execwait '"$INSTDIR\meditor2.exe" /Open Assoc'

  RMdir /r "$PLUGINSDIR"
FunctionEnd
