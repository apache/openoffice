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
PACKAGE=org$/apache$/openoffice$/pdfimport

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

.IF "$(ENABLE_PDFIMPORT)" == "NO" || "$(SOLAR_JAVA)" == ""

all:
	@echo "PDF Import extension disabled."

.ELSE

# The four jars are downloaded by external_deps.lst into $(TARFILE_LOCATION).
# They are needed twice over: on the compile classpath here, and in the
# extension at run time, since the helper is started as `java -jar` and reaches
# them through the Class-Path in its own manifest.
PDFBOX_JARNAMES= \
	pdfbox-3.0.8.jar \
	pdfbox-io-3.0.8.jar \
	fontbox-3.0.8.jar \
	commons-logging-1.4.0.jar

PDFBOX_JARS=$(foreach,i,$(PDFBOX_JARNAMES) $(TARFILE_LOCATION)$/$(i))
DELIVERED_JARS=$(foreach,i,$(PDFBOX_JARNAMES) $(BIN)$/$(i))

# --- Files --------------------------------------------------------

JAVAFILES= \
	src$/$(PACKAGE)$/Main.java \
	src$/$(PACKAGE)$/PdfiEngine.java \
	src$/$(PACKAGE)$/PdfiWriter.java

JAVACLASSFILES=$(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

# javac needs PDFBox on the classpath; the office's own jars are irrelevant
# here, as the helper is a plain process and links no UNO.
XCLASSPATH!:=$(XCLASSPATH)$(PATH_SEPERATOR)$(PDFBOX_JARS:t"$(PATH_SEPERATOR)")

JARCLASSDIRS=org
JARTARGET=$(TARGET).jar
JARCOMPRESS=TRUE
CUSTOMMANIFESTFILE=manifest.mf

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

# Stage the downloaded jars beside the built one, because the extension's
# packaging rule in ../makefile.mk copies every CONVERTER_FILE out of $(BIN).
ALLTAR : $(DELIVERED_JARS)

$(DELIVERED_JARS) : $(TARFILE_LOCATION)$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

.ENDIF # ENABLE_PDFIMPORT
