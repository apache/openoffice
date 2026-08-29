#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************



PRJ=..

PRJNAME=desktop
TARGET=soffice
TARGETTYPE=GUI
LIBTARGET=NO
GEN_HID=TRUE
GEN_HID_OTHER=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk

UWINAPILIB =

VERINFONAME=verinfo

# --- Resourcen ----------------------------------------------------

.IF "$(GUI)" == "WNT"
RCFILES=verinfo.rc
.ENDIF
.IF "$(GUI)" == "OS2"
RCFILES=ooverinfo2.rc
.ENDIF

# --- Linken der Applikation ---------------------------------------

.IF "$(OS)" == "MACOSX"
LINKFLAGSAPPGUI!:=	$(LINKFLAGSAPPGUI:s/-bind_at_load//)
.ENDIF # MACOSX

#.IF "$(OS)" == "LINUX" || "$(OS)" == "FREEBSD" || "$(OS)" == "NETBSD"
## #74158# linux needs sal/vos/tools at end of link list, solaris needs it first,
## winXX is handled like solaris for now
#APP1_STDPRE=
#APP1_STDPOST=$(CPPULIB) $(CPPUHELPERLIB) $(UNOLIB) $(TOOLSLIB) \
#	$(VOSLIB) $(SALLIB)
#.ELSE
#APP1_STDPRE=$(SALLIB) $(VOSLIB) $(TOOLSLIB) $(UNOLIB) $(CPPULIB) \
#	$(CPPUHELPERLIB)
#APP1_STDPOST=
#.ENDIF

RESLIB1NAME=		dkt
RESLIB1IMAGES=		$(PRJ)$/res
RESLIB1SRSFILES=	$(SRS)$/desktop.srs \
                    $(SRS)$/wizard.srs

.IF "$(L10N_framework)"==""
.IF "$(LINK_SO)"=="TRUE"
.IF "$(GUI)" != "OS2"
APP1TARGET=so$/$(TARGET)
APP1NOSAL=TRUE
APP1RPATH=BRAND
APP1OBJS=$(OBJ)$/copyright_ascii_sun.obj $(OBJ)$/main.obj
APP1STDLIBS = $(SALLIB) $(SOFFICELIB)
APP1DEPN= $(APP1RES) verinfo.rc

.IF "$(GUI)" == "WNT"
APP1RES=    $(RES)$/desktop.res
APP1ICON=$(SOLARRESDIR)$/icons/so9_main_app.ico
APP1VERINFO=verinfo.rc
APP1LINKRES=$(MISC)$/$(TARGET)1.res
APP1STACK=10000000

# create a manifest file with the same name as the
#office executable file soffice.exe.manifest
#$(BIN)$/$(TARGET).exe.manifest: template.manifest
#$(COPY) $< $@

.ENDIF # WNT

.ENDIF # "$(GUI)" != "OS2"

.ENDIF # "$(LINK_SO)"=="TRUE"

APP5TARGET=soffice
APP5NOSAL=TRUE
APP5RPATH=BRAND
APP5OBJS=$(OBJ)$/copyright_ascii_ooo.obj $(OBJ)$/main.obj
APP5STDLIBS = $(SALLIB) $(SOFFICELIB)
.IF "$(OS)" == "LINUX"
APP5STDLIBS+= -lXext -lX11
#APP5STDLIBS+= -lXext -lSM -lICE
.ENDIF # LINUX

APP5DEPN= $(APP1TARGETN) $(APP5RES) ooverinfo.rc
APP5DEF=    $(MISCX)$/$(TARGET).def

.IF "$(GUI)" == "WNT"
APP5RES=    $(RES)$/oodesktop.res
APP5ICON=$(SOLARRESDIR)$/icons/ooo3_main_app.ico
APP5VERINFO=ooverinfo.rc
APP5LINKRES=$(MISC)$/ooffice5.res
APP5STACK=10000000
.ENDIF # WNT

.IF "$(GUI)" == "OS2"
APP5DEF= # automatic
APP5RES=    $(RES)$/oodesktop.res
APP5ICON=$(SOLARRESDIR)$/icons/ooo-main-app.ico
APP5VERINFO=ooverinfo2.rc
APP5LINKRES=$(MISC)$/ooffice.res
.ENDIF # OS2

.IF "$(GUI)" == "WNT"
.IF "$(LINK_SO)"=="TRUE"
APP6TARGET=so$/officeloader
APP6RES=$(RES)$/soloader.res
APP6NOSAL=TRUE
APP6DEPN= $(APP1TARGETN) $(APP6RES) verinfo.rc
APP6VERINFO=verinfo.rc
APP6LINKRES=$(MISC)$/soffice6.res
APP6ICON=$(SOLARRESDIR)$/icons/so9_main_app.ico
APP6OBJS = \
    $(OBJ)$/extendloaderenvironment.obj \
    $(OBJ)$/officeloader.obj \
    $(SOLARLIBDIR)$/pathutils-obj.obj
STDLIB6=$(ADVAPI32LIB) $(SHELL32LIB) $(SHLWAPILIB)
.ENDIF # "$(LINK_SO)"=="TRUE"

APP7TARGET=officeloader
APP7RES=$(RES)$/ooloader.res
APP7NOSAL=TRUE
APP7DEPN= $(APP1TARGETN) $(APP7RES) ooverinfo.rc
APP7VERINFO=ooverinfo.rc
APP7LINKRES=$(MISC)$/ooffice7.res
APP7ICON=$(SOLARRESDIR)$/icons/ooo3_main_app.ico
APP7OBJS = \
    $(OBJ)$/extendloaderenvironment.obj \
    $(OBJ)$/officeloader.obj \
    $(SOLARLIBDIR)$/pathutils-obj.obj
STDLIB7=$(ADVAPI32LIB) $(SHELL32LIB) $(SHLWAPILIB)
.ELIF "$(OS)" == "MACOSX"
APP6TARGET=officeloader
APP6NOSAL=TRUE
APP6RPATH=BRAND
APP6OBJS=$(OBJ)$/copyright_ascii_ooo.obj $(OBJ)$/officeloader.obj
APP6STDLIBS = $(SALLIB)
APP5DEPN= $(APP1TARGETN) $(APP5RES) ooverinfo.rc
APP5DEF=    $(MISCX)$/$(TARGET).def
.ENDIF # WNT

.ENDIF

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(L10N_framework)"==""

.IF "$(APP1TARGETN)"!=""
$(APP1TARGETN) :  $(MISC)$/binso_created.flg
.ENDIF			# "$(APP1TARGETN)"!=""

.IF "$(APP5TARGETN)"!=""
$(APP5TARGETN) :  $(MISC)$/binso_created.flg
.ENDIF			# "$(APP6TARGETN)"!=""

.IF "$(APP6TARGETN)"!=""
$(APP6TARGETN) :  $(MISC)$/binso_created.flg
.ENDIF			# "$(APP6TARGETN)"!=""

.IF "$(GUI)" == "WNT"
ALLTAR: $(MISC)$/$(TARGET).exe.manifest
ALLTAR: $(MISC)$/$(TARGET).bin.manifest
ALLTAR: $(BIN)$/$(TARGET).bin
.IF "$(LINK_SO)"=="TRUE"
ALLTAR: $(BIN)$/so$/$(TARGET).bin
.ENDIF # "$(LINK_SO)"=="TRUE"
.ENDIF # WNT

.IF "$(GUI)" == "OS2"
ALLTAR: $(BIN)$/$(TARGET).bin
.ENDIF # OS2

$(BIN)$/soffice_oo$(EXECPOST) : $(APP5TARGETN)
	$(COPY) $< $@

.IF "$(GUI)" != "OS2"
.IF "$(LINK_SO)"=="TRUE"
$(BIN)$/so$/soffice_so$(EXECPOST) : $(APP1TARGETN)
	$(COPY) $< $@

ALLTAR : $(BIN)$/so$/soffice_so$(EXECPOST)
.ENDIF # "$(LINK_SO)"=="TRUE"
ALLTAR : $(BIN)$/soffice_oo$(EXECPOST)
.ENDIF

.IF "$(OS)" == "MACOSX"
.IF "$(LINK_SO)"=="TRUE"
$(BIN)$/so$/soffice_mac$(EXECPOST) : $(APP1TARGETN)
	$(COPY) $< $@

ALLTAR : $(BIN)$/so$/soffice_mac$(EXECPOST)
.ENDIF # "$(LINK_SO)"=="TRUE"

$(BIN)$/soffice_mac$(EXECPOST) : $(APP5TARGETN)
	$(COPY) $< $@

ALLTAR : $(BIN)$/soffice_mac$(EXECPOST)

.ENDIF # "$(OS)" == "MACOSX"

.IF "$(GUI)" == "WNT"

# create a manifest file with the same name as the
# office executable file soffice.exe.manifest
# COMEX 14 joins the simple branch.  The .ELSE below extracts the manifest the
# LINKER embedded and merges the template into it, and a modern MSVC embeds
# none -- mt.exe then fails with
#
#     general error c101008c: Failed to read the manifest from the resource
#     of file "soffice.exe".  The specified resource type was not found.
#
# Nothing is lost by not having it.  That fragment existed to declare the VC90
# CRT as a side-by-side assembly, and the UCRT is not one -- it is an operating
# system component on Windows 10.  The template carries everything else this
# executable needs, which is what the pre-VC9 branch has always relied on.
# template.manifest is checked in with the x86 architecture spelled out.  It is
# copied verbatim into whichever build is running, so on x64 it used to hand the
# process an activation context asking for the *x86* Common-Controls 6.0
# assembly.  Binding that into a 64 bit process pulls
# WinSxS\x86_microsoft.windows.common-controls\COMCTL32.dll, which cannot load,
# so every component that touches common controls failed to load -- fps.dll
# among them, which is why the native file dialogs silently fell back to the
# OpenOffice ones.  Substitute the architecture instead of copying.
.IF "$(CPUNAME)"=="X86_64"
MANIFEST_ARCH=amd64
.ELSE
MANIFEST_ARCH=x86
.ENDIF

.IF "$(CCNUMVER)" <= "001399999999" || "$(COMEX)" == "14"
$(MISC)$/$(TARGET).exe.manifest: template.manifest
   $(COMMAND_ECHO)$(SED) -e 's/processorArchitecture="x86"/processorArchitecture="$(MANIFEST_ARCH)"/g' $< > $@
.ELSE
$(MISC)$/$(TARGET).exe.template.manifest: template.manifest
   $(COMMAND_ECHO)$(SED) -e 's/processorArchitecture="x86"/processorArchitecture="$(MANIFEST_ARCH)"/g' $< > $@

$(MISC)$/$(TARGET).exe.linker.manifest: $(BIN)$/$(TARGET)$(EXECPOST)
   mt.exe -inputresource:$(BIN)$/$(TARGET)$(EXECPOST) -out:$@

$(MISC)$/$(TARGET).exe.manifest: $(MISC)$/$(TARGET).exe.template.manifest $(MISC)$/$(TARGET).exe.linker.manifest
   mt.exe -manifest $(MISC)$/$(TARGET).exe.linker.manifest $(MISC)$/$(TARGET).exe.template.manifest -out:$@
.ENDIF

# create a manifest file with the same name as the
# office executable file soffice.bin.manifest
# Same condition as above, and for the same reason.
.IF "$(CCNUMVER)" <= "001399999999" || "$(COMEX)" == "14"
$(MISC)$/$(TARGET).bin.manifest: template.manifest
   $(COMMAND_ECHO)$(SED) -e 's/processorArchitecture="x86"/processorArchitecture="$(MANIFEST_ARCH)"/g' $< > $@
.ELSE
$(MISC)$/$(TARGET).bin.manifest: $(MISC)$/$(TARGET).exe.manifest
   $(COPY) $(MISC)$/$(TARGET).exe.manifest $@
.ENDIF

$(BIN)$/$(TARGET).bin: $(BIN)$/$(TARGET)$(EXECPOST)
   $(COPY) $< $@

$(BIN)$/so$/$(TARGET).bin: $(BIN)$/so$/$(TARGET)$(EXECPOST)
   $(COPY) $< $@

.ENDIF # WNT

.IF "$(GUI)" == "OS2"
$(BIN)$/$(TARGET).bin: $(BIN)$/$(TARGET)$(EXECPOST)
   $(COPY) $< $@
.ENDIF # OS2

$(MISC)$/binso_created.flg :
	@@-$(MKDIRHIER) $(BIN)$/so && $(TOUCH) $@

.ENDIF
