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
TARGET=l10n_zip

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

.IF "$(WITH_LANG)" == ""

@all:
	@echo "Nothing to do - en-US only build."
.ELSE

all_modules:=$(shell cd $(COMMONMISC)$/sdf && ls -1 )

.INCLUDE : target.mk

ALLTAR : $(foreach,i,$(all_modules) $(COMMONBIN)$/$i.zip)

$(COMMONBIN)$/%.zip : $(COMMONMISC)$/merge.done
	$(CDD) $(COMMONMISC)$/sdf$/$(@:b) && zip -r ..$/..$/..$/bin$/$(@:b)_$(INPATH).zip * $(CHECKZIPRESULT)
	$(RM) $@
	$(RENAME) $(@:d)$(@:b)_$(INPATH).zip $@

.ENDIF

