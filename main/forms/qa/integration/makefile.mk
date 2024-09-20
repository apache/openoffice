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


.IF "$(OOO_SUBSEQUENT_TESTS)" == ""
nothing .PHONY:
.ELSE

PRJ = ..$/..
PRJNAME = forms
TARGET  = FormIntegrationTest

.IF "$(OOO_JUNIT_JAR)" != ""
PACKAGE = integration

# here store only Files which contain a @Test
JAVATESTFILES = \
    forms/RadioButtons.java \
    forms/FormControlTest.java \
    forms/FormPropertyBags.java \
    forms/CellBinding.java \
    forms/ListSelection.java \
    forms/MasterDetailForms.java \
    forms/XMLFormSettings.java \
    forms/ListBox.java

# put here all other files
JAVAFILES = $(JAVATESTFILES) \
    forms/ControlValidation.java \
    forms/SpreadsheetDocument.java \
    forms/DateValidator.java \
    forms/TimeValidator.java \
    forms/NumericValidator.java \
    forms/WaitForInput.java \
    forms/FormLayer.java \
    forms/TextValidator.java \
    forms/ListSelectionValidator.java \
    forms/DocumentType.java \
    forms/FormComponent.java \
    forms/DocumentHelper.java \
    forms/BooleanValidator.java \
    forms/dbfTools.java \
    forms/ImageComparison.java \
    forms/TableCellTextBinding.java \
    forms/ControlValidator.java \
    forms/SingleControlValidation.java \
    forms/ValueBinding.java \
    forms/DocumentViewHelper.java \
    forms/SpreadsheetView.java \
    complex/forms/tools/ResultSet.java \
    xforms/XMLDocument.java \
    xforms/Instance.java \
    xforms/Model.java

JARFILES        = ridl.jar unoil.jar jurt.jar juh.jar java_uno.jar OOoRunner.jar ConnectivityTools.jar test.jar
EXTRAJARFILES = $(OOO_JUNIT_JAR)

JAVAIFLAGS = -Dorg.openoffice.test.arg.properties=$(SRCDIR)/qa/integration/integration/forms/ListSelection.props
# Sample how to debug
# JAVAIFLAGS+=-Xdebug  -Xrunjdwp:transport=dt_socket,server=y,address=9003,suspend=y

.END

.INCLUDE: settings.mk
.INCLUDE: target.mk
.INCLUDE: installationtest.mk

ALLTAR : javatest

.END

