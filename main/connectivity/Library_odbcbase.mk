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



$(eval $(call gb_Library_Library,odbcbase))

$(eval $(call gb_Library_add_precompiled_header,odbcbase,$(SRCDIR)/connectivity/inc/pch/precompiled_odbcbase))

$(eval $(call gb_Library_set_include,odbcbase,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
        -I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
        -I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_cxxflags,odbcbase,\
	-DOOO_DLLIMPLEMENTATION_ODBCBASE \
))

ifeq ($(SYSTEM_ODBC_HEADERS),YES)
$(eval $(call gb_Library_add_cxxflags,odbcbase,\
	-DSYSTEM_ODBC_HEADERS \
))
endif

$(eval $(call gb_Library_add_api,odbcbase,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,odbcbase,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	sal \
	stl \
	vos3 \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,odbcbase,\
	connectivity/source/drivers/odbcbase/OPreparedStatement \
	connectivity/source/drivers/odbcbase/OStatement \
	connectivity/source/drivers/odbcbase/OResultSetMetaData \
	connectivity/source/drivers/odbcbase/OResultSet \
	connectivity/source/drivers/odbcbase/OTools \
	connectivity/source/drivers/odbcbase/ODatabaseMetaDataResultSet \
	connectivity/source/drivers/odbcbase/ODatabaseMetaData \
	connectivity/source/drivers/odbcbase/ODriver \
	connectivity/source/drivers/odbcbase/OConnection \
))

# vim: set noet sw=4 ts=4:
