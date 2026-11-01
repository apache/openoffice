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



$(eval $(call gb_Library_Library,dbpool2))

$(eval $(call gb_Library_add_precompiled_header,dbpool2,$(SRCDIR)/connectivity/inc/pch/precompiled_dbpool2))

$(eval $(call gb_Library_set_componentfile,dbpool2,connectivity/source/cpool/dbpool2))

$(eval $(call gb_Library_set_include,dbpool2,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
))

$(eval $(call gb_Library_add_api,dbpool2,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,dbpool2,\
	cppuhelper \
	cppu \
	dbtools \
	sal \
	stl \
	vos3 \
	$(gb_STDLIBS) \
))


$(eval $(call gb_Library_add_exception_objects,dbpool2,\
	connectivity/source/cpool/ZConnectionWrapper \
	connectivity/source/cpool/ZDriverWrapper \
	connectivity/source/cpool/ZPooledConnection \
	connectivity/source/cpool/ZConnectionPool \
	connectivity/source/cpool/ZPoolCollection \
	connectivity/source/cpool/Zregistration \
))

# vim: set noet sw=4 ts=4:
