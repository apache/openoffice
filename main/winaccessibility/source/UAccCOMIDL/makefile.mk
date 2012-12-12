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
TARGET=UAccComIDL

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

.IF "$(GUI)" == "WNT"

# --- Targets ----------------------------------

ALL : $(MISC)$/Accessible2	\
	  $(MISC)$/AccessibleAction \
	  $(MISC)$/AccessibleApplication \
	  $(MISC)$/AccessibleComponent \
	  $(MISC)$/AccessibleEditableText \
	  $(MISC)$/AccessibleEventId \
	  $(MISC)$/AccessibleHyperlink \
	  $(MISC)$/AccessibleHypertext \
	  $(MISC)$/AccessibleImage \
	  $(MISC)$/AccessibleKeyBinding \
	  $(MISC)$/AccessibleKeyStroke \
	  $(MISC)$/AccessibleRelation \
	  $(MISC)$/AccessibleRole \
	  $(MISC)$/AccessibleStates \
	  $(MISC)$/AccessibleTable \
	  $(MISC)$/AccessibleText \
	  $(MISC)$/AccessibleValue \
	  $(MISC)$/Charset \
	  $(MISC)$/defines \
	  $(MISC)$/IA2CommonTypes \
	  $(MISC)$/IA2TypeLibrary \
	  $(MISC)$/UAccCOM

MTL=midl.exe
MTL_PROJ=

SOURCE=Accessible2.idl
SOURCE1=AccessibleAction.idl
SOURCE2=AccessibleApplication.idl
SOURCE3=AccessibleComponent.idl
SOURCE4=AccessibleEditableText.idl
SOURCE5=AccessibleEventId.idl
SOURCE6=AccessibleHyperlink.idl
SOURCE7=AccessibleHypertext.idl
SOURCE8=AccessibleImage.idl
SOURCE9=AccessibleKeyBinding.idl
SOURCE10=AccessibleKeyStroke.idl
SOURCE11=AccessibleRelation.idl
SOURCE12=AccessibleRole.idl
SOURCE13=AccessibleStates.idl
SOURCE14=AccessibleTable.idl
SOURCE15=AccessibleText.idl
SOURCE16=AccessibleValue.idl
SOURCE17=Charset.idl
SOURCE18=defines.idl
SOURCE19=IA2CommonTypes.idl
SOURCE20=IA2TypeLibrary.idl
SOURCE21=UAccCOM.idl

