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




# edit to match directory level 
PRJ		= ..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
.IF "$(MAKETARGETS)"!="genPO"

# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = util_shared

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

#common_build_zip:=
zip1generatedlangs=TRUE
zip1langdirs=$(aux_alllangiso)
ZIP1TARGET=xhp_shared
ZIP1FLAGS= -u -r
ZIP1DIR=$(COMMONMISC)$/$(LANGDIR)
ZIP1LIST=$(LANGDIR)$/text$/shared$/* -x "*.xhp????*" -x "*.dphh*" -x "*.hzip" -x "*.created"

LINKNAME=shared
LINKADDEDFILES= \
   -add shared.tree $(COMMONMISC)$/LANGUAGE$/shared.tree \
   -add shared.jar  $(COMMONBIN)$/xhp_shared_LANGUAGE.zip \
   -add default.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/default.css \
   -add highcontrast1.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast1.css \
   -add highcontrast2.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast2.css \
   -add highcontrastwhite.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastwhite.css \
   -add highcontrastblack.css  $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastblack.css \
   -add err.html  $(COMMONMISC)$/LANGUAGE$/text$/shared$/05$/err_html.xhp


LINKADDEDDEPS= \
   $(COMMONMISC)$/LANGUAGE$/shared.tree \
   $(COMMONBIN)$/xhp_shared_LANGUAGE.zip \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/default.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast1.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrast2.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastwhite.css \
   $(PRJ)$/source$/auxiliary$/LANGUAGE$/highcontrastblack.css \
   $(COMMONMISC)$/LANGUAGE$/text$/shared$/05$/err_html.xhp


LINKLINKFILES= \


# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
.ELSE
.INCLUDE :  target.mk
.ENDIF
