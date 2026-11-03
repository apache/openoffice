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

PRJNAME=ooo_rhino
TARGET=ooo_rhino

.IF "$(SOLAR_JAVA)"==""
all:
        @echo java disabled

.ELIF "$(ENABLE_JAVASCRIPT)"!="YES"
all:
        @echo javascript support disabled
.ELSE

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :  antsettings.mk

# --- Files --------------------------------------------------------

#.IF "$(DISABLE_RHINO)" == ""

TARFILE_NAME=rhino1_7_7_2
TARFILE_MD5=afcf570784ebd919c1d484a6d0d3fcd6
TARFILE_ROOTDIR=rhino1.7.7.2

ADDITIONAL_FILES=

PATCH_FILES=rhino1_7_7_2.patch rhino-1.7.7.2-definingclassloader.patch rhino1_7_7_2-java-version.patch rhino-1.7.7.2-main.patch rhino-1.7.7.2-xxe-fix.patch rhino-1.7.7.2-tofixed-fix.patch

.IF "$(JAVACISGCJ)"=="yes"
JAVA_HOME=
.EXPORT : JAVA_HOME
BUILD_ACTION=$(ANT) -Dbuild.label="build-$(RSCREVISION)" -Dbuild.compiler=gcj -Dno-xmlbeans=true jar
.ELSE
BUILD_ACTION=$(ANT) -Dbuild.label="build-$(RSCREVISION)" -Dno-xmlbeans=true jar
.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
