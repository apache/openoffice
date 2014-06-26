/**************************************************************************
Name: GoogleTalkPortable

Version:

Website:

Written for:
	NSIS 2.17 or higher (last tested with 2.30)
	Required plugins: NewAdvSplash, Registry, FindProc, Dialogs
License:
	Copyright© 2008 by

More informations about installation, directory structure etc. could be found
at the very end of this file or in the accompanying readme.txt

Portable application template created 2008 by Karl Loncarek,
 					version 2.5.1 - 2008/01/06
The license of the template is a two clause BSD-style license. It could be
found a the end of this file or in the accompanying readme.txt.
**************************************************************************/
; ##########################################################################
; # Change the following constants depending on the application you want to make portable
; ##########################################################################
; ----- Basic informations
!define AUTHOR 		"Zdravko Sebastijanovic"		; your name
!define APP 		"RapidSVN"		; insert application name, e.g. "TestApp"
!define VER 		"0.1.0.0"	; insert version of launcher,first two digits are version numbers, last digit is packet revision
!define EXE 		"bin/rapidsvn.exe"		; insert program exe name, e.g. "testapp.exe"
!define EXEPARMS 	""		; insert some default Parameters 

; ----- Application specific stuff
; insert regkeys to use separated by "||", comment out, when not used, 
; e.g. HKCU\Software
	!define REGKEYS 			"HKCU\Software\RapidSVN"
; If a file "Registry.reg" within the data directory is found then it is read
; and all child registry keys are processed as if they'd have been set within
; REGKEYS
	!define USEREGKEYSFILE		"TRUE"
; delete alle defined registry keys before own ones are applied (during Init)
	!define DELETEREGKEYS		"TRUE"
; insert settings files to use separated by "||" as stored on the host
; computer, e.g. "$WINDIR\TEST.INI", comment out, when not used
	!define SETTINGSFILES 		""
; insert settings directories to use separated by "||" as stored on the host
; computer, e.g. "$PROFILE\TEST", comment out, when not used
	!define SETTINGSDIRS 		""
; Require local administrator rights to run the launcher, necessary when
; writing to e.g. HKLM registry key. If not required comment out
	!define ADMINREQUIRED 		"FALSE"
; Redirect Userprofile folder, comment out when your application calls other
; programs, i.e. to disable automatic redirection. Default value "TRUE".
	!define REDIRECTUSERPROFILE 	"TRUE"
; define which GTK version should be used, e.g. 2.0, see docs at the end
	!define USEGTKVERSION		""
; define which JAVA version should be used, e.g. 1.6.0_01, see docs at the end
	!define USEJAVAVERSION		""

; ----- Settings when you create a standalone launcher / installer (see docs)
; When "TRUE" a launcher is created that contains the sources and copies them
; into the appropriate folder if they do not exist yet.
	!define INSTALLSOURCES 			"TRUE"
; When "TRUE" a launcher is created that contains the default Registry.reg and
; other default files and copies them into the appropriate folder if they do
; not exist yet.
	!define INSTALLDEFAULTS		"TRUE"
; define which commandline parameters should be used when compressing copied
; application files (*.exe, *.dll). Comment out if no UPX compression should be
; used. first version gives best compression, but sometime the result does not
; work correctly. Works only with Win2000 or later!
	!define UPXPARMS			"--best --lzma"
;	!define UPXPARMS			"--best --compress-icons=0 --nrv2e --crp-ms=999999"

; ----- Information that is needed for the portableapps.com menu (optional)
; When "TRUE" a launcher is created that may contain some default files for the
; portable application being PAF compatible (portableapps.com Format). If this
; constant is commented out all the follwoing settings in this section will be
; ignored.
	!define PAFCOMPATIBILITY		"TRUE"
; name of publisher (usually the author and the application programmer(company)
	!define PUBLISHER			"${AUTHOR}"
; homepage which contains the download, could be the homepage of the launcher
; or of the application
	!define HOMEPAGE			"http://rapidsvn.tigris.org"
; which group of applications does it belong to, e.g. Utilities
	!define CATEGORY			"Development"
; describe here what the application does
	!define DESCRIPTION			"${APP} Portable"
; which size will the installed application have?
	!define INSTALLSIZE			"12.0"
; Is it allowed to share the portable application with others? Beware of licensing issues!
	!define SHAREABLE			"true"

; ----- Normally no need to change anything here
; format of portable name (dirs and filenames)
	!define PNAME 				"${APP}Portable"
; comment this line out when default icon should be used
	!define ICON 				"${PNAME}.ico"
; comment this line out when no splashscreen image should be used
;	!define SPLASHIMAGE 			"${PNAME}.jpg"
; could be changed when settings for multiple applications should be stored in
; one INI file
	!define INI 				"${PNAME}.ini"

; ##########################################################################
; #  Normally no need to change anything after this point (only for intermediate/advanced users!)
; ##########################################################################

; **************************************************************************
; *  Compiler Flags (to reduce executable size, saves some bytes)
; **************************************************************************
SetDatablockOptimize on
SetCompress force
SetCompressor /SOLID /FINAL lzma
OutFile "${PNAME}.exe"

; **************************************************************************
; *  Check values of defines above and do work if necessary (mainly to avoid warnings, i.e. optimize result)
; **************************************************************************
!ifdef REGKEYS
	!if ! "${REGKEYS}" = "" ; only do stuff if constant is not empty
		!define DOREG
	!endif
!endif
!if "${USEREGKEYSFILE}" = "TRUE"
	!define DOREGFILE
!endif
!ifdef SETTINGSFILES
	!if ! "${SETTINGSFILES}" = "" ; only do stuff if constant is not empty
		!define DOFILES
	!endif
!endif
!ifdef SETTINGSDIRS
	!if ! "${SETTINGSDIRS}" = "" ; only do stuff if constant is not empty
		!define DODIRS
	!endif
!endif
!ifdef USEGTKVERSION
	!if ! "${USEGTKVERSION}" = "" ; only do stuff if constant is not empty
		!define USEGTK
	!endif
!endif
!ifdef USEJAVAVERSION
	!if ! "${USEJAVAVERSION}" = "" ; only do stuff if constant is not empty
		!define USEJAVA
	!endif
!endif
!if "${PAFCOMPATIBILITY}" = "TRUE"
	!define DOWARNING ; show information about missing files
!endif
!if "${INSTALLDEFAULTS}" = "TRUE"
	!ifndef DOWARNING
		!define DOWARNING ; show information about missing files
	!endif
!endif
!if "${INSTALLSOURCES}" = "TRUE"
	!ifndef DOWARNING
		!define DOWARNING ; show information about missing files
	!endif
!endif

; **************************************************************************
; *  Includes
; **************************************************************************
!ifdef DOREG | DOREGFILE ; add macro only when needed
	!include "Registry.nsh" ; add registry manipulation macros, not included to NSIS by default
!endif
!include "WordFunc.nsh" ; add header for word manipulation
!insertmacro "WordFind" ; add function for splitting strings
!include "FileFunc.nsh" ; add header for file manipulation
!insertmacro "GetParameters" ; add function for retrieving command line parameters
!define VAR_R0 10 ;$R0 - needed for dialogs

; **************************************************************************
; *  Runtime Switches
; **************************************************************************
CRCCheck On ; do CRC check on launcher before start ("Off" for later EXE compression)
WindowIcon Off ; show no icon of the launcher
SilentInstall Silent ; start as launcher, not as installer
AutoCloseWindow True ; automatically close when finished
SetOverwrite ifnewer ; install only files that are newer or do not exist yet

; **************************************************************************
; *  Define working variables
; **************************************************************************
Var SPLASHSCREEN ; holds the information whether the splash screen should be shown, default "enabled"
Var PROGRAMEXE ; holds the name of the EXE file to launch
Var PROGRAMDIR ; holds the path to the above EXE file
Var PROGRAMPARMS ; holds some additional parameters when launching the EXE
Var DATADIR ; holds the path to the location where all the settings should be saved
Var COMMONDIR ; holds the path to the location where common files like JRE or GTK could be found
!ifdef USEGTK
	Var GTKVERSION ; holds the GTK version that should be used
	Var GTKDIR ; holds the path to the location where the needed GTK version could be found
!endif
!ifdef USEJAVA
	Var JAVAVERSION ; holds the JAVA version that should be used
	Var JAVADIR ; holds the path to the location where the needed JAVA version could be found
!endif
Var INIFILE ; holds the complete path to the found INI file
Var SECONDLAUNCH ; holds whether the EXE may be called a second time
!if "${INSTALLSOURCES}" = "TRUE"
	Var SOURCEDIR ; holds the path to the location where the launcher source is stored
	Var EXTRACTSOURCES ; holds whether the sources should be extracted eevry time
!endif

; **************************************************************************
; *  Set basic information
; **************************************************************************
Name "${APP} Portable"
!ifdef ICON
	Icon "${ICON}"
!endif
Caption "${APP} Portable - ${VER}"
OutFile "${PNAME}.exe"
RequestExecutionLevel user
!if "${ADMINREQUIRED}" = "TRUE"
	RequestExecutionLevel admin
!endif

; **************************************************************************
; *  Set version information
; **************************************************************************
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
VIProductVersion "${Ver}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${APP} Portable"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Allow ${APP} to be run from a removeable drive. This launcher is based on the Portable Application Template created by Klonk (Karl Loncarek)."
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Launcher created by ${AUTHOR}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "by ${AUTHOR}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${APP} Portable"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "${PNAME}.exe"

; **************************************************************************
; *  Main section
; **************************************************************************
Section "Main"
	Call InitINI ; apply INI settings
	Call InitVars ; set default variable values when no INI is used
	Call InitInstall ; installs additional files, e.g. sources or INI-files
	Call Init ; other initalizations before any registry, folder, or fileoperations are done
	Call InitReg ; backup current reg, apply portable reg
	Call InitFiles ;rename current files, apply portable files
	Call InitFolders ; rename current folders, apply portable folders
	Call RunApp ; run app
	Call CleanFolders ; copy portable folders, delete portable folders, restore original folders
	Call CleanFiles ; copy portable files, delete portable files, restore original files
	Call CleanReg ; copy reg, restore original reg
	Call Clean ; Absolute last things to do
SectionEnd

; **************************************************************************
; *  Function: Search for INI file, read it, and set variables when necessary
; **************************************************************************
Function InitINI
	; --------------------------------------------------------------------------
	; Empty variables
	; --------------------------------------------------------------------------
	StrCpy "$PROGRAMDIR" ""
	StrCpy "$DATADIR" ""
	StrCpy "$COMMONDIR" ""
	StrCpy "$PROGRAMEXE" ""
	StrCpy "$SPLASHSCREEN" ""
	StrCpy "$PROGRAMPARMS" ""
	!ifdef USEGTK
		StrCpy "$GTKVERSION" "${USEGTKVERSION}"
	!endif
	!ifdef USEJAVA
		StrCpy "$JAVAVERSION" "${USEJAVAVERSION}"
	!endif
	!if "${INSTALLSOURCES}" = "TRUE"
		StrCpy "$EXTRACTSOURCES" "TRUE"
	!endif
	; --------------------------------------------------------------------------
	; Check whether an INI file exists, set variable pointing to it
	; --------------------------------------------------------------------------
	IfFileExists "$EXEDIR\${INI}" "" CheckPortableINI
		StrCpy "$INIFILE" "$EXEDIR\${INI}"
		Goto ReadINIFile
	CheckPortableINI:
		IfFileExists "$EXEDIR\${PNAME}\${INI}" "" CheckPortableAppsINI
			StrCpy "$INIFILE" "$EXEDIR\${PNAME}\${INI}"
			Goto ReadINIFile
	CheckPortableAppsINI:
		IfFileExists "$EXEDIR\PortableApps\${PNAME}\${INI}" "" InitINIEnd
			StrCpy "$INIFILE" "$EXEDIR\PortableApps\${PNAME}\${INI}"
			Goto ReadINIFile
	Goto InitINIEnd
	; --------------------------------------------------------------------------
	; Read content of the INI file, save only used
	; --------------------------------------------------------------------------
	ReadINIFile:
		ReadINIStr "$0" "$INIFILE" "${PNAME}" "ProgramDirectory"
		StrCmp "$0" "" INIDataDirectory ; if emtpy check next setting
			StrCpy "$PROGRAMDIR" "$EXEDIR\$0" ; save program directory
		INIDataDirectory:
			ReadINIStr "$0" "$INIFILE" "${PNAME}" "DataDirectory"
			StrCmp "$0" "" INICommonDirectory ; if empty retrieve correct setting
				StrCpy "$DATADIR" "$EXEDIR\$0" ; save data directory
		INICommonDirectory:
			ReadINIStr "$0" "$INIFILE" "${PNAME}" "CommonDirectory"
			StrCmp "$0" "" INIProgramExecutable ; if empty retrieve correct setting
				StrCpy "$COMMONDIR" "$0" ; save common directory
		INIProgramExecutable:
			ReadINIStr "$0" "$INIFILE" "${PNAME}" "ProgramExecutable"
			StrCmp "$0" "" INISplashScreen ; if emtpy use default
				StrCpy "$PROGRAMEXE" "$0" ; save .exe name
		INISplashScreen:
			ReadINIStr "$0" "$INIFILE" "${PNAME}" "SplashScreen"
			StrCmp "$0" "" INIGTKVersion ; check whether variable splashscreen was empty
				StrCpy "$SPLASHSCREEN" "$0" ; save state of splashscreen display
		INIGTKVersion:
		!ifdef USEGTK
				ReadINIStr "$0" "$INIFILE" "${PNAME}" "GTKVersion"
				StrCmp "$0" "" INIJAVAVersion ; check whether variable GTKVersion was empty
					StrCpy "$GTKVERSION" "$0" ; save state of splashscreen display
			INIJAVAVersion:
		!endif
		!ifdef USEJAVA
				ReadINIStr "$0" "$INIFILE" "${PNAME}" "JAVAVersion"
				StrCmp "$0" "" INIProgramParameters ; check whether variable JAVAVersion was empty
					StrCpy "$JAVAVERSION" "$0" ; save state of splashscreen display
			INIProgramParameters:
		!endif
			ReadINIStr "$0" "$INIFILE" "${PNAME}" "ProgramParameters"
			StrCpy "$PROGRAMPARMS" "$0" ; save additional program parameters
			!if "${INSTALLSOURCES}" = "TRUE"
				ReadINIStr "$0" "$INIFILE" "${PNAME}" "ExtractSources"
				StrCmp "$0" "" InitINIEnd ; check whether variable exctractsources was empty
					StrCpy "$EXTRACTSOURCES" "$0" ; save whether sources should be extracted or not
			!endif
	InitINIEnd: ;simply the end of the function
FunctionEnd

; **************************************************************************
; *  Function: Fill used variables with default values, if not set already
; **************************************************************************
Function InitVars
	; --------------------------------------------------------------------------
	; Set default values for variables, when not set already
	; --------------------------------------------------------------------------
	StrCmp "$SPLASHSCREEN" "" 0 InitProgramEXE
		StrCpy "$SPLASHSCREEN" "enabled" ; enable splashscreen
	InitProgramEXE:
		StrCmp "$PROGRAMEXE" "" 0 InitProgramDIR
			StrCpy "$PROGRAMEXE" "${EXE}" ; use default setting
	InitProgramDIR:
		StrCmp "$PROGRAMDIR" "" 0 InitVarEnd ; no programdir set before (by INI file)
			; --------------------------------------------------------------------------
			; Try to find out allowed "CommonFiles" directory
			; --------------------------------------------------------------------------
			${WordFind} "$EXEDIR" "\" "-02{*"  $R0
			IfFileExists "$R0\CommonFiles\*.*" 0 +2
				StrCpy "$COMMONDIR" "$R0\CommonFiles"
			IfFileExists "$EXEDIR\CommonFiles\*.*" 0 +2
				StrCpy "$COMMONDIR" "$EXEDIR\CommonFiles"
			IfFileExists "$EXEDIR\PortableApps\CommonFiles\*.*" 0 +2
				StrCpy "$COMMONDIR" "$EXEDIR\PortableApps\CommonFiles"
			; --------------------------------------------------------------------------
			; Set JAVA and GTK directory when found within "CommonFiles"
			; --------------------------------------------------------------------------
			!ifdef USEJAVA
				IfFileExists "$COMMONDIR\JAVA\*.*" 0 +2
					StrCpy "$JAVADIR" "$COMMONDIR\JAVA"
				IfFileExists "$COMMONDIR\JAVA\$JAVAVERSION\*.*" 0 +2 
					StrCpy "$JAVADIR" "$COMMONDIR\JAVA\$JAVAVERSION" ; higher priority, use this JAVA directory
			!endif
			!ifdef USEGTK
				IfFileExists "$COMMONDIR\GTK\*.*" 0 +2
					StrCpy "$GTKDIR" "$COMMONDIR\GTK"
				IfFileExists "$COMMONDIR\GTK\$GTKVERSION\*.*" 0 +2
					StrCpy "$GTKDIR" "$COMMONDIR\GTK\$GTKVERSION" ; higher priority, use this GTK directory
			!endif
			; --------------------------------------------------------------------------
			; Predefine default directory structure
			; --------------------------------------------------------------------------
			StrCpy "$DATADIR" "$EXEDIR\Data"
			StrCpy "$PROGRAMDIR" "$EXEDIR\App\${APP}"
			!if "${INSTALLSOURCES}" = "TRUE"
				StrCpy "$SOURCEDIR" "$EXEDIR\Other\${PNAME}Sources"
			!endif
			!ifdef USEJAVA
				IfFileExists "$EXEDIR\App\JAVA\*.*" 0 +2
					StrCpy "$JAVADIR" "$EXEDIR\App\JAVA" ; highest priority, use this JAVA directory
			!endif
			!ifdef USEGTK
				IfFileExists "$EXEDIR\App\GTK\*.*" 0 +2
					StrCpy "$GTKDIR" "$EXEDIR\App\GTK" ; highest priority, use this GTK directory
			!endif
			; --------------------------------------------------------------------------
			; Check which other directory configuration is used and set variables accordingly
			; --------------------------------------------------------------------------
			IfFileExists "$EXEDIR\${PNAME}\App\${APP}\*.*" 0 CheckPortableAppsDIR
				StrCpy "$PROGRAMDIR" "$EXEDIR\${PNAME}\App\${APP}"
				StrCpy "$DATADIR" "$EXEDIR\${PNAME}\Data"
				!if "${INSTALLSOURCES}" = "TRUE"
					StrCpy "$SOURCEDIR" "$EXEDIR\${PNAME}\Other\${PNAME}Sources"
				!endif
				!ifdef USEJAVA
					IfFileExists "$EXEDIR\${PNAME}\App\JAVA\*.*" 0 +2
						StrCpy "$JAVADIR" "$EXEDIR\${PNAME}\App\JAVA" ; highest priority, use this JAVA directory
				!endif
				!ifdef USEGTK
					IfFileExists "$EXEDIR\${PNAME}\App\GTK\*.*" 0 +2
						StrCpy "$GTKDIR" "$EXEDIR\${PNAME}\App\GTK" ; highest priority, use this GTK directory
				!endif
				Goto InitVarEnd
			CheckPortableAppsDIR:
			IfFileExists "$EXEDIR\PortableApps\${PNAME}\App\${APP}\*.*" 0 InitDataDIR
				StrCpy "$PROGRAMDIR" "$EXEDIR\PortableApps\${PNAME}\App\${APP}"
				StrCpy "$DATADIR" "$EXEDIR\PortableApps\${PNAME}\Data"
				!if "${INSTALLSOURCES}" = "TRUE"
					StrCpy "$SOURCEDIR" "$EXEDIR\PortableApps\${PNAME}\Other\${PNAME}Sources"
				!endif
				!ifdef USEJAVA
					IfFileExists "$EXEDIR\PortableApps\${PNAME}\App\JAVA\*.*" 0 +2
						StrCpy "$JAVADIR" "$EXEDIR\PortableApps\${PNAME}\App\JAVA" ; highest priority, use this JAVA directory
				!endif
				!ifdef USEGTK
					IfFileExists "$EXEDIR\PortableApps\${PNAME}\App\GTK\*.*" 0 +2
						StrCpy "$GTKDIR" "$EXEDIR\PortableApps\${PNAME}\App\GTK" ; highest priority, use this GTK directory
				!endif
				Goto InitVarEnd
		; --------------------------------------------------------------------------
		; Check whether DataDirectory was set in the INI, only called, when ProgramDirectory was set in INI -> misconfigured
		; --------------------------------------------------------------------------
		InitDataDIR:
			StrCmp "$DATADIR" "" 0 InitVarEnd
				MessageBox MB_OK|MB_ICONEXCLAMATION `"DataDirectory" was not set in INI file.  Please check your configuration!`
				Abort ; terminate launcher
	InitVarEnd:
FunctionEnd

; **************************************************************************
; *  Function: Installs additional files, e.g. launcher sources, INI files etc.
; **************************************************************************
Function InitInstall
	!ifdef DOWARNING
		!warning "FOLLOWING WARNINGS ABOUT MISSING FILES CAN BE IGNORED!"
	!endif
	; --------------------------------------------------------------------------
	; Install informations, create file needed for portableapps.com menu
	; --------------------------------------------------------------------------
	StrCpy "$R0" "" ; reset working variable
	!if "${PAFCOMPATIBILITY}" = "TRUE"
		${WordFind} "$PROGRAMDIR" "\" "-02{*"  "$R0"; remove last part of the folder
		IfFileExists "$R0\AppInfo\*.*" +2
			CreateDirectory "$R0\AppInfo" ; create appinfo directory
		SetOutPath "$R0\AppInfo"
		!ifdef ICON
			File "/oname=appicon.ico" "${ICON}" ; extract icon
		!endif
		File /nonfatal "appinfo.ini" ; will give a warning when it does not exist
		IfFileExists "$R0\AppInfo\appinfo.ini" InstallOthers ; if it does not exist then create the file
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Format" "Type" "PortableApps.comFormat"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Format" "Version" "0.9.6"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "Name" "${APP} Portable"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "Name" "${APP} Portable"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "Publisher" "${PUBLISHER}"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "Homepage" "${HOMEPAGE}"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "Category" "${CATEGORY}"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "Description" "${DESCRIPTION}"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Details" "InstallSize" "${INSTALLSIZE}"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "License" "Shareable" "${SHAREABLE}"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Version" "PackageVersion" "${VER}"
			${WordFind} "${VER}" "." "-01" "$0" ; save only last digit of version number
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Version" "PackageRevision" "$0"
			${WordFind} "${VER}" "." "+02}" "$0" ; save first two digits of version number
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Version" "DisplayVersion" "$0"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Control" "Icons" "1"
			WriteINIStr "$R0\AppInfo\appinfo.ini" "Control" "Start" "${PNAME}.exe"
		InstallOthers:
		IfFileExists "$R0\DefaultData\*.*" +2
			CreateDirectory "$R0\DefaultData" ; create appinfo directory
		SetOutPath "$R0\DefaultData"
		!if "${INSTALLDEFAULTS}" = "TRUE"
			File /nonfatal "Registry.reg" ; will give a warning when it does not exist during compilation
		!endif
	!endif
	; --------------------------------------------------------------------------
	; Install default settings in data directory
	; --------------------------------------------------------------------------
	!if "${INSTALLDEFAULTS}" = "TRUE"
		SetOutPath "$DATADIR"
		File /nonfatal "Registry.reg" ; will give a warning when it does not exist during compilation
; here additional default files could be installed
	!endif
	; --------------------------------------------------------------------------
	; Install source files, i.e. copy them to sources folder
	; --------------------------------------------------------------------------
	!if "${INSTALLSOURCES}" = "TRUE"
		StrCmp "$EXTRACTSOURCES" "TRUE" 0 InitInstallSourcesEnd ; if variable correctly set in INI or by defualt extract
			SetOutPath "$SOURCEDIR"
			!ifdef SPLASHIMAGE
				File "${SPLASHIMAGE}" ; extract splashimage
			!endif
			!ifdef ICON
				File "${ICON}" ; extract icon
			!endif
			File "${__FILE__}"
			File /nonfatal "readme.txt" ; will give a warning when it does not exist during compilation
			File /nonfatal "appinfo.ini" ; will give a warning when it does not exist during compilation
		InitInstallSourcesEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Other initializations done before any registry, folder, or file operations
; **************************************************************************
Function Init
	; --------------------------------------------------------------------------
	; Create folders that do not exist yet
	; --------------------------------------------------------------------------
	IfFileExists "$DATADIR\*.*" +2
		CreateDirectory "$DATADIR" ; create data directory
	IfFileExists "$PROGRAMDIR\*.*" +2
		CreateDirectory "$PROGRAMDIR" ; create program directory
	; --------------------------------------------------------------------------
	; Check whether EXE exists, if not copy installed application into portable folder
	; --------------------------------------------------------------------------
	IfFileExists "$PROGRAMDIR\$PROGRAMEXE" FoundEXE
		; executable was not found in portable folder, ask to copy local installation
		MessageBox MB_YESNO|MB_ICONEXCLAMATION `$PROGRAMEXE was not found.$\nDo you want to copy your local installation into your portable applications directory? (This could take some time)$\n$\nWhen you select "NO" this launcher will be terminated. In this case, please copy the necessary files yourself.` IDYES +2
			Abort ; terminate launcher
		Dialogs::Folder "Select installation folder of ${APP} " 'Select the main folder where you installed "${APP}" on your harddrive:' "$PROGRAMFILES" "${VAR_R0}"
		CopyFiles "$R0\*.*" "$PROGRAMDIR" ; copy files from harddrive to mobile drive
		!ifdef UPXPARMS ;execute only when UPX should be used
			MessageBox MB_YESNO|MB_ICONQUESTION 'Copying is finished now. Should the copied executables (*.EXE, *.DLL) be compressed with UPX?' IDNO LaunchEXE
			Dialogs::Open "(*.EXE)|*.exe|" "Please select the upx.exe you want to use:" "$EXEDIR" "${VAR_R0}" ; $R0 holds the path including name to upx.exe
			; recursive compression with UPX: for /r %e in (*.exe,*.dll) do "$R0" "%e" --best --lzma
			; works only with Windows2000 or later
			ExecWait 'cmd.exe /C for /r %e in (*.exe,*.dll) do "$R0" "%e" ${UPXPARMS}'
		!endif
		LaunchEXE:
		MessageBox MB_YESNO|MB_ICONINFORMATION "You could now (or later) delete unneeded files.$\nDo you want to launch ${PNAME}?" IDYES +2
			Abort ; terminate launcher
		; Program executable not where expected
		IfFileExists "$PROGRAMDIR\$PROGRAMEXE" FoundEXE
			MessageBox MB_OK|MB_ICONEXCLAMATION `$PROGRAMEXE was not found. Please check your configuration!`
			Abort ; terminate Launcher
	; --------------------------------------------------------------------------
	; Check whether EXE is launched a second time
	; --------------------------------------------------------------------------
	FoundEXE: ; Check if already running and set variable
		FindProcDLL::FindProc "$PROGRAMEXE"                 
		StrCmp "$R0" "1" "" EndEXE
			StrCpy "$SECONDLAUNCH" "true"
	EndEXE:
	; --------------------------------------------------------------------------
	; Check whether current user is admin (only when required)
	; --------------------------------------------------------------------------
	!if "${ADMINREQUIRED}" = "TRUE"
		userInfo::getAccountType
		pop "$0"
		StrCmp "$0" "Admin" InitAdminEnd
			messageBox MB_OK|MB_ICONEXCLAMATION "You must be logged in as a local administrator for this launcher to work!"
			Abort
		InitAdminEnd:
	!endif	
	; --------------------------------------------------------------------------
	; Display splashscreen when available
	; --------------------------------------------------------------------------
	!ifdef SPLASHIMAGE
		StrCmp "$SPLASHSCREEN" "enabled" 0 NoSplash
			InitPluginsDir
			File /oname=$PLUGINSDIR\splash.jpg "${SPLASHIMAGE}"	
			newadvsplash::show /NOUNLOAD 2500 200 200 -1 /L $PLUGINSDIR\splash.jpg
		NoSplash:
	!endif
	; --------------------------------------------------------------------------
	; Temporarily redirect USERPROFILE folder (application should write own data into that directory
	; --------------------------------------------------------------------------
	!if "${REDIRECTUSERPROFILE}" = "TRUE"
		IfFileExists "$DATADIR\UserProfile\*.*" +2
			CreateDirectory "$DATADIR\UserProfile" ; create directory for portable user profile
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("USERPROFILE", "$DATADIR\UserProfile").r0' ; set new user profile folder
		StrCmp "$0" "0" ProfileError
			Goto ProfileDone
		ProfileError:
			MessageBox MB_ICONEXCLAMATION|MB_OK "Can't set environment variable for new Userprofile!$\nLauncher will be terminated."
			Abort
		ProfileDone:
	!endif
	; --------------------------------------------------------------------------
	; Temporarily set GTK/JAVA environment variables
	; --------------------------------------------------------------------------
	!ifdef USEJAVA
		ReadEnvStr "$R0" "PATH" ; obtain current PATH setting
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PATH", "$JAVADIR\bin;$R0").r0' ; set new path
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("JAVA_HOME", "$JAVADIR").r0' ; set new path
	!endif
	!ifdef USEGTK
		ReadEnvStr "$R0" "PATH" ; obtain current PATH setting
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PATH", "$GTKDIR\bin;$R0").r0' ; set new path
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("GTK_HOME", "$GTKDIR").r0' ; set new path
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("GTKMM_HOME", "$GTKDIR").r0' ; set new path
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("GTK_BASEPATH", "$GTKDIR").r0' ; set new path
		System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("GTKMM_BASEPATH", "$GTKDIR").r0' ; set new path
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Backup registry keys, apply portable registry keys
; **************************************************************************
Function InitReg
	!ifdef DOREG | DOREGFILE
		StrCmp "$SECONDLAUNCH" "true" InitRegEnd ; do not do anything if launched a second time
			IfFileExists "$DATADIR\RegistryBackup.reg"  0 +2
				Delete "$DATADIR\RegistryBackup.reg" ; delete registry backup file if it exists
			StrCpy "$R8" "0" ; reset variable
			!ifdef DOREGFILE ; use "Registry.reg" as source for the registry keys to copy
				Push "EndOfStack" ; just in case no registry.reg file exists to make sure the loop afterwards is exited immediately
				IfFileExists "$DATADIR\Registry.reg" 0 InitRegUseVar
					StrCpy "$R0" "$DATADIR\Registry.reg"
					Call RegFileToStack ; copy registry keys stored in the file to stack
					Goto InitRegLoop ; override REGKEYS
				InitRegUseVar:
			!endif
			!ifdef DOREG
				StrCpy "$R0" "${REGKEYS}" ; copy constant to working variable
				Call ValuesToStack ; separate values from REGKEYS to stack
			!endif
			InitRegLoop:
				Pop "$R9" ; obtain registry key from stack
				StrCmp "$R9" "EndOfStack" InitRegApply ; do not do registry parsing, when no keys given anymore
					IntOp "$R8" "$R8" + 1 ; increase counter
					; --------------------------------------------------------------------------
					; Backup registry key
					; --------------------------------------------------------------------------
					${registry::KeyExists} "$R9" "$R7" ; check whether registry key exists
					StrCmp "$R7" "0" 0 InitRegLoop ; registry key does not exist, do not save anything
						${registry::SaveKey} "$R9" "$DATADIR\RegistryBackup.reg" "/G=1 /A=1" "$R7" ; Backup registry key
						StrCmp "$R7" 0 InitRegNoError ; error during backup of existing registry keys occured.
							MessageBox MB_OK|MB_ICONEXCLAMATION `It is not possible to backup the registry keys. Maybe your drive is write protected. Please check and restart!`
							IfFileExists "$DATADIR\RegistryBackup.reg" 0 +2 ; only apply if a backup registry file exists
								Delete "$DATADIR\RegistryBackup.reg" ; delete existing bad registry backup
							Abort ; terminate Launcher
						InitRegNoError:
;						Sleep 50
						!if "${DELETEREGKEYS}" = "TRUE" 
							${registry::DeleteKey} "$R9" "$R7" ; delete registry key after save
						!endif
				Goto InitRegLoop
			InitRegApply:
			; --------------------------------------------------------------------------
			; Apply portable registry key, delete existing key at same time
			; --------------------------------------------------------------------------
			IfFileExists "$DATADIR\Registry.reg" 0 +2 ; only apply if a registry file exists
				ExecWait 'regedit /s "$DATADIR\Registry.reg"'
		InitRegEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Rename current files, apply portable files
; **************************************************************************
Function InitFiles
	!ifdef DOFILES
		StrCmp "$SECONDLAUNCH" "true" InitFilesEnd ; do not do anything if launched a second time
			IfFileExists "$DATADIR\Settings\*.*" +2
				CreateDirectory "$DATADIR\Settings" ; create directory for portable configuration files, if it does not exist			
			StrCpy "$R0" "${SETTINGSFILES}" ; copy constant to working variable
			Call ValuesToStack ; separate values from SETTINGSFILES to stack
			StrCpy "$R8" "0" ; reset variable
			InitFilesLoop:
				Pop "$R9" ; obtain filename from stack
				StrCmp "$R9" "EndOfStack" InitFilesEnd ; do not do filename parsing, when no filename given anymore
					IntOp "$R8" "$R8" + 1 ; increase counter
					; --------------------------------------------------------------------------
					; Delete backup file if it exists (otherwise rename won't work)
					; --------------------------------------------------------------------------
					IfFileExists "$R9-PortBak" 0 InitFilesBackup
						; Tell user that backup files/folders already exist, YES - copy portable data, keep original backup, NO - delete backup file/folder
						MessageBox MB_ICONEXCLAMATION|MB_YESNOCANCEL "Backup file $\"R9-PortBak$\" already exists. Do you want to keep it?$\n$\nYES = simple copy portable data, keep backup of original file$\nNO = delete backup file, create new backup of actual file$\nCANCEL = exit launcher, and fix problem manually$\n$\nAttention: You will be asked for every found backup file." IDYES InitFilesApply IDNO InitFilesDelete
							; CANCEL - exit launcher, fix problem
							Abort
						InitFilesDelete:
							Delete "$R9-PortBak"
					; --------------------------------------------------------------------------
					; Backup file (in fact simply rename existing file)
					; --------------------------------------------------------------------------
					InitFilesBackup:
						IfFileExists "$R9" 0 InitFilesApply ; check whether file exists
							Rename "$R9" "$R9-PortBak" ; rename file for backup
					; --------------------------------------------------------------------------
					; Apply portable settings file
					; --------------------------------------------------------------------------
					InitFilesApply:
						IfFileExists "$DATADIR\Settings\File$R8.dat" 0 InitFilesCopy ; only restore when available
							CopyFiles /SILENT "$DATADIR\Settings\File$R8.dat" "$R9" ; restore file
							Goto InitFilesLoop
					InitFilesCopy:
						CopyFiles /SILENT "$R9-PortBak" "$R9" ; copy existing settings file if no portable version exists
				Goto InitFilesLoop
		InitFilesEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Rename folder, apply portable folder
; **************************************************************************
Function InitFolders
	!ifdef DODIRS
		StrCmp "$SECONDLAUNCH" "true" InitFoldersEnd ; do not do anything if launched a second time
			IfFileExists "$DATADIR\Settings\*.*" +2
				CreateDirectory "$DATADIR\Settings" ; create directory for portable configuration files, if it does not exist			
			StrCpy "$R0" "${SETTINGSDIRS}" ; copy constant to working variable
			Call ValuesToStack ; separate values from SETTINGSDIRS to stack
			StrCpy "$R8" "0" ; reset variable
			InitFoldersLoop:
				Pop "$R9" ; obtain folder from stack
				StrCmp "$R9" "EndOfStack" InitFoldersEnd ; do not do folder parsing, when no folder given anymore
					IntOp "$R8" "$R8" + 1 ; increase counter
					; --------------------------------------------------------------------------
					; Delete backup folder if it exists (otherwise rename won't work)
					; --------------------------------------------------------------------------
					IfFileExists "$R9-PortBak\*.*" 0 InitFoldersBackup
						; Tell user that backup files/folders already exist, YES - copy portable data, keep original backup, NO - delete backup file/folder
						MessageBox MB_ICONEXCLAMATION|MB_YESNOCANCEL "Backup folder $\"R9-PortBak$\" already exists. Do you want to keep it?$\n$\nYES = simple copy portable data, keep backup of original folder$\nNO = delete backup folder, create new backup of actual folder$\nCANCEL = exit launcher, and fix problem manually$\n$\nAttention: You will be asked for every found backup folder." IDYES InitFoldersApply IDNO InitFoldersDelete
							; CANCEL - exit launcher, fix problem
							Abort
						InitFoldersDelete:
							RMDir "/r"  "$R9-PortBak" ; delete folder
					; --------------------------------------------------------------------------
					; Backup folder (in fact simply rename existing folder)
					; --------------------------------------------------------------------------
					InitFoldersBackup:
						IfFileExists "$R9\*.*" 0 InitFoldersApply ; check whether folder exists
							Rename "$R9" "$R9-PortBak" ; rename folder for backup
					; --------------------------------------------------------------------------
					; Apply portable folder
					; --------------------------------------------------------------------------
					InitFoldersApply:
						IfFileExists "$DATADIR\Settings\Dir$R8.dat\*.*" 0 InitFoldersCopy ; check whether backup exists
							IfFileExists "$R9\*.*" +2 0 ; does target folder exist
								CreateDirectory "$R9" ; create target folder
							CopyFiles /SILENT "$DATADIR\Settings\Dir$R8.dat\*.*" "$R9" ; apply folder content
							Goto InitFoldersLoop
					InitFoldersCopy:
						CopyFiles /SILENT "$R9-PortBak\*.*" "$R9" ; copy existing settings folder if no portable vrsion exists
				Goto InitFoldersLoop
		InitFoldersEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Run application
; **************************************************************************
Function RunApp
	${GetParameters} "$R0" ; obtain eventually provided commandline parameters
	StrCmp "$R0" "" 0 +2
		StrCpy "$R0" "$PROGRAMPARMS"
	!ifdef EXEPARMS
		StrCmp "$R0" "" 0 +2
			StrCpy "$R0" "${EXEPARMS}"
	!endif
	StrCmp "$SECONDLAUNCH" "true" RunAppNoWait ; simply start if launched a second time
		; --------------------------------------------------------------------------
		; Start program
		; --------------------------------------------------------------------------
		ExecWait '"$PROGRAMDIR\$PROGRAMEXE" $R0' ; run program
		Goto RunAppEnd
		; --------------------------------------------------------------------------
		; run application without waiting
		; --------------------------------------------------------------------------
	RunAppNoWait:
		Exec '"$PROGRAMDIR\$PROGRAMEXE" $R0' ; run program
	RunAppEnd:
FunctionEnd

; **************************************************************************
; *  Function: Copy portable folders, delete portable folders, restore original folders
; **************************************************************************
Function CleanFolders
	!ifdef DODIRS
		StrCmp "$SECONDLAUNCH" "true" CleanFoldersEnd ; do not do anything if launched a second time
			StrCpy "$R0" "${SETTINGSDIRS}" ; copy constant to working variable
			Call ValuesToStack ; separate values from SETTINGSDIRS to stack
			StrCpy "$R8" "0" ; reset variable
			CleanFoldersLoop:
				Pop "$R9" ; obtain folder from stack
				StrCmp "$R9" "EndOfStack" CleanFoldersEnd ; do not do folder parsing, when no folder given anymore
					IntOp "$R8" "$R8" + 1 ; increase counter
					; --------------------------------------------------------------------------
					; Copy actual folder to portable folder
					; --------------------------------------------------------------------------
					IfFileExists "$R9\*.*" 0 CleanFoldersRestore ; check whether source folder exists
						IfFileExists "$DATADIR\Settings\Dir$R8.dat" +2 0 ; does target folder exist?
							CreateDirectory "$DATADIR\Settings\Dir$R8.dat" ; create target folder
						CopyFiles /SILENT "$R9\*.*" "$DATADIR\Settings\Dir$R8.dat" ; backup folder
						; --------------------------------------------------------------------------
						; Delete actual folder (with portable content)
						; --------------------------------------------------------------------------
						RMDir "/r" "$R9" ; delete directory
					; --------------------------------------------------------------------------
					; Restore original folder
					; --------------------------------------------------------------------------
					CleanFoldersRestore:
						IfFileExists "$R9-PortBak\*.*" 0 CleanFoldersLoop ; check whether folder exists
							Rename "$R9-PortBak" "$R9"; rename folder back to original name
			Goto CleanFoldersLoop
		CleanFoldersEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Copy portable files, delete portable files restore original files
; **************************************************************************
Function CleanFiles
	!ifdef DOFILES
		StrCmp "$SECONDLAUNCH" "true" CleanFilesEnd ; do not do anything if launched a second time
			StrCpy "$R0" "${SETTINGSFILES}" ; copy constant to working variable
			Call ValuesToStack ; separate values from SETTINGSFILES to stack
			StrCpy "$R8" "0" ; reset variable
			CleanFilesLoop:
				Pop "$R9" ; obtain filename from stack
				StrCmp "$R9" "EndOfStack" CleanFilesEnd ; do not do filename parsing, when no filename given anymore
					IntOp "$R8" "$R8" + 1 ; increase counter
					; --------------------------------------------------------------------------
					; Copy actual file to portable folder
					; --------------------------------------------------------------------------
					IfFileExists "$R9" 0 CleanFilesRestore ; check whether file exists
						CopyFiles /SILENT "$R9" "$DATADIR\Settings\File$R8.dat" ; backup file
						; --------------------------------------------------------------------------
						; Delete actual file (with portable content)
						; --------------------------------------------------------------------------
						Delete "$R9" ; delete file
					; --------------------------------------------------------------------------
					; Restore original file
					; --------------------------------------------------------------------------
					CleanFilesRestore:
						IfFileExists "$R9-PortBak" 0 CleanFilesLoop ; check whether file exists
							Rename "$R9-PortBak" "$R9"; rename file back to original name
			Goto CleanFilesLoop
		CleanFilesEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Copy registry key (portable), restore oroginal registry key
; **************************************************************************
Function CleanReg
	!ifdef DOREG | DOREGFILE
		StrCmp "$SECONDLAUNCH" "true" CleanRegEnd ; do not do anything if launched a second time
			StrCpy "$R8" "0" ; reset variable
			!ifdef DOREGFILE ; use "Registry.reg" as source for the registry keys to copy
				IfFileExists "$DATADIR\Registry.reg" 0 CleanRegUseVar
					StrCpy "$R0" "$DATADIR\Registry.reg"
					Call RegFileToStack ; copy registry keys stored in the file to stack
					Goto CleanRegCleanUp ; override REGKEYS
				CleanRegUseVar:
			!endif
			!ifdef DOREG
				StrCpy "$R0" "${REGKEYS}" ; copy constant to working variable
				Call ValuesToStack ; separate values from REGKEYS to stack
			!endif
			!ifdef DOREGFILE
				CleanRegCleanUp:
			!endif
			IfFileExists "$DATADIR\Registry.reg"  0 +2
				Delete "$DATADIR\Registry.reg" ; delete portable registry file if it exists to write a new one
			CleanRegLoop:
				Pop "$R9" ; obtain registry key from stack
				StrCmp "$R9" "EndOfStack" CleanRegApply ; do not do registry parsing, when no keys given anymore
					IntOp "$R8" "$R8" + 1 ; increase counter
					; --------------------------------------------------------------------------
					; Copy actual registry key to portable folder
					; --------------------------------------------------------------------------
					${registry::KeyExists} "$R9" "$R7" ; check whether registry key exists
					StrCmp "$R7" "0" 0 CleanRegLoop ; registry key does not exist, do not save anything
						${registry::SaveKey} "$R9" "$DATADIR\Registry.reg" "/G=1 /A=1" "$R7" ; Backup registry key
						StrCmp "$R7" 0 +3 ; an error occures when saving registry key
							MessageBox MB_OK|MB_ICONEXCLAMATION `It is not possible to save the registry keys. Maybe your drive is write protected. Please check and press OK afterwards!`
							${registry::SaveKey} "$R9" "$DATADIR\Registry.reg" "/G=1 /A=1" "$R7" ; Backup registry key
;						Sleep 50
						; --------------------------------------------------------------------------
						; Delete actual actual registry key (with portable content)
						; --------------------------------------------------------------------------
						${registry::DeleteKey} "$R9" "$R7" ; Delete registry key
				Goto CleanRegLoop
			CleanRegApply:
			; --------------------------------------------------------------------------
			; Restore original registry key
			; --------------------------------------------------------------------------
			IfFileExists "$DATADIR\RegistryBackup.reg" 0 +2 ; only restore if a registry file exists
				ExecWait 'regedit /s "$DATADIR\RegistryBackup.reg"'
		CleanRegEnd:
	!endif
FunctionEnd

; **************************************************************************
; *  Function: Clean up stuff,  The absolute last things to do
; **************************************************************************
Function Clean
	StrCmp "$SECONDLAUNCH" "true" CleanEnd ; do not do anything if launched a second time
		!ifdef DOREG | DOREGFILE
			${registry::Unload} ; unload registry functions from, memory
;			Sleep 500 ; let REGEDIT read the registry file
			IfFileExists "$DATADIR\RegistryBackup.reg" 0 CleanEnd ; remove registry backup file
				Delete "$DATADIR\RegistryBackup.reg"
		!endif
		!ifdef SPLASHIMAGE ; remove the dll form the temp directory, clean up
			StrCmp "$SPLASHSCREEN" "enabled" 0 CleanEnd
			newadvsplash::stop /WAIT
		!endif
	CleanEnd:
FunctionEnd

; ##########################################################################
; #  Helper function which might be called from one of the above functions
; ##########################################################################

; **************************************************************************
; *  Helper Function: Move value of constants onto stack, $R0 holds values separated by "||"
; **************************************************************************
!ifdef DOREG | DOFILES | DODIRS
	Function ValuesToStack
		StrCpy "$0" "0" ; reset counter
		; --------------------------------------------------------------------------
		; Get single parameter out of list, i.e. obtain next single registry key
		; --------------------------------------------------------------------------
		Push "EndOfStack" ; set end marker for stack
		ValuesToStackStart:
			StrCmp "$R0" "" ValuesToStackEnd ; do not do registry parsing, when no keys given anymore
				IntOp "$0" "$0" + 1 ; increase counter
				${WordFind} "$R0" "||" "-01" "$9" ; save last parameter to register
				${WordFind} "$R0" "||" "-02{*"  "$R0" ; remove last part from saved value
				Push $9 ; save parameter to stack
				StrCmp "$R0" "$9" ValuesToStackEnd ; if values are identical (last parameter) -> no more delimiters
			Goto ValuesToStackStart
		ValuesToStackEnd:
	FunctionEnd
!endif

; **************************************************************************
; *  Helper Function: Move registry keys out of a file onto stack, $R0 holds file name
; **************************************************************************
!ifdef DOREGFILE
	Function RegFileToStack
		ClearErrors ; Reset error flag
		Push "EndOfStack" ; set end marker for stack
		FileOpen "$0" "$R0" r
			StartRegFileRead:
				FileRead "$0" "$R0"
				StrCpy "$R0" "" ;clear variable
				FileReadSpecial:
					FileReadByte "$0" "$1"
					IfErrors EndRegFileRead ; end of file
					IntCmp "$1" 0x00 FileReadSpecial ;remove all zero bytes
					IntCmp "$1" 0x0a FileReadSpecial ;remove cursor return
					IntCmp "$1" 0x0d FileReadSpecialEnd ;end of line
					IntFmt "$1" "%c" "$1" ;convert to character
					StrCpy "$R0" "$R0$1" ;combine characters
					Goto FileReadSpecial ;read next character
				FileReadSpecialEnd:
				StrCpy "$9" "$R0" 5
				StrCmp "$9" "[HKEY" 0 StartRegFileRead ; only work on lines containing a registry key
					StrCpy "$R0" "$R0" "" 1 ; remove first "["
					${WordFind} "$R0" "]" "+01" "$R0" ; remove last "]" and everything behind -> only registry key is left
					StrCmp "$R1" "" DoRegFileWork ; start with very first registry key
						StrLen "$9" "$R1"
						StrCpy "$R2" "$R0" "$9"
						StrCmp "$R1" "$R2" StartRegFileRead ; if current registry key is a subkey of the key read before read next key
					DoRegFileWork:
					Push "$R0" ; put actual unique registry to stack
					StrCpy "$R1" "$R0\"
				Goto StartRegFileRead
			EndRegFileRead:
		FileClose "$0"
	FunctionEnd
!endif

; ##########################################################################
; #  - README --- README --- README --- README --- README --- README -
; ##########################################################################
; The following section could be deleted for own launchers, or you might want
; to create a readme.txt out of it.
/*------------------- cut here ------------------------------------------------
Portable Application Template
=============================
Copyright (c) 2007 Karl Loncarek (for the template)

Latest version of this template could be downloaded at:
http://www.loncarek.de/downloads/PortableApplicationTemplate.zip

ABOUT Portable Application Template
===================================
This template is intended to help developers or interested users to easily
create launchers for applications to make them portable. Those applications
could be open source or shareware or even commercial applications. The two
latter ones may be interesting for personal use. Thus almost every
application could be run from e.g. USB drive no matter what drive letter is
used, i.e. without leaving any information or traces on the host PC.
 
HOW TO ADAPT THE TEMPLATE TO YOUR NEEDS
=======================================
The key problem of "normal" applications is that they leave traces on the
computer that they are running on. Those traces could be registry keys,
directories with files, or setting files (e.g. .INI files) themselves.
This template is a NSIS source. It uses the same technique as all the
applications that could be found on http://portableapps.com .

First of all you have to trace what changes are done on your system when
installing an application. Personally I use the freeware version of Total
Uninstall, but you can use any other tracing software you like.
You could also use Sysinternals Filemon or Regmon application for this task.

Then list the changed/created folders, files, registry keys in the constants
REGKEYS, SETTINGSFILES, SETTINGSDIRS. Separate multiple entries with "||".

When you set USEREGKEYSFILE to "TRUE" (default) an existing file "Registry.reg"
within the data directory will override the settings done in REGKEYS. In this
case the parent registry keys within "Registry.reg" will be processed
recursively (all subkeys of the parent keys are saved automatically). Child
registry keys are ignored for processing. The file "Registry.reg" has to be of
windows regedit format. Also UNICODE format created by regedit version 5 will
be read. You could create it with regedit.exe itself or with any
other application that saves monitored registry keys in that format (e.g. Total
Uninstall). The launcher itself creates files in regedit (version 4) format. 
Not only the registry keys are used but also the saved settings which then will
be the actual portable registry settings. Just take care that the parent
registry keys within "Registry.reg" are directly followed by their child
registry keys. Two byte characters within the file registry values are not
supported! (But I haven't seen then yet in the registry, therefor no big deal.)
The registry values will always be saved within a file "Registry.reg" within
the Data directory.

Folders or files written to the users profile directory are automatically
redirected if the constant REDIRECTUSERPROFILE is set to true. So there is no
need to add them in the above lists.
Beware of applications that launch other applications. Those applications
started by your portable program will also use the changed user profile folder.
In such a case it is recommended to comment out REDIRECTUSERPROFILE or set it
to something different than "TRUE". Then automatic redirection is disabled.
If the application to launch does not create those files on the first start
then they have to be copied manually into the "UserProfile" directory. The
"UserProfile" could be found within your data directory depending on your
directory structure (see below).

You could hand over some additional default command line parameters to your
application to launch. Simply set them within EXEPARMS. Those settings will be
overridden by command line parameters given in an INI file. The highest
priority have command line parameters given to the launcher when running it.
All other settings within the INI or defined in EXEPARMS will then be ignored.

On some applications additional changes in e.g. configuration files have to be
done, mainly regarding path settings. You have to check if you could use
relative paths (the easiest thing as you don't have to do anything else) or if
you have to change a drive letter each time you start the launcher. This has
to be configured manually and should be done only by intermediate/advanced
users who have some knowledge on NSIS.

If you want to create a launcher for a commercial/shareware application you
have first to take care whether the license of that application allows it. This
is in your responsibility. Do not create a package which contains the
commercial files as this would be illegal in most cases. Since version 1.9 this
template integrates some functions that help you achieving that task. You can
simply share the EXE created from this source. All the application and data
directories are created automatically. Even if your portable application folder
is empty (or at least does not contain the EXE) you will be prompted for a
folder that holds your locally installed application and copies all the files
wihtin it (inculding subdirectories). When you have set the constant UPXPARMS
all .exe and .dll files will be compressed using UPX. You will be prompted for
the location of upx.exe. Use the most current version of UPX.
Existing settings (defaults files) will be copied when the launcher runs for
the first time. You should not uninstall the local application before that.

If you want to share your sources with your launcher simple set the constant
INSTALLSOURCES to "TRUE". Then those files will be packed into the launcher.
The launcher then acts as launcher and installer! The source files will then be
copied automatically into the appropriate folders. An existing "readme.txt"
will be copied also to the sources. If it does not exist a warning will occur
when compiling. Do not worry about this warning, it's more a kind of an
information.
The same is done with the constant INSTALLDEFAULTS. The only diffrence: It
copies some default settings (currently only the file "Registry.reg") into the
appropriate folders. Also an existing "Registry.reg" will be installed into the data
directory.

To enable the application to be used with future versions of portableapps.com
menu set the constant PAFCOMPATIBILITY to "TRUE". Then a special directory
AppInfo and DefaultData is created automatically if it does not exist yet. In
the directory DefaultData the default settings are copied. The icon in AppInfo
will be the same as the one used for this launcher. Additionally a file
"appinfo.ini" will be extracted. If you haven't included one when compiling
this script it will be created out of some constants you could define at the
beginning (following directly after PAFCOMPATIBILITY). Filling those constants
is optional. Some values are set by default.

Beware also of settings which are only possible with administrator rights.
It might happen that you don't have adminstrator rights on the host computer.
In such a case the application might not run. But you have the possibility to
check whether the local user has administrator rights (necessary when e.g.
writing to the HKLM registry key). Simply set ADMINREQUIRED to "TRUE". If not
required comment it out or set it to something different than "TRUE".

With GTKVERSION you could set the version number of the GTK installation your
portable application requires. With USEJAVAVERSION you could do the same for
JAVA. Java and/or GTK could also be installed into a common files folder which
allows multiple applications to use the same or several JAVA/GTK versions.
See below at the directory structure on mor information.

For your reference:
List of possible root keys for the registry value (use short version as much
as possible to save space. The list is taken from the NSIS help)
 HKCC or HKEY_CURRENT_CONFIG
 HKCR or HKEY_CLASSES_ROOT
 HKCU or HKEY_CURRENT_USER
 HKDD or HKEY_DYN_DATA
 HKLM or HKEY_LOCAL_MACHINE
 HKPD or HKEY_PERFORMANCE_DATA
 HKU or HKEY_USERS
 SHCTX or SHELL_CONTEXT

LICENSE OF TEMPLATE
===================
           Copyright© 2006-2007 Karl Loncarek <karl [at] loncarek [dot] de>
           All rights reserved.

           Redistribution and use in source and binary forms, with or without
           modification, are permitted provided that the following conditions
           are met:

             1. Redistributions of source code must retain the above copyright
                notice, this list of conditions and the following disclaimer.
             2. Redistributions in binary form must reproduce the above
                copyright notice, this list of conditions and the following
                disclaimer in the documentation and/or other materials
                provided with the distribution.

           THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
           "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
           LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
           FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
           COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
           INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
           (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
           SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
           HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
           STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
           ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
           OF THE POSSIBILITY OF SUCH DAMAGE.

REQUIREMENTS
============
In order to compile this script with NSIS you need the following Plugins:
- NewAdvSplash 	(http://nsis.sourceforge.net/NewAdvSplash_plug-in)
- Registry 		(http://nsis.sourceforge.net/Registry_plug-in)
- FindProc 		(http://nsis.sourceforge.net/Find_Process_By_Name)
- Dialogs 		(http://nsis.sourceforge.net/Dialogs_plug-in)
And of course you need NSIS (http://nsis.sourceforge.net)

INSTALLATION / DIRECTORY STRUCTURE
==================================
This template uses the same directory structure as John T. Haller did with most
of the packages at http://www.portableapps.com to keep some kind of uniformity.

Below <NAME> is used for your applications name.
 
By default the program expects one of these directory structures:
(The first version is default and should be preferred)

+-\CommonFiles\ (optional)
| +-\GTK\
| | +-\<GTK_VERSION>\ (optional, files/directories of GTK installation (bin, lib, ...)
| +-\JAVA\
|   +-\<JRE_VERSION>\ (optional, files/directories of JRE installation (bin, lib, ...)
+-\ <--- Directory with <NAME>Portable.exe
  +-\App\
  | +-\<NAME>\ <--- All files and directories of a local installation
  | +-\GTK\ (optional, files/directories of GTK installation (bin, lib, ...)
  | +-\JAVA\ (optional, files/directories of JRE installation (bin, lib, ...)
  +-\Data\ (optional, will be created automatically when needed)
  | +-\Settings\ (optional, will be created automatically when needed)
  | +-\UserProfile\ (optional, will be created automatically when needed)
  +-\Other\ (optional) <--- contains additional information
    +-\<NAME>Sources\ (optional)
    +-\<NAME>PortableSources\ (optional) <--- contains source of launcher

OR

+-\ <--- Directory with Portable<NAME>.exe
  +-\CommonFiles\ (optional)
  | +-\GTK\
  | | +-\<GTK_VERSION>\ (optional, files/directories of GTK installation (bin, lib, ...)
  | +-\JAVA\
  |   +-\<JRE_VERSION>\ (optional, files/directories of JRE installation (bin, lib, ...)
  +-\<NAME>Portable\
    +-\App\
    | +-\<NAME>\ <--- All files and directories of a local installation
    | +-\GTK\ (optional, files/directories of GTK installation (bin, lib, ...)
    | +-\JAVA\ (optional, files/directories of JRE installation (bin, lib, ...)
    +-\Data\ (optional, will be created automatically when needed)
    | +-\Settings\ (optional, will be created automatically when needed)
    | +-\UserProfile\ (optional, will be created automatically when needed)
    +-\Other\ (optional) <--- contains additional information
      +-\<NAME>Sources\ (optional)
      +-\<NAME>PortableSources\ (optional) <--- contains source of launcher

OR

+-\ <--- Directory with Portable<NAME>.exe
  +-\PortableApps\
    +-\CommonFiles\ (optional)
    | +-\GTK\
    | | +-\<GTK_VERSION>\ (optional, files/directories of GTK installation (bin, lib, ...)
    | +-\JAVA\
    |   +-\<JRE_VERSION>\ (optional, files/directories of JRE installation (bin, lib, ...)
    +-\<NAME>Portable\
      +-\App\
      | +-\<NAME>\ <--- All files and directories of a local installation
      | +-\GTK\ (optional, files/directories of GTK installation (bin, lib, ...)
      | +-\JAVA\ (optional, files/directories of JRE installation (bin, lib, ...)
      +-\Data\ (optional, will be created automatically when needed)
      | +-\Settings\ (optional, will be created automatically when needed)
      | +-\UserProfile\ (optional, will be created automatically when needed)
      +-\Other\ (optional) <--- contains additional information
        +-\<NAME>Sources\ (optional)
        +-\<NAME>PortableSources\ (optional) <--- contains source of launcher

Which directory will be scanned for the JAVA/GTK version that will be used
(highest priority first):
A. "App\JAVA" or "App\GTK"
B. "CommonFiles\JAVA\<JRE_VERSION>" or "CommonFiles\GTK\<GTK_VERSION>"
C. "CommonFiles\JAVA" or "CommonFiles\GTK"
D. on the host computer installed JAVA or GTK

Remarks: When there is no version within A or B the version within C will be
used. It is highly recommended to use B as far as possible if you want to share
it with other portable applications. If your application does not require a
special GTK/JAVA version it is recommended to use C with the newest available
version of GTK/JAVA. <JRE_VERSION> should be only the version number e.g.
"1.6.0_01", otherwise it won't be found. A message will appear when the hosts
JAVA/GTK will be used.

It can be used in other directory configurations by including the
<NAME>Portable.INI in the same directory as the launcher <NAME>Portable.EXE.
Details for the configuration settings in the INI files could be found below.
The INI file may also be placed in the subdirectories "Portable<NAME>",
"PortableApps\<NAME>Portable", "Apps\<NAME>Portable", or
"Data\<NAME>Portable". Those directories are relative to the location of the
<NAME>Portable.EXE file. All paths given in the INI-file should be relative
to the location of <NAME>Portable.EXE.

.INI FILE CONFIGURATION OPTIONS
===============================
The launcher will look for an .INI file within it's directory. If you are
happy with the default options, you won't need it. The INI file is formatted
as follows (Below <NAME> is used for your applications name.):

[<NAME>Portable]
ProgramExecutable=<NAME>.exe
ProgramParameters=
ProgramDirectory=App\<NAME>
DataDirectory=Data
SplashScreen=enabled
ExtractSources=TRUE
JAVAVersion=1.6.0_01

Required entries are ProgramDirectory and DataDirectory. If others are
missing or empty the default values will be used.
All paths provided should be RELATIVE to the "<NAME>Portable.exe".

ProgramExecutable:
	Allows to launch a different EXE file of your application
ProgramParameters:
	Additonal parameters that should be used when launching the application.
	This setting overrides te defaults.
ProgramDirectory:
	The path to the EXE file of your application that should be launched. But
	this is only the path, thus must not contain a filename.
DataDirectory:
	The path where all settings should be stored (registry keys and the
	above subdirectories)
SplashScreen:
	Controls whether the splash screen is showed upon start of the launcher
	if one is defined by default. Anything but "enabled" disables it.
ExtractSources:
	Controls whether the launcher acts also as installer, i.e. will install
	sources and readme.txt (or other files) if they do not exist yet.
	Anything else than TRUE will not install/extract any files
GTKVersion:
	Tells the launcher which version of GTk should be used. See above for
	version handling.
JAVAVersion:
	Tells the launcher which version of JAVA should be used. Well in fact
	it's the JRE version. See above for version handling.

CURRENT LIMITATIONS
===================
WRITE ACCESS REQUIRED - The data directory must be writeable on your drive
(e.g. USB drive) as all the settings and configurations are saved there.

HISTORY
=======
1.0 -	First release: Based on Quickport NSIS template developed by Deuce
	added capability to backup multiple files/registry keys
1.1 -	INI files and some default directory structures are supported now.
1.2 -	added sleep time when deleting registry keys; adopted new directory
	structure as recently used by John (AppPortable instead of PortableApp)
1.3 -	fixed a bug
1.4 -	added handling for second launch; some bugfixes
1.5 -	complete rewrite for easier understanding of the source; original
	files/directories are now renamed instead of copied; error message when
	backup files/folders exist; Readme now included at end of the script
1.6 -	no need to define any folder/file within SETTINGSFILES or
	SETTINGSDIRS that is normally stored within the users profile folder.
	Those folders/files will now be stored on the portable drive
	automatically (i.e. redirected)!
1.7 -	added creation of other (maybe missing) data folders; made user profile
	redirection switchable, documentation updates
1.8 -	added check whether user has local administrator rights; removed
	"Unused WordFunc" warning during compile time; optimized compiling when
	commenting out some constants was forgotten ( to get smallest EXE you
	should really comment out); bugfixes
1.9 -	when no directories are found a default directory structure is created;
	when no EXE is found you are asked to select a folder from which to
	copy the files; added possibility to integrate the sources if you want
	to distribute only the exe, e.g. when creating commercial/shareware
	applications launchers -> launcher is an installer at the same time;
	added switch for INI file to disable that behaviour;
	optimzed source code
2.0 -	registry keys are now stored in one single file even with multiple
	registry keys (not compatible to previous versions); Overrides setting
	of REGKEYS 	if USEREGKEYSFILE is "TRUE" and reads registry keys
	to process from file "Registry.reg"; Added "Registry.reg" to source
	installation; Added default command line parameters; removed one
	possible directroy structure (which did not make sense); added GTK and
	JAVA (or better JRE) support, adjustable via INI; automatic detection
	of a "CommonFiles" directory; some code cleanup
2.1 - minor bugfixes; Added option to delete registry keys before applying own
	(saved) registry keys; set default Execution Level to user (for VISTA)
2.2 - fixed newadvsplash DLL being left in the temp directory (forgot cleanup);
	changed way of restoring the registry keys (now uses "regedit /s");
	removed sleeps again (commented them out, just in case...);
	removed /D=2 switch when saving registry keys
2.3 - added a lot of "" around the variables to avoid errors when leaving
	variables empty instead of commenting them out; Cleaned also errors
	before doing regsitry stuff; added error messages when something
	goes wrong when saving registry keys
2.4 - added extraction/creation of AppInfo folder and content to be PAF
	compatible (see portableapps.com); added some error handling; added
	capability to read also Registry.reg in UNICODE format; added more
	options for creating a standalone launcher/installer
2.5 - changed cleanup of newadvsplash to reflect changes to newest version;
	fixed some wording in docs; fixed commandline parameters being given as
	one string
*/##########################################################################
# End of file
##########################################################################
