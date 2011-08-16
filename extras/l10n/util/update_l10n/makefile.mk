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

PRJ=../..
PRJNAME=l10n
TARGET=l10n_update

#########################################################################
#
# Use me if you make minor changes in the l10n/*/localize.sdf files to merge the change into the output tree without removeing / remerging all and everything
#                    ^^^^^ ^^^^^^^
########################################################################

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

.IF "$(WITH_LANG)" == ""

@all:
	@echo "Nothing to do - en-US only build."
.ELSE

all_sdfs:=$(shell ls -1 $(PRJ)$/source$/*$/localize.sdf)

.INCLUDE .IGNORE : $(COMMONMISC)$/sdf$/lock.mk

.INCLUDE : target.mk

ALLTAR : $(COMMONMISC)$/merge.done

$(COMMONMISC)$/merge.done : $(all_sdfs)
	$(foreach,j,$? $(PERL) $(SOLARVER)$/$(INPATH)$/bin$(UPDMINOREXT)$/localize_old.pl -m -v -l all -s $(COMMONMISC)$/sdf -f $j && $(TOUCH) $@ ; )

#$(COMMONMISC)$/merge.done : $(COMMONMISC)/{$(all_sdfs:d:d:f)}_merge.done

#$(COMMONMISC)$/%_merge.done : $(PRJ)/source/%/localize.sdf
#	$(PERL) $(SOLARVER)$/$(INPATH)$/bin$(UPDMINOREXT)$/localize_old.pl -m -v -l all -s $(COMMONMISC)$/sdf -f $? && $(TOUCH) $@

.ENDIF
