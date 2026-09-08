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

.INCLUDE : settings.mk

.IF "$(ENABLE_PDFIMPORT)" == "NO" || "$(SOLAR_JAVA)" == ""

all:
	@echo "PDF Import extension disabled."

.ELSE

ANT_BUILDFILE=build.xml
.INCLUDE : antsettings.mk

# external_deps.lst downloads the four jars into $(TARFILE_LOCATION).  They are
# needed in two places and neither is optional: solenv/ant/externals.xml
# resolves external.<name>.path to $(OUTDIR)/bin, which is what puts them on
# ant's compile classpath, and the extension packs them beside the helper jar
# so that `java -jar` can follow the Class-Path in its manifest.
#
# Unlike apache-commons, which fetches source and builds it, these are official
# ASF release artifacts, so there is nothing to build -- only to place.
PDFBOX_JARNAMES= \
	pdfbox-3.0.8.jar \
	pdfbox-io-3.0.8.jar \
	fontbox-3.0.8.jar \
	commons-logging-1.4.0.jar

SOLVER_JARS=$(foreach,i,$(PDFBOX_JARNAMES) $(OUTDIR)$/bin$/$(i))
DELIVERED_JARS=$(foreach,i,$(PDFBOX_JARNAMES) $(BIN)$/$(i))

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

# The solver copies must exist before ant runs, or the compile classpath points
# at nothing; the $(BIN) copies are what ../makefile.mk packs into the .oxt.
ANTBUILD : $(SOLVER_JARS)

ALLTAR : $(DELIVERED_JARS) $(BIN)$/$(TARGET).jar

$(SOLVER_JARS) : $(TARFILE_LOCATION)$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

$(DELIVERED_JARS) : $(TARFILE_LOCATION)$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

# aoo-ant.xml writes the jar to $(WORKDIR)/Ant; bring it where the extension
# packaging expects to find it.
$(BIN)$/$(TARGET).jar : ANTBUILD
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $(WORKDIR)$/Ant$/$(TARGET).jar $@

.ENDIF # ENABLE_PDFIMPORT
