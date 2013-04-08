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



PRJ		= ..$/..
PRJNAME = helpcontent2
TARGET  = auxiliary

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

TREEFILES  = 

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.EXPORT : LOCALIZESDF LOCALIZATION_FOUND TRYSDF

ALLTAR : aux_dirs $(COMMONMISC)$/treefiles.done $(COMMONBIN)$/helpimg.ilst

.IF "$(WITH_LANG)"!=""
t_aux_langdirs:=$(shell @find ./ -name "*.cfg" | sed "s/\.\/\///" | sed "s/\.\///" )

aux_langdirs:=$(uniq $(t_aux_langdirs:d:d))

aux_alllangiso:=$(foreach,i,$(alllangiso) $(foreach,j,$(aux_langdirs) $(eq,$i,$j  $i $(NULL))))

WITH_LANG!:=$(aux_alllangiso)

.EXPORT : WITH_LANG

LOCTREEFILES:=$(foreach,i,$(aux_alllangiso) $(foreach,j,$(TREEFILES) $(COMMONMISC)$/$i$/$j))
.ELSE			#IF "$(WITH_LANG)"!=""
aux_langdirs:=en-US
LOCTREEFILES:=$(foreach,j,$(TREEFILES) $(COMMONMISC)$/en-US$/$j)
.ENDIF			#IF "$(WITH_LANG)"!=""

$(COMMONMISC)$/treefiles.done : $(LOCTREEFILES)
	+$(PERL) $(PRJ)$/helpers$/update_tree.pl && $(TOUCH) $@

%.created:
	@-$(MKDIRHIER) $(@:d) && $(TOUCH) $@

JAN : $(TREEFILES) $$(@:d)$/dir.created
	@$(TOUCH) $@

.IF "$(WITH_LANG)"!=""
.IF "$(LOCALIZATION_FOUND)"=="YES"
JAN : $(TRYSDF:d:d:d:d)$/text$/shared/localize.sdf
.ELSE			# "$(LOCALIZATION_FOUND)"=="YES"
$(COMMONMISC)/unpack.done : $(SOLARCOMMONSDFDIR)$/$(PRJNAME).zip
    @@-$(MKDIRHIER) $(COMMONMISC)$/$(PRJNAME)_$(TARGET)
	unzip -o -d $(COMMONMISC)$/$(PRJNAME) $(SOLARCOMMONSDFDIR)$/$(PRJNAME).zip 
    $(TOUCH) $@

$(LOCTREEFILES) : $(COMMONMISC)/unpack.done

.ENDIF			# "$(LOCALIZATION_FOUND)"=="YES"
.ENDIF			# "$(WITH_LANG)"!=""
aux_dirs .PHONY :
    echo aux_langdirs:=$(aux_langdirs) > $(LOCAL_COMMON_OUT)/inc$/aux_langs.mk

$(COMMONBIN)$/helpimg.ilst .PHONY:
	-$(RM) $@
	$(PERL) $(PRJ)$/helpers$/create_ilst.pl -dir=$(SOLARSRC)/default_images/res/helpimg > $@.$(INPATH)
	$(RENAME) $@.$(INPATH) $@
	

