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
SUBPRJ=$(PRJ)$/bf_so3

PRJPCH=

PRJNAME=binfilter
TARGET=bf_so

USE_LDUMP2=TRUE
#LDUMP2=LDUMP3

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(SUBPRJ)$/util$/makefile.pmk

RSCFLAGS+=-p

# --- Files --------------------------------------------------------


LIB1TARGET= $(SLB)$/$(TARGET).lib
LIB1FILES=\
		$(SLB)$/so3_solink.lib		\
        $(SLB)$/so3_dialog.lib      \
        $(SLB)$/so3_data.lib        \
        $(SLB)$/so3_inplace.lib     \
        $(SLB)$/so3_misc.lib        \
        $(SLB)$/so3_persist.lib \
        $(SLB)$/so3_copied.lib

.IF "$(GUI)"=="WNT"
LIB1FILES+=$(SLB)$/so3_ole.lib
.ENDIF

.IF "$(UPDATER)"==""
RESLIB1NAME=so
RESLIB1SRSFILES= $(SRS)$/so3_src.srs
.ENDIF

SHL1TARGET= $(TARGET)$(DLLPOSTFIX)
SHL1IMPLIB= $(TARGET)
SHL1STDLIBS=\
		$(VCLLIB) \
		$(SOTLIB) \
		$(UNOTOOLSLIB) \
		$(TOOLSLIB)	\
		$(BFSVTOOLLIB) \
		$(VOSLIB) \
		$(COMPHELPERLIB) \
		$(CPPUHELPERLIB) \
		$(CPPULIB) \
        $(SALLIB) \
        $(UCBHELPERLIB)

.IF "$(linkinc)" == ""
SHL1LIBS=	$(SLB)$/$(TARGET).lib
.ENDIF

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= \
			$(OLE32LIB) $(UUIDLIB) $(ADVAPI32LIB) $(GDI32LIB)
.ENDIF

.IF "$(GUI)"=="WIN"
SHL1STDLIBS=$(VOSLIB) \
			ole2.lib compobj.lib storage.lib shell.lib \
			ole2disp.lib typelib.lib
.ENDIF

.IF "$(GUI)"=="UNX"
SHL1DEPN= $(LIB1TARGET)
.ELSE
SHL1DEPN= $(LIB1TARGET)
.ENDIF

.IF "$(linkinc)" == ""
.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"
SHL1OBJS+=	$(SHLOBJECTS)
.ELSE
SHL1OBJS+=	$(SLO)$/so2dll.obj					\
			$(SHLOBJECTS)
.ENDIF
.ENDIF
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

.ELSE			#"$(linkinc)" == ""
SHL11FILE=	$(MISC)$/so3_data.slo
SHL12FILE=  $(MISC)$/so3_dialog.slo
SHL13FILE=  $(MISC)$/so3_inplace.slo
SHL14FILE=	$(MISC)$/vmisc.slo
.IF "$(GUI)"=="WNT"
SHL15FILE=  $(MISC)$/so3_ole.slo
.ENDIF
SHL16FILE=  $(MISC)$/so3_persist.slo
SHL17FILE=  $(MISC)$/so3_solink.slo
.ENDIF

DEF1NAME	=$(SHL1TARGET)
DEF1DEPN	=$(MISC)$/$(SHL1TARGET).flt
DEFLIB1NAME =$(TARGET)
DEF1DES 	=StarObjects

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  target.pmk


# --- SO2-Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
	@echo ------------------------------
	@echo Making: $@
	@echo WEP > $@
	@echo LIBMAIN >> $@
	@echo LibMain >> $@
	@echo CreateDdeExtern@SvBaseLink >> $@
	@echo RemoveDdeExtern@SvBaseLink >> $@
	@echo DataChanged@SvBaseLink >> $@
	@echo allocator >> $@
	@echo Exception >> $@
	@echo =@8 >> $@
	@echo __CT >> $@

