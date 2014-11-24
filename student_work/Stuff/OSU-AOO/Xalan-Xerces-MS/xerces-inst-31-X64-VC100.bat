::
:: FILE TO INSTALL XERCES BINARY PACKAGE CREATED FROM SOURCE
::

:: SET VCVER=VC6
:: SET VCVER=VC7.1
:: SET VCVER=VC8
:: SET VCVER=VC9
SET VCVER=VC10

:: SET ARCH=Win32
SET ARCH=Win64

:: SET XERCESC_VER=27
:: SET XERCESC_VER=28
SET XERCESC_VER=31

:: -- XERCES-C Source Directory {xerces-src-27, xerces-src-28, xerces-src-31}

SET SRCDIR=xerces-src-%XERCESC_VER%

SET PKGDIR=XERCESCPKG

IF NOT "%VCVER%" == "VC6" GOTO test_vc7
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-VC60
GOTO do_install_pkg

:test_vc7

IF NOT "%VCVER%" == "VC7.1" GOTO test_vc8
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-VC71
GOTO do_install_pkg

:test_vc8

IF NOT "%VCVER%" == "VC8" GOTO test_vc9
IF "%ARCH%" == "Win64" GOTO win64_vc8
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-VC80
GOTO do_install_pkg

:win64_vc8
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-X64-VC80
GOTO do_install_pkg

:test_vc9

IF NOT "%VCVER%" == "VC9" GOTO test_vc10
IF "%ARCH%" == "Win64" GOTO win64_vc9
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-VC90
GOTO do_install_pkg

:win64_vc9
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-X64-VC90
GOTO do_install_pkg

:test_vc10

IF NOT "%VCVER%" == "VC10" GOTO finish
IF "%ARCH%" == "Win64" GOTO win64_vc10
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-VC100
GOTO do_install_pkg

:win64_vc10
SET PKGDIR=XERCESCPKG-%XERCESC_VER%-X64-VC100
GOTO do_install_pkg

:do_install_pkg

mkdir "%PKGDIR%"
mkdir "%PKGDIR%\bin"
mkdir "%PKGDIR%\lib"

:: --- NOW TO COPY THE FILES FROM SOURCE REPOSITORY TO PACKAGE ---

:StaticDebugBuild
IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug" GOTO StaticDebugBuild2

XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug\*.exe "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug\*.dll "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug\*.pdb "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug\*.ilk "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug\*.lib "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticDebug\*.exp "%PKGDIR%\lib"

:StaticDebugBuild2
IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug" GOTO StaticReleaseBuild

XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug\*.exe" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug\*.dll" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug\*.pdb" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug\*.ilk" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug\*.lib" "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Debug\*.exp" "%PKGDIR%\lib"

:StaticReleaseBuild
IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease" GOTO StaticReleaseBuild2

XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease\*.exe "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease\*.dll "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease\*.pdb "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease\*.ilk "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease\*.lib "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\StaticRelease\*.exp "%PKGDIR%\lib"

:StaticReleaseBuild2
IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release" GOTO DebugBuild

XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release\*.exe" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release\*.dll" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release\*.pdb" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release\*.ilk" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release\*.lib" "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Static Release\*.exp" "%PKGDIR%\lib"

:: -- DEBUG BUILD --

:DebugBuild
IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug" GOTO ReleaseBuild

XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.exe "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.dll "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.pdb "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.ilk "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.lib "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.exp "%PKGDIR%\lib"

:: -- RELEASE BUILD --

:ReleaseBuild
IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\Release" GOTO IncludeLibrary

XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.exe "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.dll "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.pdb "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.ilk "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.lib "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C %SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.exp "%PKGDIR%\lib"

:: -- INCLUDE LIBRARY --

:IncludeLibrary

XCOPY /I /F /R /Y /C /E %SRCDIR%\src\xercesc  "%PKGDIR%\include\xercesc" /EXCLUDE:exclude-src.txt

:finish
