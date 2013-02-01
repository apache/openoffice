#/*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright IBM Corporation 2010.
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
#************************************************************************/

.IF "$(GUI)"=="WNT"
PRJ=..$/..
PRJNAME=winaccessibility
TARGET=winaccessibility
LIBTARGET=NO
USE_DEFFILE=TRUE
ENABLE_EXCEPTIONS=TRUE
VERSIONOBJ=
AUTOSEG=true
NO_BSYMBOLIC=	TRUE

# output directory (one dir for each project)
UNOUCROUT=$(OUT)$/inc
# adding to inludepath
INCPRE+=$(UNOUCROUT)
INCPRE+=$(MISC)
# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------
SLOFILES=   $(SLO)$/AccObject.obj		\
            $(SLO)$/ResIDGenerator.obj		\
            $(SLO)$/AccObjectWinManager.obj			\
            $(SLO)$/AccObjectManagerAgent.obj			\
            $(SLO)$/AccEventListener.obj	\
            $(SLO)$/AccComponentEventListener.obj	\
            $(SLO)$/AccContainerEventListener.obj	\
            $(SLO)$/AccDialogEventListener.obj	\
            $(SLO)$/AccFrameEventListener.obj	\
            $(SLO)$/AccWindowEventListener.obj	\
            $(SLO)$/AccMenuEventListener.obj	\
            $(SLO)$/AccTextComponentEventListener.obj	\
            $(SLO)$/AccObjectContainerEventListener.obj	\
            $(SLO)$/AccParagraphEventListener.obj	\
            $(SLO)$/AccDescendantManagerEventListener.obj	\
            $(SLO)$/AccListEventListener.obj	\
            $(SLO)$/AccTableEventListener.obj	\
            $(SLO)$/AccTreeEventListener.obj	\
            $(SLO)$/AccTopWindowListener.obj	\
			$(SLO)$/msaaservice_impl.obj 	\
			$(SLO)$/checkmt.obj	\
			$(SLO)$/AccResource.obj

#SRCFILES=winaccessibility.src
#RES1FILELIST=$(SRS)$/winaccessibility.srs
#RESLIB1NAME=$(TARGET)
#RESLIB1SRSFILES=$(RES1FILELIST)

SHL1TARGET=winaccessibility
SHL1IMPLIB=i$(SHL1TARGET)
SHL1STDLIBS=\
				$(VCLLIB)		\
				$(CPPULIB)		\
				$(SALLIB)		\
				$(CPPUHELPERLIB) \
				$(TOOLSLIB) \
				$(TKLIB)		\
			    uwinapi.lib      \
				gdi32.lib        \
				winspool.lib     \
				ole32.lib        \
				shell32.lib      \
				advapi32.lib     \
				imm32.lib		\
				uuid.lib			\
				oleacc.lib			\
				$(VOSLIB)
				
SHL1OBJS=$(SLOFILES) 
SHL1VERSIONOBJ=
SHL1DESCRIPTIONOBJ=

DEF1NAME=$(SHL1TARGET)
DEF1EXPORTFILE=exports.dxp



# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

ALLTAR : \
    $(MISC)/winaccessibility.component

.IF "$(OS)" == "WNT"
my_platform = .windows
.END

$(MISC)/winaccessibility.component .ERRREMOVE : \
        $(SOLARENV)/bin/createcomponent.xslt winaccessibility.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt winaccessibility.component


.ENDIF			# "$(GUI)"=="WNT"
