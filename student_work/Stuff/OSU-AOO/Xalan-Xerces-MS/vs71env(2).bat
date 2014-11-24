SET PATH=%WINDIR%\system32;%WINDIR%;%WINDIR%\system32\Wbem
SET INCLUDE=
SET LIB=
SET LIBPATH=
SET SOURCE=

:: -- FOR (X86, Win32) BUILD 32-bit Platforms

call "%VS71COMNTOOLS%\vsvars32.bat"
:: call "%VS80COMNTOOLS%\vsvars32.bat"
:: call "%VS90COMNTOOLS%\vsvars32.bat"
:: call "%VS100COMNTOOLS%\vsvars32.bat"

REM  DEVENV path-to/solution.sln /USEENV

:: -- FOR (X64, Win64) BUILD 64-bit Platforms

:: call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
:: call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
:: call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64

REM  DEVENV path-to/solution.sln /USEENV


