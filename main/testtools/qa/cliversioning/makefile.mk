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

PRJ = ..$/..
PRJNAME = cli_ure
TARGET := qa_test_climaker

.IF "$(OOO_JUNIT_JAR)" != ""
PACKAGE = cliversion

# here store only Files which contain a @Test
JAVATESTFILES = \
    VersionTestCase.java

# put here all other files
JAVAFILES = $(JAVATESTFILES)

JARFILES = ridl.jar unoil.jar jurt.jar juh.jar java_uno.jar OOoRunner.jar
EXTRAJARFILES = $(OOO_JUNIT_JAR)

JAVAIFLAGS =\
    -DSystemRoot=$(SystemRoot) \
    -Dcli_test_program=$(BIN)$/runtests.exe \
    -Dpath="$(office)"\OpenOffice.org\URE\bin


# Sample how to debug
# JAVAIFLAGS+=-Xdebug  -Xrunjdwp:transport=dt_socket,server=y,address=9003,suspend=y

.END

.IF "$(office)" != ""

.INCLUDE: settings.mk
.INCLUDE: target.mk
.INCLUDE: installationtest.mk

ALLTAR : javatest

.ELSE
ALLTAR :
	@echo .
	@echo ###########################   N O T E  ######################################
	@echo . 
	@echo To run the test you have to provide the path to the  office location. It must
	@echo "contain the ure (d:\myOffice\OpenOffice.org\URE)."
	@echo Also an office must be installed with full system integration.
	@echo Example:
	@echo "dmake office="d:\myOffice""
	@echo .
	@echo ###########################   N O T E  ######################################
	exit 1
.ENDIF

.END
