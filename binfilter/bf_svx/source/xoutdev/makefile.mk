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

NO_HIDS=TRUE
TARGET=svx_xout
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

# --- Files --------------------------------------------------------

SLOFILES= \
		$(SLO)$/svx_xout.obj   		\
		$(SLO)$/svx_xout1.obj  		\
		$(SLO)$/svx_xattr.obj  		\
		$(SLO)$/svx_xattr2.obj  	\
		$(SLO)$/svx_xattrbmp.obj	\
		$(SLO)$/svx_xline.obj   	\
		$(SLO)$/svx_xpool.obj  		\
		$(SLO)$/svx_xtable.obj 		\
		$(SLO)$/svx_xtabcolr.obj	\
		$(SLO)$/svx_xtablend.obj	\
		$(SLO)$/svx_xtabdash.obj	\
		$(SLO)$/svx_xtabhtch.obj	\
		$(SLO)$/svx_xtabgrdt.obj	\
		$(SLO)$/svx_xtabbtmp.obj	\
		$(SLO)$/svx_xiocomp.obj		\
		$(SLO)$/svx__xpoly.obj  	\
		$(SLO)$/svx__ximp.obj   	\
		$(SLO)$/svx__xoutbmp.obj   	\
		$(SLO)$/svx__xfont.obj

HXXCOPYFILES=\
	 $(PRJ)$/inc$/bf_svx$/xenum.hxx


.INCLUDE :  target.mk
