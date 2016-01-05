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
PRJ=../..
PRJNAME=l10n
TARGET=l10n_update

#########################################################################
#
# Use me if you make minor changes in the l10n/*/localize.sdf files to merge the change into the output tree without removeing / remerging all and everything
#                    ^^^^^ ^^^^^^^
########################################################################

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

.IF "$(WITH_LANG)" == ""

@all:
	@echo "Nothing to do - en-US only build."
.ELSE

all_sdfs:=$(shell ls -1 $(PRJ)$/source$/*$/localize.sdf)

.INCLUDE .IGNORE : $(COMMONMISC)$/sdf$/lock.mk

.INCLUDE : target.mk

ALLTAR : $(COMMONMISC)$/merge.done

$(COMMONMISC)$/merge.done : $(all_sdfs)
	$(foreach,j,$? $(PERL) $(SOLARVER)$/$(INPATH)$/bin$(UPDMINOREXT)$/localize_old.pl -m -v -l all -s $(COMMONMISC)$/sdf -f $j && $(TOUCH) $@ ; )

#$(COMMONMISC)$/merge.done : $(COMMONMISC)/{$(all_sdfs:d:d:f)}_merge.done

#$(COMMONMISC)$/%_merge.done : $(PRJ)/source/%/localize.sdf
#	$(PERL) $(SOLARVER)$/$(INPATH)$/bin$(UPDMINOREXT)$/localize_old.pl -m -v -l all -s $(COMMONMISC)$/sdf -f $? && $(TOUCH) $@

.ENDIF
