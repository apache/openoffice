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



PRJ=..$/..
PRJNAME=embedserv
TARGET=inprocserv

use_shl_versions=

# --- Settings ----------------------------------
.INCLUDE : settings.mk

.IF "$(GUI)" == "WNT"

LIBTARGET=NO
USE_DEFFILE=YES
# The static CRT, deliberately, and replacing LIBCMT rather than adding to it:
# this DLL is loaded into a foreign process and cannot rely on that process
# having ours.  $(LIBCRTEXTRA_STATIC) is the rest of the static CRT on the UCRT
# generation, where it comes in three pieces rather than one; it is undefined,
# and so expands to nothing, on VC9.
LIBCMT=libcmt.lib $(LIBCRTEXTRA_STATIC)
UWINAPILIB=

# --- Files -------------------------------------

SLOFILES=\
	$(SLO)$/dllentry.obj \
	$(SLO)$/advisesink.obj \
	$(SLO)$/inprocembobj.obj

SHL1TARGET=$(TARGET)
.IF "$(COM)"=="GCC"
SHL1STDLIBS += -lstdc++
.IF "$(MINGW_GCCLIB_EH)"=="YES"
SHL1STDLIBS += -lgcc_eh
.ENDIF
SHL1STDLIBS += -lgcc -lmingw32 -lmoldname -lmsvcrt
.ELSE
SHL1STDLIBS=
.ENDIF

SHL1STDLIBS+=\
    $(UUIDLIB)\
    $(OLE32LIB)\
    $(GDI32LIB)\
    $(ADVAPI32LIB)

SHL1OBJS=$(SLOFILES)

SHL1DEF=$(MISC)$/$(TARGET).def

DEF1NAME= $(TARGET)
DEF1EXPORTFILE=	exports.dxp

.ENDIF

# --- Targets ----------------------------------

.INCLUDE : target.mk
