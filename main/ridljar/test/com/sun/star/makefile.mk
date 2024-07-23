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

PRJ = ..$/..$/..$/..
PRJNAME = ridljar
TARGET  = ridljar_test

.IF "$(OOO_JUNIT_JAR)" != ""
PACKAGE = com$/sun$/star

# here store only Files which contain a @Test
JAVATESTFILES = \
    uno/Type_Test.java \
    uno/UnoRuntime_Test.java \
    uno/Any_Test.java \
    lib/util/WeakMap_Test.java \
    lib/uno/typedesc/TypeDescription_Test.java

# put here all other files
JAVAFILES = $(JAVATESTFILES)

JARFILES        = ridl.jar OOoRunner.jar
EXTRAJARFILES = $(OOO_JUNIT_JAR)

# Sample how to debug
# JAVAIFLAGS+=-Xdebug  -Xrunjdwp:transport=dt_socket,server=y,address=9003,suspend=y

.END

.INCLUDE: settings.mk
.INCLUDE: target.mk
.INCLUDE: installationtest.mk

ALLTAR : javatest

.END

