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

$(eval $(call gb_AllLangResTarget_AllLangResTarget,sd))

$(eval $(call gb_AllLangResTarget_add_srs,sd,\
	sd/res \
))

$(eval $(call gb_SrsTarget_SrsTarget,sd/res))

$(eval $(call gb_SrsTarget_set_include,sd/res,\
	$$(INCLUDE) \
	-I$(OUTDIR)/inc \
	-I$(WORKDIR)/inc/sd \
	-I$(SRCDIR)/sd/inc \
	-I$(SRCDIR)/sd/source/ui/inc \
	-I$(SRCDIR)/sd/source/ui/slidesorter/inc \
))

$(eval $(call gb_SrsTarget_add_files,sd/res,\
	sd/source/core/glob.src \
	sd/source/filter/html/pubdlg.src \
	sd/source/ui/accessibility/accessibility.src \
	sd/source/ui/animations/CustomAnimation.src \
	sd/source/ui/animations/CustomAnimationCreateDialog.src \
	sd/source/ui/animations/CustomAnimationDialog.src \
	sd/source/ui/animations/CustomAnimationPane.src \
	sd/source/ui/animations/SlideTransitionPane.src \
	sd/source/ui/annotations/annotations.src \
	sd/source/ui/app/popup.src \
	sd/source/ui/app/res_bmp.src \
	sd/source/ui/app/strings.src \
	sd/source/ui/dlg/animobjs.src \
	sd/source/ui/dlg/brkdlg.src \
	sd/source/ui/dlg/celltempl.src \
	sd/source/ui/dlg/copydlg.src \
	sd/source/ui/dlg/custsdlg.src \
	sd/source/ui/dlg/dlgass.src \
	sd/source/ui/dlg/dlgfield.src \
	sd/source/ui/dlg/dlgolbul.src \
	sd/source/ui/dlg/dlgpage.src \
	sd/source/ui/dlg/dlgsnap.src \
	sd/source/ui/dlg/dlg_char.src \
	sd/source/ui/dlg/headerfooterdlg.src \
	sd/source/ui/dlg/inspagob.src \
	sd/source/ui/dlg/ins_paste.src \
	sd/source/ui/dlg/LayerDialog.src \
	sd/source/ui/dlg/layeroptionsdlg.src \
	sd/source/ui/dlg/masterlayoutdlg.src \
	sd/source/ui/dlg/morphdlg.src \
	sd/source/ui/dlg/navigatr.src \
	sd/source/ui/dlg/PaneDockingWindow.src \
	sd/source/ui/dlg/paragr.src \
	sd/source/ui/dlg/present.src \
	sd/source/ui/dlg/prltempl.src \
	sd/source/ui/dlg/prntopts.src \
	sd/source/ui/dlg/sdpreslt.src \
	sd/source/ui/dlg/tabtempl.src \
	sd/source/ui/dlg/tpaction.src \
	sd/source/ui/dlg/tpoption.src \
	sd/source/ui/dlg/vectdlg.src \
	sd/source/ui/notes/NotesChildWindow.src \
	sd/source/ui/slideshow/slideshow.src \
	sd/source/ui/slidesorter/view/SlsResource.src \
	sd/source/ui/table/TableDesignPane.src \
	sd/source/ui/view/DocumentRenderer.src \
))

#todo?
#	sd/util/hidother.src \
