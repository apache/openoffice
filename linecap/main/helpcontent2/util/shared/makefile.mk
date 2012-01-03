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


# edit to match directory level 
PRJ		= ..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = util_shared

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

#common_build_zip:=
zip1generatedlangs=TRUE
zip1langdirs=$(aux_alllangiso)
ZIP1TARGET=xhp_shared
ZIP1FLAGS= -u -r
ZIP1DIR=$(COMMONMISC)$/$(LANGDIR)
ZIP1LIST=$(LANGDIR)$/text$/shared$/* -x "*.xhp????*" -x "*.dphh*" -x "*.hzip" -x "*.created"

LINKNAME=shared
LINKADDEDFILES= \
   -add shared.tree $(COMMONMISC)$/LANGUAGE$/shared.tree \
   -add shared.jar  $(COMMONBIN)$/xhp_shared_LANGUAGE.zip \
   -add default.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/default.css \
   -add highcontrast1.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast1.css \
   -add highcontrast2.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast2.css \
   -add highcontrastwhite.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastwhite.css \
   -add highcontrastblack.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastblack.css \
   -add err.html  $(COMMONMISC)$/LANGUAGE$/text$/shared$/05$/err_html.xhp


LINKADDEDDEPS= \
   $(COMMONMISC)$/LANGUAGE$/shared.tree \
   $(COMMONBIN)$/xhp_shared_LANGUAGE.zip \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/default.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast1.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast2.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastwhite.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastblack.css \
   $(COMMONMISC)$/LANGUAGE$/text$/shared$/05$/err_html.xhp


LINKLINKFILES= \


# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
