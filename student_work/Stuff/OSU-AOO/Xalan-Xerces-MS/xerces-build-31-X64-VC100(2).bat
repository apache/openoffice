::
:: FILE TO LAUNCH VISUAL STUDIO.NET FOR XERCES BUILDS
::

:: SET SRCDIR=xerces-src-27
:: SET SRCDIR=xerces-src-28
SET SRCDIR=xerces-src-31

:: SET VCVER=VC6
:: SET VCVER=VC7.1
:: SET VCVER=VC8
:: SET VCVER=VC9
SET VCVER=VC10

:: SET ARCH=Win32
SET ARCH=Win64

IF "%VCVER%" == "VC6" (
  SET XSOLUTIONFILE=%SRCDIR%\Projects\Win32\%VCVER%\xerces-all\xerces-all.dsw
) ELSE (
  SET XSOLUTIONFILE=%SRCDIR%\Projects\Win32\%VCVER%\xerces-all\xerces-all.sln
)

:: -- SETUP ENVIRONMENT FOR devenv /USEENV

SET PATH=%WINDIR%\system32;%WINDIR%;%WINDIR%\system32\Wbem
SET INCLUDE=
SET LIB=
SET LIBPATH=
SET SOURCE=


IF NOT "%VCVER%" == "VC6" GOTO test_vc7
CALL "C:\Program Files\Microsoft Visual Studio\VC98\Bin\VCVARS32.BAT"
GOTO enter_build

:test_vc7
IF NOT "%VCVER%" == "VC7.1" GOTO test_vc8
CALL "%VS71COMNTOOLS%\vsvars32.bat"
GOTO enter_build

:test_vc8
IF NOT "%VCVER%" == "VC8" GOTO test_vc9
IF "%ARCH%" == "Win64" GOTO win64_vc8
CALL "%VS80COMNTOOLS%\vsvars32.bat"
GOTO enter_build

:win64_vc8
CALL "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64 
GOTO enter_build

:test_vc9
IF NOT "%VCVER%" == "VC9" GOTO test_vc10
IF "%ARCH%" == "Win64" GOTO win64_vc9
CALL "%VS90COMNTOOLS%\vsvars32.bat"
GOTO enter_build

:win64_vc9
CALL "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
GOTO enter_build

:test_vc10
IF NOT "%VCVER%" == "VC10" GOTO finish
IF "%ARCH%" == "Win64" GOTO win64_vc10
CALL "%VS100COMNTOOLS%\vsvars32.bat"
GOTO enter_build

:win64_vc10
CALL "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
GOTO enter_build

:: -- ENTER THE DEVELOPMENT ENVIRONMENT --

:enter_build

IF "%VCVER%" == "VC6" (
:: MSDEV %XSOLUTIONFILE% /MAKE "XercesLib - Win32" /USEENV /OUT "XercesLib-Output.log"
   MSDEV %XSOLUTIONFILE% /USEENV
) ELSE (
   devenv %XSOLUTIONFILE% /useenv
)

:finish