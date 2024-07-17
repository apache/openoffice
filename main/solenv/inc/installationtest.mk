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



# relevant for non-product builds only, but built unconditionally
.IF "$(ABORT_ON_ASSERTION)" != ""
    DBGSV_ERROR_OUT=abort
    SAL_DIAGNOSE_ABORT=TRUE
    .EXPORT: SAL_DIAGNOSE_ABORT
.ELSE
    DBGSV_ERROR_OUT=shell
.ENDIF
.EXPORT: DBGSV_ERROR_OUT

# don't allow to overwrite DBGSV_ERROR_OUT with an INI file. Otherwise, people
# might be tempted to put an DBGSV_INIT into their .bash_profile which points to a file
# delcaring to ignore all assertions completely ...
DBGSV_INIT=
.EXPORT: DBGSV_INIT

.IF "$(OS)" == "WNT"
my_file = file:///
.ELSE
my_file = file://
.END

# The following conditional is an approximation of: UPDATER set to YES and
# SHIPDRIVE set and CWS_WORK_STAMP not set and either SOL_TMP not set or
# SOLARENV set to a pathname of which SOL_TMP is not a prefix:
.IF "$(UPDATER)" == "YES" && "$(SHIPDRIVE)" != "" && \
    "$(CWS_WORK_STAMP)" == "" && "$(SOLARENV:s/$(SOL_TMP)//" == "$(SOLARENV)"
my_instsets = $(shell ls -dt \
    $(SHIPDRIVE)/$(INPATH)/Apache_OpenOffice/archive/$(WORK_STAMP)_$(LAST_MINOR)_native_packed-*_$(defaultlangiso).$(BUILD))
installationtest_instset = $(my_instsets:1)
.ELSE
installationtest_instset = \
    $(SOLARSRC)/instsetoo_native/$(INPATH)/Apache_OpenOffice/archive/install/$(defaultlangiso)
.END

installationtest_instpath = $(SOLARVERSION)/$(INPATH)/installation$(UPDMINOREXT)

.IF "$(OS)" == "WNT"
my_sofficepath = \
    $(SOLARSRC)'/instsetoo_native/$(INPATH)/Apache_OpenOffice/installed/install/en-US/Openoffice 4/program/soffice.exe'
.ELSE
my_sofficepath = \
    $(SOLARSRC)/instsetoo_native/$(INPATH)/Apache_OpenOffice/installed/install/en-US/openoffice4/program/soffice
.END

.IF "$(OOO_TEST_SOFFICE)" == ""
my_soffice = path:$(my_sofficepath)
.ELSE
my_soffice = '$(OOO_TEST_SOFFICE:s/'/'\''/)'
.END

.IF "$(OOO_LIBRARY_PATH_VAR)" != ""
my_cppenv = \
    -env:arg-env=$(OOO_LIBRARY_PATH_VAR)"$${{$(OOO_LIBRARY_PATH_VAR)+=$$$(OOO_LIBRARY_PATH_VAR)}}"
my_javaenv = \
    -Dorg.openoffice.test.arg.env=$(OOO_LIBRARY_PATH_VAR)"$${{$(OOO_LIBRARY_PATH_VAR)+=$$$(OOO_LIBRARY_PATH_VAR)}}"
.END

.IF "$(SOLAR_JAVA)" == "TRUE" && "$(OOO_JUNIT_JAR)" != ""
.IF "$(HAMCREST_CORE_JAR)" != ""
javatest_% .PHONY : $(JAVATARGET)
    $(COMMAND_ECHO)$(RM) -r $(MISC)/$(TARGET)/user
    $(COMMAND_ECHO)$(MKDIRHIER) $(MISC)/$(TARGET)/user
    $(COMMAND_ECHO)$(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(HAMCREST_CORE_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        -Dorg.openoffice.test.arg.soffice=$(my_soffice) \
        -Dorg.openoffice.test.arg.user=$(my_file)$(PWD)/$(MISC)/$(TARGET)/user \
        $(my_javaenv) $(TEST_ARGUMENTS:^"-Dorg.openoffice.test.arg.testarg.") \
        org.junit.runner.JUnitCore \
        $(subst,/,. $(PACKAGE)).$(@:s/javatest_//)
.ELSE
javatest_% .PHONY : $(JAVATARGET)
    $(COMMAND_ECHO)$(RM) -r $(MISC)/$(TARGET)/user
    $(COMMAND_ECHO)$(MKDIRHIER) $(MISC)/$(TARGET)/user
    $(COMMAND_ECHO)$(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        -Dorg.openoffice.test.arg.soffice=$(my_soffice) \
        -Dorg.openoffice.test.arg.user=$(my_file)$(PWD)/$(MISC)/$(TARGET)/user \
        $(my_javaenv) $(TEST_ARGUMENTS:^"-Dorg.openoffice.test.arg.testarg.") \
        org.junit.runner.JUnitCore \
        $(subst,/,. $(PACKAGE)).$(@:s/javatest_//)
.END
    $(RM) -r $(MISC)/$(TARGET)/user
javatest .PHONY : $(JAVATARGET)
    $(COMMAND_ECHO)$(RM) -r $(MISC)/$(TARGET)/user
    $(COMMAND_ECHO)$(MKDIRHIER) $(MISC)/$(TARGET)/user
.IF "$(HAMCREST_CORE_JAR)" != ""
    $(COMMAND_ECHO)$(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(HAMCREST_CORE_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        -Dorg.openoffice.test.arg.soffice=$(my_soffice) \
        -Dorg.openoffice.test.arg.user=$(my_file)$(PWD)/$(MISC)/$(TARGET)/user \
        $(my_javaenv) $(TEST_ARGUMENTS:^"-Dorg.openoffice.test.arg.testarg.") \
        org.junit.runner.JUnitCore \
        $(foreach,i,$(JAVATESTFILES) $(subst,/,. $(PACKAGE)).$(subst,/,. $(i:s/.java//)))
.ELSE
    $(COMMAND_ECHO)$(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        -Dorg.openoffice.test.arg.soffice=$(my_soffice) \
        -Dorg.openoffice.test.arg.user=$(my_file)$(PWD)/$(MISC)/$(TARGET)/user \
        $(my_javaenv) $(TEST_ARGUMENTS:^"-Dorg.openoffice.test.arg.testarg.") \
        org.junit.runner.JUnitCore \
        $(foreach,i,$(JAVATESTFILES) $(subst,/,. $(PACKAGE)).$(subst,/,. $(i:s/.java//)))
.END
    $(RM) -r $(MISC)/$(TARGET)/user
.ELSE
javatest .PHONY :
    @echo 'javatest needs SOLAR_JAVA=TRUE and OOO_JUNIT_JAR'
.END
