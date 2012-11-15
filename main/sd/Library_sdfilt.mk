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

$(eval $(call gb_Library_Library,sdfilt))

$(eval $(call gb_Library_add_api,sdfilt,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_set_include,sdfilt,\
	$$(INCLUDE) \
	-I$(SRCDIR)/sd/inc \
	-I$(SRCDIR)/sd/inc/pch \
))

$(eval $(call gb_Library_add_linked_libs,sdfilt,\
	editeng \
	svxcore \
	sd \
	sfx \
	svl \
	svt \
	stl \
	sot \
	tl \
	vcl \
	msfilter \
	cppu \
	cppuhelper \
	ucbhelper \
	i18nisolang1 \
	sal \
	comphelper \
	utl \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,sdfilt,\
	sd/source/filter/ppt/propread \
	sd/source/filter/ppt/pptin \
	sd/source/filter/ppt/pptinanimations \
	sd/source/filter/ppt/pptatom \
	sd/source/filter/ppt/ppt97animations \
	sd/source/filter/eppt/eppt \
	sd/source/filter/eppt/escherex \
	sd/source/filter/eppt/pptexanimations \
	sd/source/filter/eppt/pptexsoundcollection \
))

# the following source file can't be compiled with optimization by some compilers (crash or endless loop):
# Solaris Sparc with Sun compiler, gcc on MacOSX and Linux PPC
# the latter is currently not supported by gbuild and needs a fix here later
ifeq ($(COM),GCC)
$(eval $(call gb_Library_add_cxxobjects,sdfilt,\
    sd/source/filter/eppt/epptso \
    , $(gb_COMPILERNOOPTFLAGS) $(gb_LinkTarget_EXCEPTIONFLAGS) \
))
else
$(eval $(call gb_Library_add_exception_objects,sdfilt,\
    sd/source/filter/eppt/epptso \
))
endif

# vim: set noet sw=4 ts=4:
