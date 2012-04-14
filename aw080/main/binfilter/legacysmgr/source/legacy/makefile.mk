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



EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ = ..$/..$/..
BFPRJ = ..$/..
PRJNAME = binfilter
TARGET = legacy_binfilters
USE_DEFFILE = TRUE
NO_BSYMBOLIC = TRUE
ENABLE_EXCEPTIONS = TRUE
# COMP1TYPELIST = $(TARGET)

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
INC+= -I$(PRJ)$/inc$/legacysmgr
# ------------------------------------------------------------------

SLOFILES = \
	$(SLO)$/legacy_binfilters_smgr.obj

SHL1TARGET = $(TARGET)$(DLLPOSTFIX)

SHL1STDLIBS = \
	$(CPPUHELPERLIB)	\
	$(CPPULIB)		\
	$(SALLIB)

.IF "$(COMNAME)"=="msci"
SHL1VERSIONMAP=msci.map
.ELIF "$(COMNAME)"=="sunpro5"
SHL1VERSIONMAP=sols.map
.ELIF "$(GUI)$(COM)"=="WNTGCC"
SHL1VERSIONMAP=mingw_intel.map
.ELIF "$(COMNAME)"=="gcc3"
SHL1VERSIONMAP=gcc3.map
.ENDIF

SHL1DEPN =
SHL1IMPLIB = i$(TARGET)
SHL1LIBS = $(SLB)$/$(TARGET).lib
SHL1DEF = $(MISC)$/$(SHL1TARGET).def
DEF1NAME = $(SHL1TARGET)

.IF "$(debug)" != ""

# msvc++: no inlining for debugging
.IF "$(COM)" == "MSC"
CFLAGS += -Ob0
.ENDIF

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

