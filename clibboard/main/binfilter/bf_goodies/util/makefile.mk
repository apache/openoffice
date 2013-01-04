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
TARGET=bf_go

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

LIBTARGET=NO

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_go

# --- Files --------------------------------------------------------

LIB1TARGET=$(SLB)$/bf_go.lib
LIB1FILES=\
   $(SLB)$/goodies_base2d.lib	\
   $(SLB)$/goodies_base3d.lib

SHL1TARGET= bf_go$(DLLPOSTFIX)
SHL1IMPLIB= bf_go
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
SHL1LIBS=	$(SLB)$/bf_go.lib

DEF1NAME	=$(SHL1TARGET)
DEF1DEPN	=$(MISC)$/$(SHL1TARGET).flt \
		$(LIB1TARGET)

DEF1DES		=Bf_Goodies
DEFLIB1NAME	=bf_go
#DEF1EXPORTFILE	=bf_goodies.dxp

SHL1STDLIBS= \
			$(SVTOOLLIB)		\
			$(TOOLSLIB)			\
			$(SALLIB)

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk

# --- Goodies-Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
	@echo ------------------------------
	@echo Making: $@
        @echo _Impl>$@
	@echo WEP>>$@
        @echo m_pLoader>$@
	@echo LIBMAIN>>$@
	@echo LibMain>>$@
        @echo CT>>$@
