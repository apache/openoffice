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



PRJ=..$/..$/..

PRJNAME=testtools
TARGET=cli_cpp_bridgetest.uno
USE_DEFFILE=TRUE
NO_BSYMBOLIC=TRUE
ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO

# disable caching to avoid stale objects
# on version changes
CCACHE_DISABLE=TRUE
.EXPORT : CCACHE_DISABLE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
DLLPRE =
CFLAGSENABLESYMBOLS:=-Z7
# ------------------------------------------------------------------

#These tests are for Windows only
# ...and only when the CLI binding was actually built.  This whole directory is
# the C++/CLI half of bridgetest and follows cli_ure.  No makefile.pmk here,
# so DISABLE_CLI comes from the environment.
.IF "$(COM)" == "MSC" && "$(GUI)" == "WNT" && "$(DISABLE_CLI)" == ""

# Which syntax, and which mixed-mode runtime, are two separate questions.
# /clr:oldSyntax went away after VS2015, so a UCRT compiler gets plain -clr
# and this source is C++/CLI.  msvcmrt.lib is needed either way from VS2005
# on: it carries the managed module initializer, and without it the link
# fails on an unresolved .cctor.
.IF "$(CCNUMVER)" >= "001399999999" && "$(COMEX)" != "14"
CFLAGSCXX += -clr:oldSyntax -AI $(OUT)$/bin -AI $(SOLARBINDIR)
.ELSE
CFLAGSCXX += -clr -AI $(OUT)$/bin -AI $(SOLARBINDIR)
.ENDIF

SHL1STDLIBS = \
	mscoree.lib
.IF "$(CCNUMVER)" >= "001399999999"
SHL1STDLIBS += \
	msvcmrt.lib
.ENDIF
SLOFILES= \
	$(SLO)$/cli_cpp_bridgetest.obj

SHL1OBJS = $(SLOFILES)

SHL1TARGET = $(TARGET)

SHL1DEF = $(MISC)$/$(SHL1TARGET).def
DEF1NAME = $(SHL1TARGET)

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

# ...and only when the CLI binding was actually built.  This whole directory is
# the C++/CLI half of bridgetest and follows cli_ure.  No makefile.pmk here,
# so DISABLE_CLI comes from the environment.
.IF "$(COM)" == "MSC" && "$(GUI)" == "WNT" && "$(DISABLE_CLI)" == ""

ALLTAR : $(BIN)$/cli_bridgetest_inprocess.exe

#################################################################

CLI_URE = $(SOLARBINDIR)$/cli_ure.dll
CLI_URETYPES = $(SOLARBINDIR)$/cli_uretypes.dll
CLI_BASETYPES = $(SOLARBINDIR)$/cli_basetypes.dll
CLI_CPPUHELPER = $(SOLARBINDIR)$/cli_cppuhelper.dll
CLI_OOOTYPES = $(SOLARBINDIR)$/cli_oootypes.dll
CLI_TYPES_BRIDGETEST = $(BIN)$/cli_types_bridgetest.dll

CSCFLAGS = -warnaserror+

# csc defaults to anycpu, so on 64-bit Windows the test exe would start as
# a 64-bit process and then fail to load the mixed-mode assemblies, which
# follow the C++ build and are x86.  The error is a BadImageFormatException
# naming cli_cppuhelper, which does not sound like a bitness problem at all.
.IF "$(CPUNAME)" == "X86_64"
CLI_PLATFORM = x64
.ELSE
CLI_PLATFORM = x86
.ENDIF
CSCFLAGS += -platform:$(CLI_PLATFORM)
.IF "$(CCNUMVER)" <= "001399999999"
VBC_FLAGS = -warnaserror+
.ELSE
VBC_FLAGS = -nowarn:42030 -warnaserror+
.ENDIF
VBC_FLAGS += -platform:$(CLI_PLATFORM)
.IF "$(debug)" != ""
CSCFLAGS += -debug+ -checked+ -define:DEBUG -define:TRACE
VBC_FLAGS += -debug+ -define:DEBUG=TRUE -define:TRACE=TRUE
.ELSE
CSCFLAGS += -optimize
VBC_FLAGS += -optimize
.ENDIF


# C# ----------------------------------------------
$(BIN)$/cli_cs_testobj.uno.dll : \
		cli_cs_testobj.cs \
		cli_cs_multi.cs \
		$(CLI_BASETYPES) \
		$(CLI_URETYPES) \
		$(CLI_URE)
	$(CSC) $(CSCFLAGS) -target:library -out:$@ \
		-reference:$(CLI_TYPES_BRIDGETEST) \
		-reference:$(CLI_URE) \
		-reference:$(CLI_BASETYPES) \
 		-reference:$(CLI_URETYPES) \
		cli_cs_testobj.cs cli_cs_multi.cs

