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
PRJNAME=l10n
TARGET=l10n_merge

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

.IF "$(WITH_LANG)" == ""

@all:
	@echo "Nothing to do - en-US only build."
.ELSE

all_sdfs:=$(shell cd $(PRJ)/source && ls -1 */localize.sdf)

.INCLUDE .IGNORE : $(COMMONMISC)/sdf/lock.mk

.INCLUDE : target.mk

ALLTAR : $(COMMONMISC)/merge.done

$(COMMONMISC)/merge.done : $(all_sdfs)
.IF "$(L10N_LOCK)" != "YES"
	$(IFEXIST) $(COMMONMISC)/sdf $(THEN) $(RENAME) $(COMMONMISC)/sdf $(COMMONMISC)/sdf$(INPATH)_begone $(FI)
	-rm -rf $(COMMONMISC)/sdf$(INPATH)_begone
	-$(MKDIRHIER) $(COMMONMISC)/sdf
.ENDIF			# "$(L10n_LOCK)" != "YES"
	$(PERL) $(SOLARVER)/$(INPATH)/bin$(UPDMINOREXT)/fast_merge.pl -sdf_files $(mktmp $<) -merge_dir $(COMMONMISC)/sdf && $(TOUCH) $@
    $(COPY) $(PRJ)/localization_present.mk $(PRJ)/$(COMMON_OUTDIR)$(PROEXT)/inc
.ENDIF
