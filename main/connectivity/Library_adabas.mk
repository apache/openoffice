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



$(eval $(call gb_Library_Library,adabas))

$(eval $(call gb_Library_add_precompiled_header,adabas,$(SRCDIR)/connectivity/inc/pch/precompiled_adabas))

$(eval $(call gb_Library_set_componentfile,adabas,connectivity/source/drivers/adabas/adabas))

$(eval $(call gb_Library_set_include,adabas,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/adabas \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

ifeq ($(SYSTEM_ODBC_HEADERS),YES)
$(eval $(call gb_Library_add_cxxflags,adabas,\
	-DSYSTEM_ODBC_HEADERS \
))
endif

$(eval $(call gb_Library_add_api,adabas,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,adabas,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	odbcbase \
	sal \
	stl \
	tl \
	utl \
	vos3 \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,adabas,\
    connectivity/source/drivers/adabas/BCatalog \
    connectivity/source/drivers/adabas/BColumns \
    connectivity/source/drivers/adabas/BConnection \
    connectivity/source/drivers/adabas/BDatabaseMetaData \
    connectivity/source/drivers/adabas/BDriver \
    connectivity/source/drivers/adabas/BFunctions \
    connectivity/source/drivers/adabas/BGroup \
    connectivity/source/drivers/adabas/BGroups \
    connectivity/source/drivers/adabas/BIndex \
    connectivity/source/drivers/adabas/BIndexColumns \
    connectivity/source/drivers/adabas/BIndexes \
    connectivity/source/drivers/adabas/BKeys \
    connectivity/source/drivers/adabas/BPreparedStatement \
    connectivity/source/drivers/adabas/BResultSet \
    connectivity/source/drivers/adabas/BResultSetMetaData \
    connectivity/source/drivers/adabas/BStatement \
    connectivity/source/drivers/adabas/BTable \
    connectivity/source/drivers/adabas/BTables \
    connectivity/source/drivers/adabas/BUser \
    connectivity/source/drivers/adabas/BUsers \
    connectivity/source/drivers/adabas/BViews \
    connectivity/source/drivers/adabas/Bservices \
))

# vim: set noet sw=4 ts=4:
