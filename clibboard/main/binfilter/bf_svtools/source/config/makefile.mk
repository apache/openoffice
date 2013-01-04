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


PRJ=..$/..$/..

PRJNAME=binfilter
TARGET=config
ENABLE_EXCEPTIONS := TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- Files --------------------------------------------------------

SLOFILES = \
    $(SLO)$/svt_moduleoptions.obj           \
    $(SLO)$/svt_pathoptions.obj             \
	$(SLO)$/svt_saveopt.obj					\
    $(SLO)$/svt_lingucfg.obj                \
	$(SLO)$/svt_securityoptions.obj			\
	$(SLO)$/svt_localisationoptions.obj		\
	$(SLO)$/svt_workingsetoptions.obj		\
	$(SLO)$/svt_viewoptions.obj				\
	$(SLO)$/svt_internaloptions.obj			\
	$(SLO)$/svt_startoptions.obj			\
	$(SLO)$/svt_historyoptions.obj			\
	$(SLO)$/svt_inetoptions.obj				\
	$(SLO)$/svt_menuoptions.obj				\
	$(SLO)$/svt_dynamicmenuoptions.obj		\
	$(SLO)$/svt_options3d.obj				\
	$(SLO)$/svt_fontoptions.obj				\
	$(SLO)$/svt_addxmltostorageoptions.obj	\
	$(SLO)$/svt_defaultoptions.obj			\
	$(SLO)$/svt_searchopt.obj				\
	$(SLO)$/svt_printwarningoptions.obj		\
    $(SLO)$/svt_cacheoptions.obj            \
	$(SLO)$/svt_cmdoptions.obj				\
	$(SLO)$/svt_extendedsecurityoptions.obj \
	$(SLO)$/svt_sourceviewconfig.obj		\
	$(SLO)$/svt_compatibility.obj           \
    $(SLO)$/svt_optionsdlg.obj              \
    $(SLO)$/svt_itemholder1.obj \
    $(SLO)$/svt_options.obj \
	$(SLO)$/svt_misccfg.obj				\
    $(SLO)$/svt_helpopt.obj                 \
	$(SLO)$/svt_printoptions.obj			\
    $(SLO)$/svt_languageoptions.obj     \
    $(SLO)$/svt_ctloptions.obj          \
    $(SLO)$/svt_cjkoptions.obj          \
    $(SLO)$/svt_colorcfg.obj            \
    $(SLO)$/svt_undoopt.obj                 \
    $(SLO)$/svt_useroptions.obj      \
    $(SLO)$/svt_syslocaleoptions.obj \
	$(SLO)$/svt_itemholder2.obj \
	$(SLO)$/svt_miscopt.obj

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

