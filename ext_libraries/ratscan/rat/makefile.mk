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

PRJ=..

PRJNAME=ratscan
TARGET=so_ratscan

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

.IF "$(RAT_JAR_HOME)"==""

all:
	@echo "rat scan is disabled."

.ELSE

TARFILE_NAME=apache-rat-0.10-bin
TARFILE_MD5=0f926196d42577addec7e1aa5b510605
TARFILE_ROOTDIR=apache-rat-0.10
#TARFILE_NAME=apache-rat-incubating-0.8-bin
#TARFILE_MD5=e4244e69eb3644e71c254aea5952a918
#TARFILE_ROOTDIR=apache-rat-0.8

# Trigger unpacking of archive.
PATCH_FILES=

CONFIGURE_DIR=
CONFIGURE_ACTION=
CONFIGURE_FLAGS=

BUILD_DIR=
BUILD_ACTION=echo no building necessary
BUILD_FLAGS=

OUT2CLASS=apache-rat-0.10.jar
#OUT2CLASS=apache-rat-0.8.jar

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
