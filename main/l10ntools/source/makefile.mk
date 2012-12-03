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

# localizer for new l10n framework
# 
APP1TARGET= genLang
APP1OBJS= $(OBJ)$/gLang.obj \
          $(OBJ)$/gL10nMem.obj \
          $(OBJ)$/gHandler.obj \
          $(OBJ)$/gConProp.obj \
          $(OBJ)$/gConSrc.obj \
          $(OBJ)$/gConSrcUtil.obj \
          $(OBJ)$/gConTree.obj \
          $(OBJ)$/gConUlf.obj \
          $(OBJ)$/gConPo.obj \
          $(OBJ)$/gConXcs.obj \
          $(OBJ)$/gConXcu.obj \
          $(OBJ)$/gConXhp.obj \
          $(OBJ)$/gConXrm.obj \
          $(OBJ)$/gCon.obj \
          $(OBJ)$/gConPoWrap.obj \
          $(OBJ)$/gConSrcWrap.obj \
          $(OBJ)$/gConXcsWrap.obj \
          $(OBJ)$/gConXcuWrap.obj \
          $(OBJ)$/gConXrmWrap.obj \
          $(OBJ)$/gConXhpWrap.obj 
APP1RPATH=  NONE
APP1STDLIBS+= $(TOOLSLIB) \
              $(VOSLIB) \
              $(SALLIB)

DEPOBJFILES=$(APP1OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

$(MISC)$/%_yy.c : %lex.l
	flex -l -w -8 -o$@ $<

# Helper to suppress warnings in lex generated c code, see #i57362#

$(OBJ)$/gConPoWrap.obj: $(MISC)$/gConPo_yy.c
$(OBJ)$/gConSrcWrap.obj: $(MISC)$/gConSrc_yy.c
$(OBJ)$/gConXcuWrap.obj: $(MISC)$/gConXcu_yy.c
$(OBJ)$/gConXcsWrap.obj: $(MISC)$/gConXcs_yy.c
$(OBJ)$/gConXrmWrap.obj: $(MISC)$/gConXrm_yy.c
$(OBJ)$/gConXhpWrap.obj: $(MISC)$/gConXhp_yy.c

