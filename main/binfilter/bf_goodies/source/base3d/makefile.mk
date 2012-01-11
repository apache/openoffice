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
TARGET=goodies_base3d

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_goodies

# --- Files --------------------------------------------------------

SLOFILES=\
		 $(SLO)$/goodies_b3dvolum.obj	\
		 $(SLO)$/goodies_b3dcolor.obj	\
		 $(SLO)$/goodies_b3dentty.obj	\
		 $(SLO)$/goodies_b3dtrans.obj	\
		 $(SLO)$/goodies_b3dlight.obj	\
		 $(SLO)$/goodies_matril3d.obj	\
		 $(SLO)$/goodies_b3dcompo.obj	\
		 $(SLO)$/goodies_b3dgeom.obj	\
		 $(SLO)$/goodies_vector3d.obj	\
		 $(SLO)$/goodies_point4d.obj	\
		 $(SLO)$/goodies_hmatrix.obj

.INCLUDE :  target.mk
