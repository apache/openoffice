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

PRJNAME=sdext
TARGET=pdfimport-pdfbox

# --- Settings -----------------------------------------------------

.IF "$(ENABLE_PDFIMPORT)" == "NO" || "$(SOLAR_JAVA)" == ""

@all:
	@echo "PDF Import extension disabled."

.ELSE

# ant.mk, not antsettings.mk: it pulls in settings.mk and target.mk itself and
# is what defines the ANTBUILD target.  antsettings.mk only sets variables, and
# is for the BUILD_ACTION/tg_ext.mk route that unpacks a source tarball first --
# apache-commons does that; there is nothing to unpack here.
.INCLUDE : ant.mk

# build.xml does the file placement, not dmake.  $(OUTDIR) and
# $(TARFILE_LOCATION) are absolute and begin "C:", and dmake reads that colon as
# its rule separator, so neither can appear as a target or a prerequisite -- it
# fails with "Multiply defined recipe for target C".  Ant is unaffected.
#
# $(BIN) is relative, so it survives the trip through -D as a plain value and
# resolves against the build file's basedir, which is this directory.
ANT_FLAGS+=-Dpdfi.bin.dir=$(BIN)

# --- Targets ------------------------------------------------------

ALLTAR : ANTBUILD

.ENDIF # ENABLE_PDFIMPORT
