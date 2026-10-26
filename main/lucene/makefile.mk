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

PRJNAME=so_lucene
TARGET=so_lucene


# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :	antsettings.mk

.IF "$(SOLAR_JAVA)" != ""
.IF "$(SYSTEM_LUCENE)" == "YES"

all:
	@echo "An already available installation of Apache Lucene should exist on your system."
	@echo "Therefore the version provided here does not need to be built in addition."

.ENDIF

# --- Files --------------------------------------------------------

LUCENE_MAJOR=8
LUCENE_MINOR=11
LUCENE_MICRO=4

LUCENE_NAME=lucene-$(LUCENE_MAJOR).$(LUCENE_MINOR).$(LUCENE_MICRO)
LUCENE_CORE_JAR=lucene-core-$(LUCENE_MAJOR).$(LUCENE_MINOR).$(LUCENE_MICRO).jar
LUCENE_ANALYZERS_JAR=lucene-analyzers-common-$(LUCENE_MAJOR).$(LUCENE_MINOR).$(LUCENE_MICRO).jar

TARFILE_NAME=$(LUCENE_NAME)-src
TARFILE_MD5=9275dbc22d542d7ca54783867d8628ef
TARFILE_ROOTDIR=$(LUCENE_NAME)

# Upstream's Ant build needs Ivy and network access; this adds a minimal one.
ADDITIONAL_FILES=aoo-build.xml
PATCH_FILES=lucene-aoo-build.patch

BUILD_DIR=.
BUILD_ACTION= ${ANT} -buildfile aoo-build.xml

OUT2BIN=.$/aoo-out$/$(LUCENE_CORE_JAR) .$/aoo-out$/$(LUCENE_ANALYZERS_JAR)

.ELSE			# $(SOLAR_JAVA)!= ""
nojava:
	@echo "Not building $(PRJNAME) because Java is disabled"
.ENDIF			# $(SOLAR_JAVA)!= ""

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk

.IF "$(SOLAR_JAVA)" != ""
.INCLUDE : tg_ext.mk
.ENDIF