$(BIN)$/cli_cs_bridgetest.uno.dll : \
		cli_cs_bridgetest.cs \
		$(CLI_BASETYPES) \
		$(CLI_URETYPES) \
		$(CLI_URE)
	$(CSC) $(CSCFLAGS) -target:library -out:$@ \
		-reference:$(CLI_TYPES_BRIDGETEST) \
		-reference:$(CLI_URETYPES) \
		-reference:$(CLI_BASETYPES) \
		-reference:$(CLI_URE) \
		-reference:System.dll \
		cli_cs_bridgetest.cs

# Visual Basic ------------------------------------------
$(BIN)$/cli_vb_bridgetest.uno.dll : \
		cli_vb_bridgetest.vb \
		$(CLI_BASETYPES) \
		$(CLI_URETYPES) \
		$(CLI_URE)
	$(VBC) $(VBC_FLAGS) \
		-target:library \
		-out:$@ \
		-reference:$(CLI_URETYPES) \
		-reference:$(CLI_BASETYPES) \
		-reference:$(CLI_URE) \
		-reference:$(CLI_TYPES_BRIDGETEST) \
		-reference:System.dll \
		-reference:System.Drawing.dll \
		-reference:System.Windows.Forms.dll \
		cli_vb_bridgetest.vb

$(BIN)$/cli_vb_testobj.uno.dll : \
		cli_vb_testobj.vb \
		$(CLI_BASETYPES) \
		$(CLI_URETYPES) \
		$(CLI_URE)
	$(VBC) $(VBC_FLAGS) \
		-target:library \
		-out:$@ \
		-reference:$(CLI_BASETYPES) \
		-reference:$(CLI_URETYPES) \
		-reference:$(CLI_URE) \
		-reference:$(CLI_TYPES_BRIDGETEST) \
		-reference:System.dll \
		-reference:System.Drawing.dll \
		-reference:System.Windows.Forms.dll \
		cli_vb_testobj.vb

# The stamp has to name what it copies.  Without these prerequisites a
# rebuilt cli_uretypes.dll never reaches $(BIN), and the test then runs
# against the stale copy -- which fails at assembly-load time, a long way
# from the cause.
$(MISC)$/copyassemblies.done .ERRREMOVE: \
		$(CLI_CPPUHELPER) \
		$(CLI_BASETYPES) \
		$(CLI_URETYPES) \
		$(CLI_URE) \
		$(CLI_OOOTYPES)
    $(GNUCOPY) $(CLI_CPPUHELPER) $(BIN)$/$(CLI_CPPUHELPER:f)
    $(GNUCOPY) $(CLI_BASETYPES) $(BIN)$/$(CLI_BASETYPES:f)
    $(GNUCOPY) $(CLI_URETYPES) $(BIN)$/$(CLI_URETYPES:f)
    $(GNUCOPY) $(CLI_URE) $(BIN)$/$(CLI_URE:f)
    $(GNUCOPY) $(CLI_OOOTYPES) $(BIN)$/$(CLI_OOOTYPES:f)
    $(TOUCH) $@

$(BIN)$/cli_bridgetest_inprocess.exe : \
		cli_bridgetest_inprocess.cs \
		$(BIN)$/cli_cs_bridgetest.uno.dll \
		$(BIN)$/cli_cs_testobj.uno.dll \
		$(BIN)$/cli_vb_bridgetest.uno.dll \
		$(BIN)$/cli_vb_testobj.uno.dll \
		$(BIN)$/cli_cpp_bridgetest.uno.dll \
		$(MISC)$/copyassemblies.done \
		$(CLI_BASETYPES) \
		$(CLI_URETYPES) \
		$(CLI_URE) \
		$(CLI_CPPUHELPER)
	$(CSC) $(CSCFLAGS) -target:exe -out:$@ \
		-reference:$(CLI_TYPES_BRIDGETEST) \
		-reference:$(CLI_BASETYPES) \
		-reference:$(CLI_URETYPES) \
		-reference:$(CLI_URE) \
		-reference:$(CLI_CPPUHELPER) \
		-reference:$(BIN)$/cli_cs_bridgetest.uno.dll \
		-reference:$(BIN)$/cli_cs_testobj.uno.dll \
		-reference:$(BIN)$/cli_vb_bridgetest.uno.dll \
		-reference:$(BIN)$/cli_vb_testobj.uno.dll \
		-reference:$(BIN)$/cli_cpp_bridgetest.uno.dll \
		cli_bridgetest_inprocess.cs
	$(GNUCOPY) cli_bridgetest_inprocess.ini $(BIN)

.ENDIF
