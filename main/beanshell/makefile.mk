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



PRJ=.

PRJNAME=ooo_beanshell
TARGET=ooo_beanshell

.IF "$(SOLAR_JAVA)"==""

all:
        @echo java disabled

.ELIF "$(DISABLE_BEANSHELL)"=="YES"

all:
        @echo beanshell support disabled

.ELSE

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

# BeanShell 3 is released as a ready-to-use OSGi bundle jar, so there is
# nothing to unpack or build; the downloaded jar is the bsh.jar we ship.
BSH_JAR_NAME=bsh-3.0.0RC3.jar
BSH_JAR_MD5=3623feb40cb7e4fb26e02d8921ef86fa

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

ALLTAR : $(BIN)$/bsh.jar

$(BIN)$/bsh.jar : $(TARFILE_LOCATION)$/$(BSH_JAR_MD5)-$(BSH_JAR_NAME)
	$(COMMAND_ECHO)$(COPY) $< $@

.ENDIF
