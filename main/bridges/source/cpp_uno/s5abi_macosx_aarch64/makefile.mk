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

PRJNAME=bridges
TARGET=$(COMNAME)_uno
LIBTARGET=no
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

# Apple Silicon (arm64 / AAPCS64) C++-UNO bridge. CPU=R, CPUNAME=AARCH64.
# Note: the "s5abi" in this directory's name is the macOS-wide COMNAME (see
# solenv/gbuild/platform/macosx.mk); the actual arm64 calling convention is
# AAPCS64, so "s5abi" here is a historical label, not a literal ABI claim.
.IF "$(OS)$(CPU)" == "MACOSXR"

.IF "$(cppu_no_leak)" == ""
CFLAGS += -DLEAK_STATIC_DATA
.ENDIF

# In case someone enabled the non-standard -fomit-frame-pointer which does not
# work with the .cxx sources in this directory:
CFLAGSCXX += -fno-omit-frame-pointer -fnon-call-exceptions

SLOFILES= \
	$(SLO)$/abi.obj			\
	$(SLO)$/except.obj		\
	$(SLO)$/cpp2uno.obj		\
	$(SLO)$/uno2cpp.obj		\
	$(SLO)$/call.obj

SHL1TARGET= $(TARGET)

SHL1DEF=$(MISC)$/$(SHL1TARGET).def
SHL1IMPLIB=i$(TARGET)
SHL1VERSIONMAP=..$/..$/bridge_exports.map
SHL1RPATH=URELIB

SHL1OBJS = $(SLOFILES)
SHL1LIBS = $(SLB)$/cpp_uno_shared.lib

SHL1STDLIBS= \
	$(CPPULIB)			\
	$(SALLIB)

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

# Assemble the AArch64 call trampoline (call.s) into call.obj, mirroring the
# pattern rule used by the gcc3_linux_arm bridge for its armhelper.S.
$(SLO)$/%.obj: %.s
	$(CXX) -c -o $(SLO)$/$(@:b).o $< -fPIC ; touch $@
