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



$(eval $(call gb_Library_Library,mysql))

$(eval $(call gb_Library_add_precompiled_header,mysql,$(SRCDIR)/connectivity/inc/pch/precompiled_mysql))

$(eval $(call gb_Library_set_componentfile,mysql,connectivity/source/drivers/mysql/mysql))

$(eval $(call gb_Library_set_include,mysql,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/mysql \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,mysql,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,mysql,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	sal \
	stl \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,mysql,\
    connectivity/source/drivers/mysql/YCatalog \
    connectivity/source/drivers/mysql/YColumns \
    connectivity/source/drivers/mysql/YDriver \
    connectivity/source/drivers/mysql/YTable \
    connectivity/source/drivers/mysql/YTables \
    connectivity/source/drivers/mysql/YUser \
    connectivity/source/drivers/mysql/YUsers \
    connectivity/source/drivers/mysql/YViews \
    connectivity/source/drivers/mysql/Yservices \
))

# vim: set noet sw=4 ts=4:
