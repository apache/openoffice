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
TARGET=l10n_zip

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

.IF "$(WITH_LANG)" == ""

@all:
	@echo "Nothing to do - en-US only build."
.ELSE

all_modules:=$(shell cd $(COMMONMISC)$/sdf && ls -1 )

.INCLUDE : target.mk

ALLTAR : $(foreach,i,$(all_modules) $(COMMONBIN)$/$i.zip)

$(COMMONBIN)$/%.zip : $(COMMONMISC)$/merge.done
	$(CDD) $(COMMONMISC)$/sdf$/$(@:b) && zip -r ..$/..$/..$/bin$/$(@:b)_$(INPATH).zip * $(CHECKZIPRESULT)
	$(RM) $@
	$(RENAME) $(@:d)$(@:b)_$(INPATH).zip $@

.ENDIF

