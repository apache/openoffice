#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
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

$(eval $(call gb_Library_Library,sdui))

$(eval $(call gb_Library_add_api,sdui,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_set_include,sdui,\
	-I$(SRCDIR)/sd/source/ui/inc \
	-I$(SRCDIR)/sd/source/ui/inc/framework \
	-I$(SRCDIR)/sd/source/ui/inc/taskpane \
	-I$(SRCDIR)/sd/source/ui/inc/view \
	-I$(SRCDIR)/sd/source/ui/inc/tools \
	-I$(SRCDIR)/sd/source/ui/slidesorter/inc \
	-I$(SRCDIR)/sd/inc \
	-I$(SRCDIR)/sd/inc/helper \
	-I$(SRCDIR)/sd/inc/undo \
	-I$(SRCDIR)/sd/inc/pch \
	-I$(WORKDIR)/SdiTarget/sd/sdi \
	-I$(WORKDIR)/Misc/sd/ \
	$$(INCLUDE) \
	-I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_add_linked_libs,sdui,\
	sd \
	editeng \
	svxcore \
	svx \
	sfx \
	basegfx \
	sot \
	stl \
	svl \
	svt \
	utl \
	vcl \
	tl \
	i18nisolang1 \
	comphelper \
	cppu \
	cppuhelper \
	sal \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,sdui,\
sd/source/ui/dlg/masterlayoutdlg \
sd/source/ui/dlg/headerfooterdlg \
sd/source/ui/dlg/sduiexp \
sd/source/ui/dlg/sddlgfact \
sd/source/ui/dlg/brkdlg \
sd/source/ui/dlg/copydlg \
sd/source/ui/dlg/custsdlg \
sd/source/ui/dlg/dlgchar \
sd/source/ui/dlg/dlgass \
sd/source/ui/dlg/dlgassim \
sd/source/ui/dlg/dlgfield \
sd/source/ui/dlg/dlgpage \
sd/source/ui/dlg/dlgsnap \
sd/source/ui/dlg/ins_paste \
sd/source/ui/dlg/inspagob \
sd/source/ui/dlg/morphdlg \
sd/source/ui/dlg/dlgolbul \
sd/source/ui/dlg/paragr \
sd/source/ui/dlg/present \
sd/source/ui/dlg/prltempl \
sd/source/ui/dlg/sdpreslt \
sd/source/ui/dlg/tabtempl \
sd/source/ui/dlg/tpaction \
sd/source/ui/dlg/tpoption \
sd/source/ui/dlg/vectdlg \
sd/source/ui/dlg/prntopts \
sd/source/ui/dlg/layeroptionsdlg \
sd/source/ui/func/bulmaper \
sd/source/filter/html/htmlattr \
sd/source/filter/html/buttonset \
sd/source/filter/html/pubdlg \
))

# vim: set noet sw=4 ts=4:
