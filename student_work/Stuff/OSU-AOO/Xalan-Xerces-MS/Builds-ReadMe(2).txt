
===========================================================================
WINDOWS BATCH FILES TO BUILD FROM SOURCE DISTRIBUTIONS THE APACHE LIBRARIES
    For XERCES-C/C++ (XML Parser) and XALAN-C/C++ (XSLT Transformer)
===========================================================================

Scripts support the Windows builds of these source distributions.

   Xalan-C 1.10  with  Xerces-C 2.7.0
      VC6, VC7.1, VC8

   Xalan-C 1.10  with  Xerces-C 2.8.0
      VC6, VC7.1, VC8

   Xalan-C 1.11  with  Xerces-C 3.1.1
      VC7.1, VC8, VC9, VC10

The build scripts assume that you have one or more of the Microsoft
Visual Studio development platform installed on your Windows system.

I have (VC6 and VC7.1) installed on an XP virtual machine.

I have (VC8, VC9, and VC10) installed on a Win-7 virtual machine.

      VC6   = Microsoft Visual Studio
      VC7.1 = Microsoft Visual Studio .NET 2003
      VC8   = Microsoft Visual Studio .NET 2005
      VC9   = Microsoft Visual Studio .NET 2008
      VC10  = Microsoft Visual Studio .NEt 2010
      VC11  = Microsoft Visual Studio .Net 2012 (Not yet supported)

Directory Path for performing the builds is coded into the scripts.

    C:\Apache\xalan-builds\

The build scripts and source distributions should be copied to this
directory.


Both the Xerces-C and Xalan-C libraries should be built using the
same version of Microsoft Visual Studio.  Otherwise there will be
severe compatibility issues.

A complete build of Xerces-C/C++ is required before the build of
Xalan-C/C++ libraries are attempted.

which contains the command-line bat files and related source
distributions for XERCES-C and XALAN-C.

-----------------
XERCES-C Sources

    xerces-src-27/ = Source Package Version (2.7.0)

    xerces-src-28/ = Source Package Version (2.8.0)

    xerces-src-31/ = Source Package Version (3.1.1)

-----------------
XALAN-C Sources

    xalan-src-10/  = Source Package Version (1.10.0)

    xalan-src-11/  = Version (1.11.0) extracted from subversion

      http://svn.apache.org/repos/asf/xalan/c/trunk
      with the contents of c/trunk/<files> 
      moved directly to c/<files>.

-----------------

Compiling Xerces-C/C++

  xerces-build-???.bat

Building a Xerces-C/C++ binary distribution

  xerces-inst-???.bat

  This creates package directories named XERCESCPKG-???
  You can copy this to a compressed folder for distribution.

-----------------

Compiling Xalan-C/C++

  xalan-build-???.bat

Building a Xalan-C/C++ binary distribution

  xalan-inst-???.bat

  This creates package directories named XALANCPKG-???
  You can copy this to a compressed folder for distribution.

-----------------

The Build Scripts:

  The xerces-build-???.bat scripts setup the build environment
  for the Xerces source package and referenced Microsoft VC??.

  A LNK1112 error will be generated if you build a Win32 (x86)
  platform when the script is configured for Win64 (x64) builds.

  A LNK1112 error will be gerated if you build a Win64 (x64)
  platform when the script is configured for Win32 (x86) builds.

  The Win64 (x64) builds use the x86_amd64 cross-compile target.

  The xerces-inst-???.bat scripts create binary package directories
  known as XERCESCPKG-??? and populate the content from the results
  of the xerces-build-???.bat builds.

  The xalan-build-???.bat scripts setup the build environment
  for the Xalan source builds, referencing Xalan version,
  Xerces version, platform, and VC??? compiler.

  The xalan-inst-???.bat scripts create binary package directories
  known as XALANCPKG-??? and populate the content from the results
  of the xalan-build-???.bat builds.

The Sources:

  Copy the xerces-c (2.7.0) source package to directory xerces-src-27.

  Copy the xerces-c (2.8.0) source package to directory xerces-src-28.

  Copy the xerces-c (3.1.1) source package to directory xerces-src-31.

  Copy the xalan-c (1.10) source package to directory xalan-src-10.

  Create an xalan-c (1.11) source package to directory xalan-src-11.

Before Building Xalan:

  You should have successful builds of the related XERCES packages
  and have the binary package components installed into the related 
  XERCESCPKG-??? directories.

Template Scripts:

  xerces-build.bat   - from which xerces-build-???.bat are created
  xerces-inst.bat    - from which xerces-inst-???.bat are created
  xalan-build.bat    - from which xalan-build-???.bat are created
  xalan-inst.bat     - from which xalan-inst-???.bat are created

The *-build-* and *-inst-* scripts do not require parameters and are
useful to launch from the Windows File Explorer using the mouse buttons.

I plan to use these scripts to automate Microsoft builds for product
release using Windows platforms.

The template scripts are a good start, but can be readily modified to 
support stand-alone batch builds for the software products.

CAVEAT:

Set the APACHEHOME environment to be C:\Apache\xalan-builds\

The xalan-build-*VC60.bat and xalan-build-*VC71.bat files
require that the APACHEHOME environment be explicit and not
relative.  The localization project requires this.  

The new localization environment requires an installed Xerces-C 
library because a Xerces-C based messageloader program is created 
to process the XLIFF message file data.

=====================================
CREATE XALAN-C/C++ HTML DOCUMENTATION
=====================================

1. Copy the "bin" folder from your XERCES-C/C++ binary package to

   xalan-src-11/c/bin/

2. Copy the "bin" folder from your XALAN-C/C++ binary package to

   xalan-src-11/c/bin/

The preceding steps ensure that the "xalan.exe" command line executable
can be found for creating the HTML products from the StyleBook XML
markup files.

3. Connect to xalan-src-11/c/xdocs/sources

4. run the "make-book.bat" script

This will build the XALAN html pages into directory

   xalan-src-11/c/Build/docs/xalan-c

Contents of this directory will be usable by your favorite web browser.
Building the Xalan apiDocs interactive source code reference requires
a Linux or UNIX system with software packages for Doxygen and Graphviz
installed.


Sincerely,
Steven J. Hathaway
Xalan Documentation Project
    