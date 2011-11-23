#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: rt $ $Date: 2005/09/08 17:56:37 $
#
#   The Contents of this file are made available
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#
#*************************************************************************


PRJ = ..

PRJNAME	= dictionaries
TARGET	= dict_hr_HR

#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

.IF "$(DIC_ALL)$(DIC_HRHR)"!="" 

DIC2BIN= \
	 	hr_HR.aff \
			hr_HR.dic \
					hyph_hr_HR.dic

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk
	.INCLUDE : $(PRJ)$/util$/target.pmk

.IF "$(DIC_ALL)$(DIC_HRHR)"!=""



.ENDIF			# "$(DIC_ALL)$(DIC_ENUS)"!="" 

