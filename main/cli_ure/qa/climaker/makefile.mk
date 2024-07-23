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


.IF "$(OOO_SUBSEQUENT_TESTS)" == ""
nothing .PHONY:
.ELSE

PRJ := ..$/..
PRJNAME := cli_ure
TARGET := test_climaker

.IF "$(OOO_JUNIT_JAR)" != ""
PACKAGE = climaker

# here store only Files which contain a @Test
JAVATESTFILES = \
    ClimakerTestCase.java

# put here all other files
JAVAFILES = $(JAVATESTFILES)

JARFILES = ridl.jar unoil.jar jurt.jar juh.jar java_uno.jar
EXTRAJARFILES = $(OOO_JUNIT_JAR)

JAVAIFLAGS =\
    -Dcli_ure_test=$(EXETARGET)

# Sample how to debug
# JAVAIFLAGS+=-Xdebug  -Xrunjdwp:transport=dt_socket,server=y,address=9003,suspend=y

.END

.INCLUDE: settings.mk
.INCLUDE: target.mk
.INCLUDE: installationtest.mk

ALLTAR : javatest

#------ Specific to this test: -----

#we use the climaker which is build by this project
CLIMAKER*=$(WRAPCMD) $(BIN)$/climaker

CSCFLAGS = -incr
.IF "$(debug)" != ""
CSCFLAGS += -checked+ -define:DEBUG -define:TRACE -debug+
.ELSE
CSCFLAGS += -optimize+
.ENDIF

OUTDIR=$(BIN)$/qa$/climaker
EXETARGET=$(OUTDIR)$/test_climaker.exe

javatest: $(EXETARGET)

CSFILES = climaker.cs testobjects.cs

$(EXETARGET): $(CSFILES) $(OUTDIR)$/cli_test_types.dll
	$(GNUCOPY) -p $(BIN)$/cli_cppuhelper.dll $(OUTDIR)$/cli_cppuhelper.dll
	$(GNUCOPY) -p $(BIN)$/cli_uretypes.dll $(OUTDIR)$/cli_uretypes.dll
	$(GNUCOPY) -p $(BIN)$/cli_basetypes.dll $(OUTDIR)$/cli_basetypes.dll
	$(GNUCOPY) -p $(BIN)$/cli_ure.dll $(OUTDIR)$/cli_ure.dll
	$(GNUCOPY) -p $(BIN)$/climaker.exe $(OUTDIR)
	$(CSC) $(CSCFLAGS) -target:exe -out:$(EXETARGET) \
		-reference:$(BIN)$/cli_ure.dll \
 		-reference:$(BIN)$/cli_uretypes.dll \
 		-reference:$(BIN)$/cli_basetypes.dll \
		-reference:$(OUTDIR)$/cli_test_types.dll \
		$(CSFILES)

CLIMAKERFLAGS =
.IF "$(debug)" != ""
CLIMAKERFLAGS += --verbose
.ENDIF

$(OUTDIR)$/types.urd: types.idl
    - $(MKDIR) $(OUTDIR)
    $(IDLC) -O$(OUTDIR) -I$(SOLARIDLDIR) -cid -we $<

$(OUTDIR)$/types.rdb: $(OUTDIR)$/types.urd
    - rm $@
    $(REGMERGE) $@ /UCR $<

$(OUTDIR)$/cli_test_types.dll: $(OUTDIR)$/types.rdb $(BIN)$/climaker.exe $(BIN)$/cli_uretypes.dll
    $(CLIMAKER) $(CLIMAKERFLAGS) --out $@  \
		-r $(BIN)$/cli_uretypes.dll \
		-X $(SOLARBINDIR)$/types.rdb \
		$(OUTDIR)$/types.rdb


#-----------------------------------


.END
