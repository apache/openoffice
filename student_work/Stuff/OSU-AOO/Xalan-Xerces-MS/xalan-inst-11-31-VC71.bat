::
:: FILE TO INSTALL XALANC TO BINARY PACKAGE CREATED FROM SOURCE
::


:: -- The XALAN-C Source Version {10, 11}

:: SET XALANC_VER=10
SET XALANC_VER=11

:: -- The XERCES-C Source Version {2.7.0=27, 2.8.0=28, 3.1.1=31}

:: SET XERCESC_VER=27
:: SET XERCESC_VER=28
SET XERCESC_VER=31

:: -- The Binary Target Architecture {X86=Win32, X64=Win64}

SET ARCH=Win32
:: SET ARCH=Win64

:: -- The Microsoft Studio C++ Version {6, 7.1, 8, 9, 10}

:: SET VCVER=VC6
SET VCVER=VC7.1
:: SET VCVER=VC8
:: SET VCVER=VC9
:: SET VCVER=VC10

:: -- Set the target package directory

PKGDIR=XALANCPKG

IF "%VCVER%" == "VC6" (
  SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-VC60
)

IF "%VCVER%" == "VC7.1" (
  SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-VC71
)

IF "%VCVER%" == "VC8" (
  IF "%ARCH%" == "Win64" (
    SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-X64-VC80
  ) ELSE SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-VC80
)

IF "%VCVER%" == "VC9" (
  IF "%ARCH%" == "Win64" (
    SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-X64-VC90
  ) ELSE SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-VC90
)

IF "%VCVER%" == "VC10" (
  IF "%ARCH%" == "Win64" (
    SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-X64-VC100
  ) ELSE SET PKGDIR=XALANCPKG-%XALANC_VER%-%XERCESC_VER%-VC100
)

:: -- The XALANC Source Directory {xalan-src-10\c, xalan-src-11\c}

SET SRCDIR=xalan-src-%XALANC_VER%\c

:: -- Create the Target Directory Tree

mkdir "%PKGDIR%"
mkdir "%PKGDIR%\bin"
mkdir "%PKGDIR%\lib"
mkdir "%PKGDIR%\include"
mkdir "%PKGDIR%\doc"
mkdir "%PKGDIR%\doc\xalanc"

:: -- Copy the LICENSE, NOTICE and README Files

copy "%SRCDIR%\LICENSE" "%PKGDIR%\doc\xalanc"
copy "%SRCDIR%\NOTICE"  "%PKGDIR%\doc\xalanc"
copy "%SRCDIR%\README"  "%PKGDIR%\doc\xalanc"

:: --- NOW TO COPY THE FILES FROM SOURCE REPOSITORY TO PACKAGE ---

:DebugBuild

IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug" GOTO ReleaseBuild

XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.exe" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.dll" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.pdb" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.lib" "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug\*.exp" "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Debug\NLS\Include\*.hpp" "%PKGDIR%\include"

:ReleaseBuild

IF NOT EXIST "%SRCDIR%\Build\%ARCH%\%VCVER%\Release" GOTO IncludeLibrary

XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.exe" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.dll" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.pdb" "%PKGDIR%\bin"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.lib" "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Release\*.exp" "%PKGDIR%\lib"
XCOPY /I /F /R /Y /C "%SRCDIR%\Build\%ARCH%\%VCVER%\Release\NLS\Include\*.hpp" "%PKGDIR%\include"

:: -- INCLUDE LIBRARY --

:IncludeLibrary

IF NOT EXIST "%SRCDIR%\src\xalanc" GOTO finish

XCOPY /I /F /R /Y /C /E "%SRCDIR%\src\xalanc" "%PKGDIR%\include\xalanc" /EXCLUDE:exclude-src.txt

:finish
