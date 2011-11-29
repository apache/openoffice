#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ=..

PRJNAME=dictionaries
TARGET=dict-it

# --- Settings -----------------------------------------------------

.INCLUDE: settings.mk
# it might be useful to have an extension wide include to set things
# like the EXTNAME variable (used for configuration processing)
# .INCLUDE :  $(PRJ)$/source$/<extension name>$/<extension_name>.pmk

# --- Files --------------------------------------------------------

# name for uniq directory
EXTENSIONNAME:=dict-it
EXTENSION_ZIPNAME:=dict-it

# some other targets to be done

# --- Extension packaging ------------------------------------------

# just copy:
COMPONENT_FILES= \
    $(EXTENSIONDIR)$/legacy/it_IT_AUTHORS \
    $(EXTENSIONDIR)$/legacy/it_IT_ChangeLog \
    $(EXTENSIONDIR)$/legacy/it_IT_COPYING \
    $(EXTENSIONDIR)$/legacy/it_IT_license.txt \
    $(EXTENSIONDIR)$/legacy/it_IT_notes.txt \
    $(EXTENSIONDIR)$/legacy/it_IT_README.1st \
    $(EXTENSIONDIR)$/legacy/it_IT_README.txt \
    $(EXTENSIONDIR)$/legacy/it_IT_statistiche.ods \
    $(EXTENSIONDIR)$/legacy/it_IT_todo.txt \
    $(EXTENSIONDIR)$/legacy/thes_gpl.txt \
    $(EXTENSIONDIR)$/legacy/th_it_IT_AUTHORS \
    $(EXTENSIONDIR)$/legacy/th_it_IT_ChangeLog \
    $(EXTENSIONDIR)$/legacy/th_it_IT_COPYING \
    $(EXTENSIONDIR)$/legacy/th_it_IT_copyright_licenza.txt \
    $(EXTENSIONDIR)$/legacy/th_it_IT_INSTALL \
    $(EXTENSIONDIR)$/legacy/th_it_IT_lettera_in_inglese.txt \
    $(EXTENSIONDIR)$/legacy/th_it_IT_Lingua.png \
    $(EXTENSIONDIR)$/legacy/th_it_IT_Linguistica-Modifica.png \
    $(EXTENSIONDIR)$/legacy/th_it_IT_README \
    $(EXTENSIONDIR)$/legacy/th_it_IT_sinonimi.jpg \
    $(EXTENSIONDIR)$/CHANGELOG.txt \
    $(EXTENSIONDIR)$/desc_en.txt \
    $(EXTENSIONDIR)$/desc_it.txt \
    $(EXTENSIONDIR)$/hyph_it_IT.dic \
    $(EXTENSIONDIR)$/ico.png \
    $(EXTENSIONDIR)$/it_IT.aff \
    $(EXTENSIONDIR)$/it_IT.dic \
    $(EXTENSIONDIR)$/README_hyph_it_IT.txt \
    $(EXTENSIONDIR)$/README_th_it_IT.txt \
    $(EXTENSIONDIR)$/th_it_IT_v2.dat \
    $(EXTENSIONDIR)$/th_it_IT_v2.idx 

COMPONENT_CONFIGDEST=.
COMPONENT_XCU= \
    $(EXTENSIONDIR)$/dictionaries.xcu

# disable fetching default OOo license text
CUSTOM_LICENSE=README.txt
# override default license destination
PACKLICS= $(EXTENSIONDIR)$/$(CUSTOM_LICENSE)

# add own targets to packing dependencies (need to be done before
# packing the xtension
# EXTENSION_PACKDEPS=makefile.mk $(CUSTOM_LICENSE)
EXTENSION_PACKDEPS=$(COMPONENT_UNZIP_FILES)     $(EXTENSIONDIR)$/th_it_IT_v2.idx

# global settings for extension packing
.INCLUDE : extension_pre.mk
.INCLUDE : target.mk
# global targets for extension packing
.INCLUDE : extension_post.mk

