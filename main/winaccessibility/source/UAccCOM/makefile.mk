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

PRJ=..$/..
PRJNAME=winaccessibility
TARGET=UAccCOM

# --- Settings -----------------------------------------------------
.IF "$(GUI)" == "WNT" && "$(DISABLE_ATL)"==""
PROF_EDITION=TRUE
.ENDIF

.INCLUDE :	settings.mk

.IF "$(GUI)" == "WNT" && "$(DISABLE_ATL)"==""

VERSIONOBJ=
LIBTARGET=NO
USE_DEFFILE=YES
UWINAPILIB=

INCPRE+=$(foreach,i,$(ATL_INCLUDE) -I$(i)) \
    -I$(MISC) \

CFLAGS+=-D_UNICODE -DUNICODE

# --- Files --------------------------------------------------------
.IF "$(PRODUCT)"!=""
RC+=-DPRODUCT
.ENDIF
.IF "$(USE_STLP_DEBUG)"!=""
CDEFS+=-D_DEBUG
.ENDIF # "$(USE_STLP_DEBUG)"!=""

RCFILES=\
        $(TARGET).rc
#RCDEPN=$(MISC)$/envsettings.h

SLOFILES=\
	$(SLO)$/AccAction.obj \
	$(SLO)$/AccActionBase.obj \
	$(SLO)$/AccComponent.obj \
	$(SLO)$/AccComponentBase.obj \
	$(SLO)$/AccEditableText.obj \
	$(SLO)$/AccHyperLink.obj \
	$(SLO)$/AccHypertext.obj \
	$(SLO)$/AccImage.obj \
	$(SLO)$/AccRelation.obj \
	$(SLO)$/AccText.obj \
	$(SLO)$/AccValue.obj \
	$(SLO)$/EnumVariant.obj \
	$(SLO)$/StdAfx.obj \
	$(SLO)$/UAccCOM.obj \
	$(SLO)$/UNOXWrapper.obj \
	$(SLO)$/AccTable.obj \
	$(SLO)$/AccTextBase.obj \
	$(SLO)$/MAccessible.obj \
	$(SLO)$/CheckEnableAccessible.obj \
	
EXCEPTIONSFILES= \
	$(SLO)$/AccAction.obj \
	$(SLO)$/AccActionBase.obj \
	$(SLO)$/AccComponent.obj \
	$(SLO)$/AccComponentBase.obj \
	$(SLO)$/AccEditableText.obj \
	$(SLO)$/AccHyperLink.obj \
	$(SLO)$/AccHypertext.obj \
	$(SLO)$/AccImage.obj \
	$(SLO)$/AccRelation.obj \
	$(SLO)$/AccText.obj \
	$(SLO)$/AccValue.obj \
	$(SLO)$/EnumVariant.obj \
	$(SLO)$/StdAfx.obj \
	$(SLO)$/UAccCOM.obj \
	$(SLO)$/UNOXWrapper.obj \
	$(SLO)$/AccTable.obj \
	$(SLO)$/AccTextBase.obj \
	$(SLO)$/MAccessible.obj \

SHL1TARGET=$(TARGET)
SHL1STDLIBS=\
    oleacc.lib\
    isal.lib\
    icuuc.lib\
    icuin.lib\
    icppu.lib\
    ivcl.lib \
	kernel32.lib \
	user32.lib \
	advapi32.lib \
	ole32.lib \
	oleaut32.lib \
	delayimp.lib \
	shlwapi.lib \
	uuid.lib
	
LINKFLAGS += /delayload:sal3.dll \
             /delayload:cppu3.dll \
             /delayload:vclmi.dll	
   
.IF "$(COM)"!="GCC"
.IF "$(CCNUMVER)" > "001300000000"
.IF "$(USE_STLP_DEBUG)" != ""
    SHL1STDLIBS+= $(ATL_LIB)$/atlsd.lib
.ELSE
    SHL1STDLIBS+= $(ATL_LIB)$/atls.lib
.ENDIF
.ENDIF
.ENDIF 
    
SHL1OBJS=$(SLOFILES)
SHL1DEF=$(TARGET).def 
SHL1RES=$(RES)$/$(TARGET).res 

DEF1NAME= $(TARGET)
#DEF1EXPORTFILE=	exports.dxp

.ENDIF

# --- Targets ----------------------------------
.INCLUDE : target.mk
