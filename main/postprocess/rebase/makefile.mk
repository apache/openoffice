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


PRJNAME=postprocess
TARGET=rebase

.INCLUDE : settings.mk

.INCLUDE : target.mk

.IF "$(COMEX)"=="14"
# Microsoft's rebase.exe is in Platform SDK v7.0 and NOT in the Windows 10 SDK,
# and Cygwin has an unrelated tool of the same name earlier on PATH.  Name the
# one we mean rather than letting PATH decide.  Empty on VC9, where the old
# SDK's Bin directory is on PATH anyway and "rebase" already resolves to it.
# := and not =, so the value is expanded HERE.  .EXPORT puts the macro's text
# into the environment, and a lazily-expanded one arrives at the shell still
# spelled $(FRAME_HOME), which sh then treats as a command substitution.
REBASE_EXE:=$(FRAME_HOME)/Bin/rebase.exe
.EXPORT : REBASE_EXE
.ENDIF

STARTADDRESS=0x68000000
BASEADDRESSES=$(MISC)$/coffbase.txt
EXCLUDELIST=no_rebase.txt
LOGFILE=$(MISC)$/rebase_log.txt
IMAGENAMES=$(SOLARBINDIR)$/*.dll $(SOLARBINDIR)$/so$/*.dll

ALLTAR : REBASE

REBASE .PHONY : $(BASEADDRESSES)
.IF "$(GUI)"=="WNT"
.IF "$(product)"=="full"
	$(PERL) rebase.pl -C $(BASEADDRESSES) -b $(STARTADDRESS) -d -e 10000 -l $(LOGFILE) -m $(MISC) -v -R $(SOLARBINDIR) -N $(EXCLUDELIST) $(IMAGENAMES)
.ELSE	# "$(product)"=="full"
	@echo Doing nothing on non product builds ...
.ENDIF	# "$(product)"=="full"
.ELSE	# "$(GUI)"=="WNT"
	@echo Nothing to do, 'rebase' is windows only.
.ENDIF

$(BASEADDRESSES) : coffbase.txt
	$(COPY) coffbase.txt $@
