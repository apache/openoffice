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



$(eval $(call gb_Library_Library,odbc))

$(eval $(call gb_Library_add_precompiled_header,odbc,$(SRCDIR)/connectivity/inc/pch/precompiled_odbc))

$(eval $(call gb_Library_set_componentfile,odbc,connectivity/source/drivers/odbc/odbc))

$(eval $(call gb_Library_set_include,odbc,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
        -I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
        -I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

ifeq ($(SYSTEM_ODBC_HEADERS),YES)
$(eval $(call gb_Library_add_cxxflags,odbc,\
        -DSYSTEM_ODBC_HEADERS \
))
endif

$(eval $(call gb_Library_add_api,odbc,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,odbc,\
	cppuhelper \
	cppu \
	odbcbase \
	sal \
	stl \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,odbc,\
	connectivity/source/drivers/odbc/oservices \
	connectivity/source/drivers/odbc/ORealDriver \
	connectivity/source/drivers/odbc/OFunctions \
))


# vim: set noet sw=4 ts=4:
