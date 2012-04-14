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
PRJNAME=l10n
TARGET=l10n_merge

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

.IF "$(WITH_LANG)" == ""

@all:
	@echo "Nothing to do - en-US only build."
.ELSE

all_sdfs:=$(shell cd $(PRJ)/source && ls -1 */localize.sdf)

.INCLUDE .IGNORE : $(COMMONMISC)/sdf/lock.mk

.INCLUDE : target.mk

ALLTAR : $(COMMONMISC)/merge.done

$(COMMONMISC)/merge.done : $(all_sdfs)
.IF "$(L10N_LOCK)" != "YES"
	$(IFEXIST) $(COMMONMISC)/sdf $(THEN) $(RENAME) $(COMMONMISC)/sdf $(COMMONMISC)/sdf$(INPATH)_begone $(FI)
	-rm -rf $(COMMONMISC)/sdf$(INPATH)_begone
	-$(MKDIRHIER) $(COMMONMISC)/sdf
.ENDIF			# "$(L10n_LOCK)" != "YES"
	$(PERL) $(SOLARVER)/$(INPATH)/bin$(UPDMINOREXT)/fast_merge.pl -sdf_files $(mktmp $<) -merge_dir $(COMMONMISC)/sdf && $(TOUCH) $@
    $(COPY) $(PRJ)/localization_present.mk $(PRJ)/$(COMMON_OUTDIR)$(PROEXT)/inc
.ENDIF
