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



$(eval $(call gb_Library_Library,calc))

$(eval $(call gb_Library_add_precompiled_header,calc,$(SRCDIR)/connectivity/inc/pch/precompiled_calc))

$(eval $(call gb_Library_set_componentfile,calc,connectivity/source/drivers/calc/calc))

$(eval $(call gb_Library_set_include,calc,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,calc,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,calc,\
	comphelper \
	cppuhelper \
	cppu \
	file \
	dbtools \
	sal \
	stl \
	svl \
	tl \
	utl \
	vos3 \
	$(gb_STDLIBS) \
))


$(eval $(call gb_Library_add_exception_objects,calc,\
    connectivity/source/drivers/calc/CResultSet \
    connectivity/source/drivers/calc/CStatement \
    connectivity/source/drivers/calc/CPreparedStatement \
    connectivity/source/drivers/calc/CDatabaseMetaData \
    connectivity/source/drivers/calc/CCatalog \
    connectivity/source/drivers/calc/CColumns \
    connectivity/source/drivers/calc/CTable \
    connectivity/source/drivers/calc/CTables \
    connectivity/source/drivers/calc/CConnection \
    connectivity/source/drivers/calc/Cservices \
    connectivity/source/drivers/calc/CDriver \
))

# vim: set noet sw=4 ts=4:
