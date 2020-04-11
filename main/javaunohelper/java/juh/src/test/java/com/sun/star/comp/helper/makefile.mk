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



PRJ		=..$/..$/..$/..$/..$/..
PRJNAME = juhelper
PACKAGE = com$/sun$/star$/comp$/helper
TARGET  = com_sun_star_comp_helper_test

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

JARFILES = jurt.jar ridl.jar

JAVACLASSFILES=	\
    $(CLASSDIR)$/$(PACKAGE)$/ComponentContext_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/SharedLibraryLoader_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/RegistryServiceFactory_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/Bootstrap_Test.class

JAVAFILES= $(subst,$(CLASSDIR)$/$(PACKAGE)$/, $(subst,.class,.java $(JAVACLASSFILES)))

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

