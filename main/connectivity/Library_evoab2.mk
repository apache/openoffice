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



$(eval $(call gb_Library_Library,evoab2))

$(eval $(call gb_Library_add_precompiled_header,evoab2,$(SRCDIR)/connectivity/inc/pch/precompiled_evoab2))

$(eval $(call gb_Library_set_componentfile,evoab2,connectivity/source/drivers/evoab2/evoab))

$(eval $(call gb_Library_set_include,evoab2,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/evoab2 \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
	$(filter -I%,$(GTK_CFLAGS)) \
	$(filter -I%,$(GOBJECT_CFLAGS)) \
))

$(eval $(call gb_Library_add_cxxflags,evoab2,\
	$(filter-out -I%,$(GTK_CFLAGS)) \
	$(filter-out -I%,$(GOBJECT_CFLAGS)) \
))

$(eval $(call gb_Library_add_api,evoab2,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,evoab2,\
	comphelper \
	cppuhelper \
	cppu \
	file \
	dbtools \
	sal \
	stl \
	svl \
	tl \
	ucbhelper \
	utl \
	vos3 \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_libs,evoab2,\
	$(GTK_LIBS) \
	$(GOBJECT_LIBS) \
))

$(eval $(call gb_Library_add_exception_objects,evoab2,\
    connectivity/source/drivers/evoab2/NCatalog \
    connectivity/source/drivers/evoab2/NColumns \
    connectivity/source/drivers/evoab2/NConnection \
    connectivity/source/drivers/evoab2/NDatabaseMetaData \
    connectivity/source/drivers/evoab2/NDriver \
    connectivity/source/drivers/evoab2/NPreparedStatement \
    connectivity/source/drivers/evoab2/NResultSet \
    connectivity/source/drivers/evoab2/NResultSetMetaData \
    connectivity/source/drivers/evoab2/NStatement \
    connectivity/source/drivers/evoab2/NTable \
    connectivity/source/drivers/evoab2/NTables \
    connectivity/source/drivers/evoab2/NServices \
    connectivity/source/drivers/evoab2/EApi \
    connectivity/source/drivers/evoab2/NDebug \
))

# vim: set noet sw=4 ts=4:
