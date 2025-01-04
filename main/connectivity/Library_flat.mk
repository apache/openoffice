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



$(eval $(call gb_Library_Library,flat))

$(eval $(call gb_Library_add_precompiled_header,flat,$(SRCDIR)/connectivity/inc/pch/precompiled_flat))

$(eval $(call gb_Library_set_componentfile,flat,connectivity/source/drivers/flat/flat))

$(eval $(call gb_Library_set_include,flat,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,flat,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,flat,\
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


$(eval $(call gb_Library_add_exception_objects,flat,\
    connectivity/source/drivers/flat/ECatalog \
    connectivity/source/drivers/flat/EColumns \
    connectivity/source/drivers/flat/EConnection \
    connectivity/source/drivers/flat/EDatabaseMetaData \
    connectivity/source/drivers/flat/EDriver \
    connectivity/source/drivers/flat/EPreparedStatement \
    connectivity/source/drivers/flat/EResultSet \
    connectivity/source/drivers/flat/EStatement \
    connectivity/source/drivers/flat/ETable \
    connectivity/source/drivers/flat/ETables \
    connectivity/source/drivers/flat/Eservices \
))

# vim: set noet sw=4 ts=4:
