@ECHO OFF

:: *** TESTING THE PROPOGATION OF DEBUG LEVEL ENVIRONMENT ***

SET LEMSSOLUTION=lemsjx.sln

:: -------------------------------------------------------------------------
:: LEMS Directories
::
SET LEMSBASE=C:\lems2k-3.4.3\
SET LEMSDIR=%LEMSBASE%MSGSWITCH\
SET LEMSBIN=%LEMSDIR%bin
SET LEMSLIB=%LEMSDIR%lib

:: rem Setup LEMS/JX Debug Environment  *** TEST PROPOGATION OF DEBUG LEVEL ***
:: call C:\lems2k\lemsenv 9
 call C:\lems2k\lemsenv2 7
 set LEMSEXE=%LEMS%\bin
 set LEMSDB=LEMSLOCAL

:: Now to keep  Unisys LEMS .NET projects happy
:: $(LEMSBINDIR) and $(LEMSLIBDIR) need to be resolvable

SET LEMSBINDIR=%LEMSBIN%\
SET LEMSLIBDIR=%LEMSLIB%\

:: -------------------------------------------------------------------------
:: DELETE .NET 2003 Project Attributes - Used by Previous Jobs
::
erase "%USERPROFILE%\Local Settings\Application Data\Microsoft\Visual Studio\7.1\VCComponents.dat"
erase "%USERPROFILE%\Local Settings\Application Data\Microsoft\Visual Studio\7.1\toolbox.tbd"

:: -------------------------------------------------------------------------
:: Initialize Microsoft Studio .NET 2003 C/C++ environment variables
::

SET PATH=%WINDIR%\system32;%WINDIR%;%WINDIR%\Wbem
SET PATH=%PATH%;%LEMSEXE%
SET INCLUDE=
SET LIB=
SET LIBPATH=
SET SOURCE=

:: The LIBPATH and SOURCE environment variables are not set by the "vcvars32.bat"
:: When launching devenv.exe" /usenev, the values become empty.  Therefore we
:: should redefine them here for default .NET IDE consistency.

SET LIBPATH=$(FrameworkDir)$(FrameworkVersion)
SET SOURCE=$(VCInstallDir)atlmfc\src\mfc;$(VCInstallDir)atlmfc\src\atl;$(VCInstallDir)crt\src

call "%VS71COMNTOOLS%..\..\Vc7\bin\vcvars32.bat"

:: -------------------------------------------------------------------------
:: Unisys Definitions of Studio .NET Environment
::
:: SET VSBASE=C:\Program Files\Microsoft Visual Studio .NET 2003\
:: SET VSCBIN=%VSBASE%Vc\bin\
:: SET VSDEVENV=%VSBASE%Common7\IDE\
:: SET VSCPFSDK=%VSBASE%Vc7\PlatformSDK\
::

SET VSBASE=%VCINSTALLDIR%\
SET VSCBIN=%VCINSTALLDIR%\Vc7\bin\
SET VSDEVENV=%VSINSTALLDIR%
SET VSCPFSDK=%VCINSTALLDIR%\Vc7\PlatformSDK\

:: -------------------------------------------------------------------------
:: The Microsoft SQL Server 2000 SDK Environment
::

SET MSQLDIR=C:\Program Files\Microsoft SQL Server\
SET MSQLBIN=%MSQLDIR%MSSQL\Binn;%MSQLDIR%80\Tools\Binn
SET MSQLINC=%MSQLDIR%80\Tools\DevTools\Include
SET MSQLLIB=%MSQLDIR%80\Tools\DevTools\Lib

:: SET MSQLLEMSINC=%LEMSDIR%ut\include
:: SET MSQLLEMSINC=%MSQLLEMSINC%;%LEMSDIR%mp\include
:: SET MSQLLEMSINC=%MSQLLEMSINC%;%LEMSDIR%ir\include
:: SET MSQLLEMSINC=%MSQLLEMSINC%;%LEMSDIR%ip\include\hqcntlr
:: SET MSQLLEMSINC=%MSQLLEMSINC%;%LEMSDIR%ip\include\cmdservr
:: SET MSQLLEMSINC=%MSQLLEMSINC%;%LEMSDIR%dm\include


:: -------------------------------------------------------------------------
:: The SNA Microsoft Host Integration Server 2004 SDK Environment
::
::  SET MHISDIR="C:\Program Files\Microsoft Host Integration Server\SDK\

SET MHISDIR=%SNARoot%\..\SDK\
SET MHISBIN=%MHISDIR%bin
SET MHISINC=%MHISDIR%Include
SET MHISLIB=%MHISDIR%Lib

:: -------------------------------------------------------------------------
:: The Apache XML (Xerces/Xalan) Development Environment
::
::  XALANCROOT and XERCESCROOT are required if you have a source distribution
::  and need to access the source files in a debug environment.

SET APACHEDIR=C:\lems2k-3.4.3\Apache\
SET XALANCROOT=%APACHEDIR%XALANCPKG
SET XERCESCROOT=%APACHEDIR%XERCESCPKG

:: LEMS SOURCES REQUIRE $(XALANDIR)

SET XALANDIR=%XALANCROOT%

SET XALANBIN=%XALANCROOT%\bin
SET XALANINC=%XALANCROOT%\include
SET XALANLIB=%XALANCROOT%\lib

SET XERCESBIN=%XERCESCROOT%\bin
SET XERCESINC=%XERCESCROOT%\include
SET XERCESLIB=%XERCESCROOT%\lib

SET APACHEBIN=%XALANBIN%;%XERCESBIN%
SET APACHEINC=%XALANINC%;%XERCESINC%
SET APACHELIB=%XALANLIB%;%XERCESLIB%

:: -------------------------------------------------------------------------
:: The IBM WebSphere MQ Development Environment
::
:: SET WEBSPHDIR=D:\Program Files\IBM\WebSphere MQ\
:: SET WEBSPHLIB=%WEBSPHDIR%Tools\lib
:: SET WEBSPHINC=%WEBSPHDIR%Tools\c\include

:: -------------------------------------------------------------------------
:: Microsoft Message Queue (MSMQ)
::
:: SET MSMQINC=%VSCPFSDK%include

SET MSMQINC=%VCINSTALLDIR%\Vc7\PlatformSDK\include

:: -------------------------------------------------------------------------
:: The Java Development Environment - Sun Microsystems Java
::

:: SET JAVADIR=C:\j2sdk1.4.2_17\
SET JAVADIR=C:\jdk1.5.0_18\
SET JAVABIN=%JAVADIR%bin;%JAVADIR%\jre\bin;%JAVADIR%\jre\bin\client
SET JAVAINC=%JAVADIR%include;%JAVADIR%include\win32
SET JAVALIB=%JAVADIR%lib

:: SET JAVADIR=D:\jdk1.5.0_06\
:: SET JAVABIN=
:: SET JAVAINC=%JAVADIR%include
:: SET JAVALIB=%JAVADIR%lib

:: -------------------------------------------------------------------------
:: The Candle Roma Development Environment
::

:: CANDLE=D:\Candle\Roma\Toolkit\
:: SET CANDLEBIN=
:: SET CANDLEINC=%CANDLE%include
:: SET CANDLELIB=%CANDLE%Lib

:: -------------------------------------------------------------------------
:: Systinet Files
::

:: SYSTINETCDIR=D:\systinet553\
:: SYSTINETCBIN=%SYSTINETCDIR%bin
:: SYSTINETCINC=%SYSTINETCDIR%include
:: SYSTINETCLIB=%SYSTINETCDIR%bin

SET WASP_HOME=C:\systinet\server_java55
SET WASPBIN=%WASP_HOME%\bin

:: -------------------------------------------------------------------------
:: Construct the .NET development environment for LEMS Projects
::

SET PATH=%PATH%;%MSQLBIN%
SET PATH=%PATH%;%MHISBIN%
SET PATH=%PATH%;%LEMSBIN%
SET PATH=%PATH%;%XALANBIN%;%XERCESBIN%
SET PATH=%PATH%;%JAVABIN%
SET PATH=%PATH%;%WASPBIN%

SET INCLUDE=%INCLUDE%;%MSQLINC%
SET INCLUDE=%INCLUDE%;%MHISINC%
SET INCLUDE=%INCLUDE%;%XALANINC%;%XERCESINC%
:: SET INCLUDE=%INCLUDE%;%WEBSPHINC%
SET INCLUDE=%INCLUDE%;%JAVAINC%

SET LIB=%LIB%;%LEMSLIB%
SET LIB=%LIB%;%MSQLLIB%
SET LIB=%LIB%;%MHISLIB%
SET LIB=%LIB%;%XALANLIB%;%XERCESLIB%
:: SET LIB=%LIB%;%CANDLELIB%
:: SET LIB=%LIB%;%WEBSPHLIB%
SET LIB=%LIB%;%JAVALIB%

:: ================================================
:: Debug Our .NET Development Environment

echo " "
echo PATH     = %PATH%
echo " "
echo INCLUDE  = %INCLUDE%
echo " "
echo LIB      = %LIB%
echo " "
echo LIBPATH  = %LIBPATH%
echo " "
echo SOURCE   = %SOURCE%
echo " "

:: -------------------------------------------------
:: Launch the .NET 2003 using the current environment settings

call "%VSINSTALLDIR%\devenv.exe" "%LEMSSOLUTION%" /useenv