MTL_SWITCHES=-tlb $(MISC)$/Accessible2.tlb -h $(MISC)$/Accessible2.h -iid $(MISC)$/Accessible2_i.c -proxy $(MISC)$/Accessible2_p.c -Oicf
MTL_SWITCHES1=-tlb $(MISC)$/AccessibleAction.tlb -h $(MISC)$/AccessibleAction.h -iid $(MISC)$/AccessibleAction_i.c -proxy $(MISC)$/AccessibleAction_p.c -Oicf
MTL_SWITCHES2=-tlb $(MISC)$/AccessibleApplication.tlb -h $(MISC)$/AccessibleApplication.h -iid $(MISC)$/AccessibleApplication_i.c -proxy $(MISC)$/AccessibleApplication_p.c -Oicf
MTL_SWITCHES3=-tlb $(MISC)$/AccessibleComponent.tlb -h $(MISC)$/AccessibleComponent.h -iid $(MISC)$/AccessibleComponent_i.c -proxy $(MISC)$/AccessibleComponent_p.c -Oicf
MTL_SWITCHES4=-tlb $(MISC)$/AccessibleEditableText.tlb -h $(MISC)$/AccessibleEditableText.h -iid $(MISC)$/AccessibleEditableText_i.c -proxy $(MISC)$/AccessibleEditableText_p.c -Oicf
MTL_SWITCHES5=-tlb $(MISC)$/AccessibleEventId.tlb -h $(MISC)$/AccessibleEventId.h -iid $(MISC)$/AccessibleEventId_i.c -proxy $(MISC)$/AccessibleEventId_p.c -Oicf
MTL_SWITCHES6=-tlb $(MISC)$/AccessibleHyperlink.tlb -h $(MISC)$/AccessibleHyperlink.h -iid $(MISC)$/AccessibleHyperlink_i.c -proxy $(MISC)$/AccessibleHyperlink_p.c -Oicf
MTL_SWITCHES7=-tlb $(MISC)$/AccessibleHypertext.tlb -h $(MISC)$/AccessibleHypertext.h -iid $(MISC)$/AccessibleHypertext_i.c -proxy $(MISC)$/AccessibleHypertext_p.c -Oicf
MTL_SWITCHES8=-tlb $(MISC)$/AccessibleImage.tlb -h $(MISC)$/AccessibleImage.h -iid $(MISC)$/AccessibleImage_i.c -proxy $(MISC)$/AccessibleImage_p.c -Oicf
MTL_SWITCHES9=-tlb $(MISC)$/AccessibleKeyBinding.tlb -h $(MISC)$/AccessibleKeyBinding.h -iid $(MISC)$/AccessibleKeyBinding_i.c -proxy $(MISC)$/AccessibleKeyBinding_p.c -Oicf
MTL_SWITCHES10=-tlb $(MISC)$/AccessibleKeyStroke.tlb -h $(MISC)$/AccessibleKeyStroke.h -iid $(MISC)$/AccessibleKeyStroke_i.c -proxy $(MISC)$/AccessibleKeyStroke_p.c -Oicf
MTL_SWITCHES11=-tlb $(MISC)$/AccessibleRelation.tlb -h $(MISC)$/AccessibleRelation.h -iid $(MISC)$/AccessibleRelation_i.c -proxy $(MISC)$/AccessibleRelation_p.c -Oicf
MTL_SWITCHES12=-tlb $(MISC)$/AccessibleRole.tlb -h $(MISC)$/AccessibleRole.h -iid $(MISC)$/AccessibleRole_i.c -proxy $(MISC)$/AccessibleRole_p.c -Oicf
MTL_SWITCHES13=-tlb $(MISC)$/AccessibleStates.tlb -h $(MISC)$/AccessibleStates.h -iid $(MISC)$/AccessibleStates_i.c -proxy $(MISC)$/AccessibleStates_p.c -Oicf
MTL_SWITCHES14=-tlb $(MISC)$/AccessibleTable.tlb -h $(MISC)$/AccessibleTable.h -iid $(MISC)$/AccessibleTable_i.c -proxy $(MISC)$/AccessibleTable_p.c -Oicf
MTL_SWITCHES15=-tlb $(MISC)$/AccessibleText.tlb -h $(MISC)$/AccessibleText.h -iid $(MISC)$/AccessibleText_i.c -proxy $(MISC)$/AccessibleText_p.c -Oicf
MTL_SWITCHES16=-tlb $(MISC)$/AccessibleValue.tlb -h $(MISC)$/AccessibleValue.h -iid $(MISC)$/AccessibleValue_i.c -proxy $(MISC)$/AccessibleValue_p.c -Oicf
MTL_SWITCHES17=-tlb $(MISC)$/Charset.tlb -h $(MISC)$/Charset.h -iid $(MISC)$/Charset_i.c -proxy $(MISC)$/Charset_p.c -Oicf
MTL_SWITCHES18=-tlb $(MISC)$/defines.tlb -h $(MISC)$/defines.h -iid $(MISC)$/defines_i.c -proxy $(MISC)$/defines_p.c -Oicf
MTL_SWITCHES19=-tlb $(MISC)$/IA2CommonTypes.tlb -h $(MISC)$/IA2CommonTypes.h -iid $(MISC)$/IA2CommonTypes_i.c -proxy $(MISC)$/IA2CommonTypes_p.c -Oicf
MTL_SWITCHES20=-tlb $(MISC)$/IA2TypeLibrary.tlb -h $(MISC)$/IA2TypeLibrary.h -iid $(MISC)$/IA2TypeLibrary_i.c -proxy $(MISC)$/IA2TypeLibrary_p.c -Oicf
MTL_SWITCHES21=-tlb $(MISC)$/UAccCOM.tlb -h $(MISC)$/UAccCOM.h -iid $(MISC)$/UAccCOM_i.c -proxy $(MISC)$/UAccCOM_p.c -Oicf

.IF "$(COM)"=="GCC"
MTL_SWITCHES+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES1+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES2+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES3+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES4+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES5+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES6+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES7+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES8+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES9+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES10+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES11+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES12+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES13+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES14+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES15+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES16+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES17+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES18+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES19+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES20+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"
MTL_SWITCHES21+=-cpp_cmd gcc -cpp_opt "-x c -E -D__stdcall= -D_stdcall= -nostdinc -I $(shell @cygpath -w $(PSDK_HOME)/include)"

$(MISC)$/Accessible2 : $(SOURCE) 
	$(MTL) $(MTL_SWITCHES) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE) 
$(MISC)$/AccessibleAction : $(SOURCE1) 
	$(MTL) $(MTL_SWITCHES1) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE1) 
$(MISC)$/AccessibleApplication : $(SOURCE2) 
	$(MTL) $(MTL_SWITCHES2) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE2) 
$(MISC)$/AccessibleComponent : $(SOURCE3) 
	$(MTL) $(MTL_SWITCHES3) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE3) 
$(MISC)$/AccessibleEditableText : $(SOURCE4) 
	$(MTL) $(MTL_SWITCHES4) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE4) 
$(MISC)$/AccessibleEventId : $(SOURCE5) 
	$(MTL) $(MTL_SWITCHES5) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE5) 
$(MISC)$/AccessibleHyperlink : $(SOURCE6) 
	$(MTL) $(MTL_SWITCHES6) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE6) 
$(MISC)$/AccessibleHypertext : $(SOURCE7) 
	$(MTL) $(MTL_SWITCHES7) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE7) 
$(MISC)$/AccessibleImage : $(SOURCE8) 
	$(MTL) $(MTL_SWITCHES8) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE8) 
$(MISC)$/AccessibleKeyBinding : $(SOURCE9) 
	$(MTL) $(MTL_SWITCHES9) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE9) 
$(MISC)$/AccessibleKeyStroke : $(SOURCE10) 
	$(MTL) $(MTL_SWITCHES10) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE10) 
$(MISC)$/AccessibleRelation : $(SOURCE11) 
	$(MTL) $(MTL_SWITCHES11) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE11) 
$(MISC)$/AccessibleRole : $(SOURCE12) 
	$(MTL) $(MTL_SWITCHES12) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE12) 
$(MISC)$/AccessibleStates : $(SOURCE13) 
	$(MTL) $(MTL_SWITCHES13) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE13)
$(MISC)$/AccessibleTable : $(SOURCE14) 
	$(MTL) $(MTL_SWITCHES14) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE14)
$(MISC)$/AccessibleText : $(SOURCE15) 
	$(MTL) $(MTL_SWITCHES15) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE15)
$(MISC)$/AccessibleValue : $(SOURCE16) 
	$(MTL) $(MTL_SWITCHES16) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE16)
$(MISC)$/Charset : $(SOURCE17) 
	$(MTL) $(MTL_SWITCHES17) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE17)
$(MISC)$/defines : $(SOURCE18) 
	$(MTL) $(MTL_SWITCHES18) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE18)
$(MISC)$/IA2CommonTypes : $(SOURCE19) 
	$(MTL) $(MTL_SWITCHES19) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE19)
$(MISC)$/IA2TypeLibrary : $(SOURCE20) 
	$(MTL) $(MTL_SWITCHES20) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE20)
$(MISC)$/UAccCOM : $(SOURCE21) 
	$(MTL) $(MTL_SWITCHES21) -I $(shell @cygpath -w $(PSDK_HOME)/include|sed 's#\\#\\\\#g') $(SOURCE21)

.ELSE

$(MISC)$/Accessible2 : $(SOURCE)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES) $(INCLUDE) $(SOURCE) && $(TOUCH) $@
$(MISC)$/AccessibleAction : $(SOURCE1)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES1) $(INCLUDE) $(SOURCE1) && $(TOUCH) $@
$(MISC)$/AccessibleApplication : $(SOURCE2)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES2) $(INCLUDE) $(SOURCE2) && $(TOUCH) $@
$(MISC)$/AccessibleComponent : $(SOURCE3)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES3) $(INCLUDE) $(SOURCE3) && $(TOUCH) $@
$(MISC)$/AccessibleEditableText : $(SOURCE4)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES4) $(INCLUDE) $(SOURCE4) && $(TOUCH) $@
$(MISC)$/AccessibleEventId : $(SOURCE5)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES5) $(INCLUDE) $(SOURCE5) && $(TOUCH) $@
$(MISC)$/AccessibleHyperlink : $(SOURCE6)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES6) $(INCLUDE) $(SOURCE6) && $(TOUCH) $@
$(MISC)$/AccessibleHypertext : $(SOURCE7)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES7) $(INCLUDE) $(SOURCE7) && $(TOUCH) $@
$(MISC)$/AccessibleImage : $(SOURCE8)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES8) $(INCLUDE) $(SOURCE8) && $(TOUCH) $@
$(MISC)$/AccessibleKeyBinding : $(SOURCE9)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES9) $(INCLUDE) $(SOURCE9) && $(TOUCH) $@
$(MISC)$/AccessibleKeyStroke : $(SOURCE10)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES10) $(INCLUDE) $(SOURCE10) && $(TOUCH) $@
$(MISC)$/AccessibleRelation : $(SOURCE11)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES11) $(INCLUDE) $(SOURCE11) && $(TOUCH) $@
$(MISC)$/AccessibleRole : $(SOURCE12)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES12) $(INCLUDE) $(SOURCE12) && $(TOUCH) $@
$(MISC)$/AccessibleStates : $(SOURCE13)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES13) $(INCLUDE) $(SOURCE13) && $(TOUCH) $@
$(MISC)$/AccessibleTable : $(SOURCE14)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES14) $(INCLUDE) $(SOURCE14) && $(TOUCH) $@
$(MISC)$/AccessibleText : $(SOURCE15)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES15) $(INCLUDE) $(SOURCE15) && $(TOUCH) $@
$(MISC)$/AccessibleValue : $(SOURCE16)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES16) $(INCLUDE) $(SOURCE16) && $(TOUCH) $@
$(MISC)$/Charset : $(SOURCE17)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES17) $(INCLUDE) $(SOURCE17) && $(TOUCH) $@
$(MISC)$/defines : $(SOURCE18)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES18) $(INCLUDE) $(SOURCE18) && $(TOUCH) $@
$(MISC)$/IA2CommonTypes : $(SOURCE19)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES19) $(INCLUDE) $(SOURCE19) && $(TOUCH) $@
$(MISC)$/IA2TypeLibrary : $(SOURCE20)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES20) $(INCLUDE) $(SOURCE20) && $(TOUCH) $@
$(MISC)$/UAccCOM : $(SOURCE21)
	$(WRAPCMD) $(MTL) $(MTL_SWITCHES21) $(INCLUDE) $(SOURCE21) && $(TOUCH) $@
.ENDIF

.ENDIF

.INCLUDE : target.mk

