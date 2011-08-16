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
PRJ		= ..$/..$/..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
# edit to match the current package
PACKAGE = text/simpress/guide
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_simpress_guide
# edit to match the current module
MODULE  = simpress

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   3d_create.xhp \
   animated_gif_create.xhp \
   animated_gif_save.xhp \
   animated_objects.xhp \
   animated_slidechange.xhp \
   arrange_slides.xhp \
   background.xhp \
   change_scale.xhp \
   footer.xhp \
   gluepoints.xhp \
   html_export.xhp \
   html_import.xhp \
   individual.xhp \
   keyboard.xhp \
   layer_move.xhp \
   layer_new.xhp \
   layer_tipps.xhp \
   layers.xhp \
   line_arrow_styles.xhp \
   line_draw.xhp \
   line_edit.xhp \
   main.xhp \
   masterpage.xhp \
   move_object.xhp \
   orgchart.xhp \
   page_copy.xhp \
   palette_files.xhp \
   print_tofit.xhp \
   printing.xhp \
   rehearse_timings.xhp \
   select_object.xhp \
   show.xhp \
   table_insert.xhp \
   text2curve.xhp \
   vectorize.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
