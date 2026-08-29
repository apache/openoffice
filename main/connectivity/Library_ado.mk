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



$(eval $(call gb_Library_Library,ado))

$(eval $(call gb_Library_add_precompiled_header,ado,$(SRCDIR)/connectivity/inc/pch/precompiled_ado))

$(eval $(call gb_Library_set_componentfile,ado,connectivity/source/drivers/ado/ado))

$(eval $(call gb_Library_set_include,ado,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/ado \
        -I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,ado,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,ado,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	ole32 \
	oleaut32 \
	sal \
	stl \
	uuid \
	vos3 \
	$(gb_STDLIBS) \
))


$(eval $(call gb_Library_add_exception_objects,ado,\
    connectivity/source/drivers/ado/ACallableStatement \
    connectivity/source/drivers/ado/ACatalog \
    connectivity/source/drivers/ado/AColumn \
    connectivity/source/drivers/ado/AColumns \
    connectivity/source/drivers/ado/AConnection \
    connectivity/source/drivers/ado/ADatabaseMetaData \
    connectivity/source/drivers/ado/ADatabaseMetaDataImpl \
    connectivity/source/drivers/ado/ADatabaseMetaDataResultSet \
    connectivity/source/drivers/ado/ADatabaseMetaDataResultSetMetaData \
    connectivity/source/drivers/ado/ADriver \
    connectivity/source/drivers/ado/AGroup \
    connectivity/source/drivers/ado/AGroups \
    connectivity/source/drivers/ado/AIndex \
    connectivity/source/drivers/ado/AIndexes \
    connectivity/source/drivers/ado/AKey \
    connectivity/source/drivers/ado/AKeys \
    connectivity/source/drivers/ado/Aolevariant \
    connectivity/source/drivers/ado/APreparedStatement \
    connectivity/source/drivers/ado/AResultSet \
    connectivity/source/drivers/ado/AResultSetMetaData \
    connectivity/source/drivers/ado/Aservices \
    connectivity/source/drivers/ado/AStatement \
    connectivity/source/drivers/ado/ATable \
    connectivity/source/drivers/ado/ATables \
    connectivity/source/drivers/ado/AUser \
    connectivity/source/drivers/ado/AUsers \
    connectivity/source/drivers/ado/AView \
    connectivity/source/drivers/ado/AViews \
))

$(eval $(call gb_Library_add_noexception_objects,ado,\
    connectivity/source/drivers/ado/Awrapado \
    connectivity/source/drivers/ado/adoimp \
))

# vim: set noet sw=4 ts=4:
