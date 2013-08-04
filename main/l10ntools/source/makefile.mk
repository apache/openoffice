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



PRJ=..

INCPRE=$(MISC)

PRJNAME=l10ntools
TARGET=genLang
TARGETTYPE=CUI
LIBTARGET=no
# --- Settings -----------------------------------------------------
ENABLE_EXCEPTIONS=TRUE

.INCLUDE :  settings.mk
CDEFS+= -DYY_NEVER_INTERACTIVE=1
#CDEFS+= -pg

.IF "$(SYSTEM_EXPAT)" == "YES"
CFLAGS+=-DSYSTEM_EXPAT
.ENDIF

	
# --- Files --------------------------------------------------------


OBJFILES= $(OBJ)$/gLang.obj        \
          $(OBJ)$/gL10nMem.obj     \
          $(OBJ)$/gL10nMemDB.obj   \
          $(OBJ)$/gHandler.obj     \
          $(OBJ)$/gConProp.obj     \
          $(OBJ)$/gCon.obj         \
          $(OBJ)$/gConDB.obj       \
          $(OBJ)$/gConPoWrap.obj   \
          $(OBJ)$/gConSrcWrap.obj  \
          $(OBJ)$/gConXrmWrap.obj  \
          $(OBJ)$/gConXhpWrap.obj  \
          $(OBJ)$/gConXcsWrap.obj  \
          $(OBJ)$/gConXcuWrap.obj  \
          $(OBJ)$/gConUlfWrap.obj  \
          $(OBJ)$/gConTreeWrap.obj


# localizer for new l10n framework
APP1TARGET=   genLang 
APP1OBJS=     $(OBJFILES)
APP1RPATH=    NONE
APP1LINKTYPE= STATIC

APP1LIBS= 
APP1STDLIBS= 
APP1LIBSALCPPRT=


# --- Targets ------------------------------------------------------
.INCLUDE :  target.mk

$(MISC)$/%_yy.c : %lex.l
	flex -l -w -8 -o$@ $<

# --- Files --------------------------------------------------------
$(OBJ)$/gConPoWrap.obj:   $(MISC)$/gConPo_yy.c
$(OBJ)$/gConSrcWrap.obj:  $(MISC)$/gConSrc_yy.c
$(OBJ)$/gConXcuWrap.obj:  $(MISC)$/gConXcu_yy.c
$(OBJ)$/gConXcsWrap.obj:  $(MISC)$/gConXcs_yy.c
$(OBJ)$/gConXrmWrap.obj:  $(MISC)$/gConXrm_yy.c
$(OBJ)$/gConXhpWrap.obj:  $(MISC)$/gConXhp_yy.c
$(OBJ)$/gConUlfWrap.obj:  $(MISC)$/gConUlf_yy.c
$(OBJ)$/gConTreeWrap.obj: $(MISC)$/gConTree_yy.c
