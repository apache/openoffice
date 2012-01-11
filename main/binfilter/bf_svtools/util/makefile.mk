#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ=..$/..
BFPRJ=..

PRJNAME=binfilter
RESTARGET=bf_svt
TARGET=bf_svtool

NO_HIDS=TRUE
LIBTARGET=NO

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- general section ----------------------------------------------------

LIB1TARGET= $(SLB)$/bf_svt.lib
LIB1FILES=	\
		$(SLB)$/misc.lib		\
		$(SLB)$/items.lib		\
		$(SLB)$/unoiface.lib	\
		$(SLB)$/filter.lib		\
		$(SLB)$/igif.lib		\
		$(SLB)$/jpeg.lib		\
		$(SLB)$/ixpm.lib		\
		$(SLB)$/ixbm.lib		\
		$(SLB)$/numbers.lib 	\
		$(SLB)$/numbers.uno.lib 	\
		$(SLB)$/wmf.lib 		\
		$(SLB)$/undo.lib		\
        $(SLB)$/syslocale.lib   \
		$(SLB)$/svdde.lib \
		$(SLB)$/config.lib	\
		$(SLB)$/filerec.lib \
		$(SLB)$/items1.lib	\
		$(SLB)$/misc1.lib	\
		$(SLB)$/notify.lib	\
		$(SLB)$/svarray.lib

RESLIB1NAME=	$(RESTARGET)
RESLIB1IMAGES=$(PRJ)$/res
RESLIB1SRSFILES= \
		$(SRS)$/ehdl.srs        \

SHL1TARGET= bf_svt$(DLLPOSTFIX)
SHL1IMPLIB= bf_svt
SHL1DEF = $(MISC)$/$(SHL1TARGET).def
SHL1LIBS= $(SLB)$/bf_svt.lib

.IF "$(OS)"!="MACOSX"
# static libraries
SHL1STDLIBS+= $(JPEG3RDLIB)
.ENDIF

# dynamic libraries
SHL1STDLIBS+= \
		$(UNOTOOLSLIB)		\
		$(VCLLIB)			\
		$(SOTLIB)			\
		$(TOOLSLIB)			\
		$(I18NISOLANGLIB)   \
		$(UCBHELPERLIB)		\
		$(COMPHELPERLIB)	\
		$(CPPUHELPERLIB)	\
		$(CPPULIB)			\
		$(VOSLIB)			\
		$(SALLIB)
		
.IF "$(OS)"=="MACOSX"
# static libraries go at end
SHL1STDLIBS+= $(JPEG3RDLIB)
.ENDIF

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= \
		$(UWINAPILIB) \
		$(ADVAPI32LIB)	\
		$(GDI32LIB) \
		$(OLE32LIB) 	\
		$(UUIDLIB) 	\
		$(ADVAPI32LIB)	\
		$(OLEAUT32LIB)
.ENDIF # WNT


DEF1NAME = $(SHL1TARGET)
DEF1DEPN = $(MISC)$/$(SHL1TARGET).flt \
		   $(LIB1TARGET)

DEF1DES		=Bf_Svtools
DEFLIB1NAME	=bf_svt

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

$(MISC)$/$(SHL1TARGET).flt: svt.flt
	@echo ------------------------------
	@echo Making: $@
	$(TYPE) svt.flt >$@

ALLTAR : $(MISC)/bf_svt.component

$(MISC)/bf_svt.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        bf_svt.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_svt.component
