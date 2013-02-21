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

.INCLUDE :	settings.mk

version=4.0

.IF "$(RAT_JAR_HOME)"=="BUILD"
rat_jar_home=$(CLASSDIR)
.ELSE
rat_jar_home=$(RAT_JAR_HOME)
.ENDIF
.IF "$(OS)"=="WNT"
rat_jar_home!:=$(shell cygpath -u $(rat_jar_home))
.ENDIF

rat_scan_output:=$(MISC)/rat-output.xml
.IF "$(OS)"=="WNT"
rat_scan_output!:=$(shell cygpath -m $(rat_scan_output))
.ENDIF

all : $(MISC)/aoo-$(version)_rat-scan-output.html

show-excludes .PHONY : $(MISC)/excluded-files.txt

$(MISC)/aoo-$(version)_rat-scan-output.html : $(MISC)/rat-scan-output.xml
	java -jar $(OUTDIR)/bin/saxon9.jar -t -s:$< -xsl:rat-output-to-html.xsl -o:$@ -l:on -warnings:fatal
	@echo
	@echo Find rat scan report in $@

$(MISC)/rat-scan-output.xml : rat-scan.xml $(SRC_ROOT)/rat-excludes
	$(ANT) -f rat-scan.xml -DRAT_SCAN_OUTPUT=$@ -DRAT_JAR_HOME=$(rat_jar_home)

$(MISC)/excluded-files.txt : $(MISC)/excluded-files-raw.txt
	cat $< | sed 's/;/\n/g' > $@

$(MISC)/excluded-files-raw.txt : rat-scan.xml $(SRC_ROOT)/rat-excludes
	$(ANT) -f rat-scan.xml -DEXCLUDED_FILES_OUTPUT=$@ -DEXCLUDED_SOLVER_OUTPUT=$@ \
		-DRAT_JAR_HOME=$(rat_jar_home) show-excluded-files
