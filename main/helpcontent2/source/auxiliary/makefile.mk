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


PRJ		= ..$/..
PRJNAME = helpcontent2
TARGET  = auxiliary

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

TREEFILES  = \
	sbasic.tree \
	simpress.tree \
	scalc.tree \
	smath.tree \
	schart.tree \
	swriter.tree \
	shared.tree

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.EXPORT : LOCALIZESDF LOCALIZATION_FOUND TRYSDF

ALLTAR : aux_dirs $(COMMONMISC)$/treefiles.done $(COMMONBIN)$/helpimg.ilst

.IF "$(WITH_LANG)"!=""
t_aux_langdirs:=$(shell @find ./ -name "*.cfg" | sed "s/\.\/\///" | sed "s/\.\///" )

aux_langdirs:=$(uniq $(t_aux_langdirs:d:d))

aux_alllangiso:=$(foreach,i,$(alllangiso) $(foreach,j,$(aux_langdirs) $(eq,$i,$j  $i $(NULL))))

WITH_LANG!:=$(aux_alllangiso)

.EXPORT : WITH_LANG

LOCTREEFILES:=$(foreach,i,$(aux_alllangiso) $(foreach,j,$(TREEFILES) $(COMMONMISC)$/$i$/$j))
.ELSE			#IF "$(WITH_LANG)"!=""
aux_langdirs:=en-US
LOCTREEFILES:=$(foreach,j,$(TREEFILES) $(COMMONMISC)$/en-US$/$j)
.ENDIF			#IF "$(WITH_LANG)"!=""

$(COMMONMISC)$/treefiles.done : $(LOCTREEFILES)
	+$(PERL) $(PRJ)$/helpers$/update_tree.pl && $(TOUCH) $@

%.created:
	@-$(MKDIRHIER) $(@:d) && $(TOUCH) $@

$(LOCTREEFILES) : $(TREEFILES) $$(@:d)$/dir.created
	@$(TOUCH) $@

.IF "$(WITH_LANG)"!=""
.IF "$(LOCALIZATION_FOUND)"=="YES"
$(LOCTREEFILES) : $(TRYSDF:d:d:d:d)$/text$/shared/localize.sdf
.ELSE			# "$(LOCALIZATION_FOUND)"=="YES"
$(COMMONMISC)/unpack.done : $(SOLARCOMMONSDFDIR)$/$(PRJNAME).zip
    @@-$(MKDIRHIER) $(COMMONMISC)$/$(PRJNAME)_$(TARGET)
	unzip -o -d $(COMMONMISC)$/$(PRJNAME) $(SOLARCOMMONSDFDIR)$/$(PRJNAME).zip 
    $(TOUCH) $@

$(LOCTREEFILES) : $(COMMONMISC)/unpack.done

.ENDIF			# "$(LOCALIZATION_FOUND)"=="YES"
.ENDIF			# "$(WITH_LANG)"!=""

aux_dirs .PHONY :
    echo aux_langdirs:=$(aux_langdirs) > $(LOCAL_COMMON_OUT)/inc$/aux_langs.mk

$(COMMONBIN)$/helpimg.ilst .PHONY:
	-$(RM) $@
	$(PERL) $(PRJ)$/helpers$/create_ilst.pl -dir=$(SOLARSRC)/default_images/res/helpimg > $@.$(INPATH)
	$(RENAME) $@.$(INPATH) $@
	

