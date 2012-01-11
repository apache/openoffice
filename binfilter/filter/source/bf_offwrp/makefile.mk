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



EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..$/..$/..
BFPRJ=..$/..

PRJNAME=binfilter
TARGET=bf_wrapper

NO_HIDS=TRUE

LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

SLOFILES =	$(SLO)$/bf_wrapper.obj

SHL1OBJS=   $(SLOFILES)
SHL1TARGET= $(TARGET)$(DLLPOSTFIX)

SHL1IMPLIB= ibf_wrp
SHL1STDLIBS=    \
				$(BFSO3LIB) 			\
				$(BFSVTOOLLIB) 		\
				$(BFOFALIB)			\
				$(VCLLIB) 			\
				$(SOTLIB) 			\
				$(SALLIB)			\
				$(TOOLSLIB)			\
				$(CPPULIB)			\
				$(CPPUHELPERLIB)	\
				$(BFSCHLIB)         \
				$(BFSMLIB)          \
				$(BFSWLIB)          \
				$(BFSDLIB)          \
				$(BFSCLIB)          \
				$(BFSVXLIB)         \
				$(UNOTOOLSLIB)		\
				$(COMPHELPERLIB)	\
				$(LEGACYSMGRLIB)

SHL1DEPN=       makefile.mk
SHL1DEF=        $(MISC)$/$(SHL1TARGET).def

DEF1NAME=       $(SHL1TARGET)
DEF1EXPORTFILE= exports.dxp

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

ALLTAR : $(MISC)/bf_wrapper.component

$(MISC)/bf_wrapper.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        bf_wrapper.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_wrapper.component
