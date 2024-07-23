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



# Makefile template for directories that contain Java unit tests.
#
# Before including this makefile, the following variables should be set:
# - PRJ, PRJNAME, TARGET
# - PACKAGE
# - JAVATESTFILES: the unit test classes;
#     each one of the form <basename>.java;
#     these java files must only define classes within the package corresponding
#     to PACKAGE, the name of each defined (outer) class should end with "_Test"
# - JAVAFILES: optional java files, corresponding class files are generated
#     that can be used when compiling or running the tests;
#     each one of the form <basename>.java;
#     these java files must only define classes within the package corresponding
#     to PACKAGE, the name of each defined (outer) class should start with
#     "Test"
# - IDLTESTFILES: optional idl files, corresponding class files are generated
#     that can be used when compiling or running the tests;
#     each one of the form <basename>.idl;
#     these idl files must only define entities within the module corresponding
#     to PACKAGE, the name of each defined entity should start with "Test"
# - JARFILES: optional jar files that can be used when compiling or running the
#     tests;
#     each one of the form <basename>.jar
# All generated class files are placed into <platform>/class/test/ instead of
# <platform>/class/, so that they are not accidentally included in jar files
# packed from <platform>/class/ subdirectories.

JAVAFILES +:= $(JAVATESTFILES)
EXTRAJARFILES += $(OOO_JUNIT_JAR)
.IF "$(HAMCREST_CORE_JAR)" != ""
EXTRAJARFILES += $(HAMCREST_CORE_JAR)
.END

.INCLUDE: settings.mk

.IF "$(XCLASSPATH)" == ""
XCLASSPATH := $(CLASSDIR)
.ELSE
XCLASSPATH !:= $(XCLASSPATH)$(PATH_SEPERATOR)$(CLASSDIR)
.ENDIF
CLASSDIR !:= $(CLASSDIR)/test

.INCLUDE: target.mk

$(JAVATARGET) : $(MISC)/$(TARGET).classdir.flag

$(MISC)/$(TARGET).classdir.flag:
    - $(MKDIR) $(CLASSDIR)
    $(TOUCH) $@

.IF "$(JAVATESTFILES)" != ""
ALLTAR : test
.END

.IF "$(SOLAR_JAVA)" == "TRUE" && "$(OOO_JUNIT_JAR)" != ""
.IF "$(HAMCREST_CORE_JAR)" != ""
test .PHONY : $(JAVATARGET)
    $(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(HAMCREST_CORE_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        org.junit.runner.JUnitCore \
        $(foreach,i,$(JAVATESTFILES) $(subst,/,. $(PACKAGE)).$(subst,/,. $(i:s/.java//)))
.ELSE
test .PHONY : $(JAVATARGET)
    $(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        org.junit.runner.JUnitCore \
        $(foreach,i,$(JAVATESTFILES) $(subst,/,. $(PACKAGE)).$(subst,/,. $(i:s/.java//)))
.END
.ELSE
test .PHONY :
    echo 'test needs SOLAR_JAVA=TRUE and OOO_JUNIT_JAR'
.END

.IF "$(IDLTESTFILES)" != ""

# The following dependency (to execute javac whenever javamaker has run) does
# not work reliably, see #i28827#:
$(JAVAFILES) $(JAVACLASSFILES): $(MISC)/$(TARGET).javamaker.flag

$(MISC)/$(TARGET).javamaker.flag: $(MISC)/$(TARGET).rdb
    $(JAVAMAKER) -O$(CLASSDIR) -BUCR -nD $< -X$(SOLARBINDIR)/types.rdb
    $(TOUCH) $@

$(MISC)/$(TARGET).rdb .ERRREMOVE: \
        $(foreach,i,$(IDLTESTFILES) $(subst,.idl,.urd $(MISC)/$(TARGET)/$i))
    - rm $@
    $(REGMERGE) $@ /UCR $<

$(foreach,i,$(IDLTESTFILES) $(subst,.idl,.urd $(MISC)/$(TARGET)/$i)): \
        $(IDLTESTFILES)
    - $(MKDIR) $(MISC)/$(TARGET)
    $(IDLC) -O$(MISC)/$(TARGET) -I$(SOLARIDLDIR) -cid -we $<

.ENDIF
