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
PRJ=../..
PRJNAME=l10n
TARGET=l10n_kid

##########################################################################
#
# Use me to update the key id file.
# DO NOT USE FOR MIXED REPOSSITORIES 
#
#########################################################################

# --- Targets ------------------------------------------------------
.INCLUDE : settings.mk

MYTEMP:=$(mktmp ) 

.INCLUDE : target.mk


blah:
	$(LOCALIZE_SL) -e -l en-US -f $(MYTEMP)
	$(RM) 	$(PRJ)$/source$/kid$/localize.sdf
	$(PERL) $(SOLARVER)$/$(INPATH)$/bin$(UPDMINOREXT)$/keyidGen.pl $(MYTEMP) $(PRJ)$/source$/kid$/localize.sdf_tmp
	$(TYPE) lic.header $(PRJ)$/source$/kid$/localize.sdf_tmp | sed -e "s/\ten-US\t/\tkid\t/" > $(PRJ)$/source$/kid$/localize.sdf
	$(RM) $(MYTEMP) $(PRJ)$/source$/kid$/localize.sdf_tmp


ALLTAR : blah 

