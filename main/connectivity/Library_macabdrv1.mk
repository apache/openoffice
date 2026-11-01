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



$(eval $(call gb_Library_Library,macabdrv1))

$(eval $(call gb_Library_add_precompiled_header,macabdrv1,$(SRCDIR)/connectivity/inc/pch/precompiled_macabdrv1))

$(eval $(call gb_Library_set_include,macabdrv1,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/macab \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,macabdrv1,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_libs,macabdrv1,\
	-framework Carbon \
	-framework AddressBook \
))

$(eval $(call gb_Library_add_linked_libs,macabdrv1,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	sal \
	stl \
	vos3 \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,macabdrv1,\
    connectivity/source/drivers/macab/MacabColumns \
    connectivity/source/drivers/macab/MacabTable \
    connectivity/source/drivers/macab/MacabTables \
    connectivity/source/drivers/macab/MacabCatalog \
    connectivity/source/drivers/macab/MacabResultSet \
    connectivity/source/drivers/macab/MacabStatement \
    connectivity/source/drivers/macab/MacabPreparedStatement \
    connectivity/source/drivers/macab/MacabDatabaseMetaData \
    connectivity/source/drivers/macab/MacabConnection \
    connectivity/source/drivers/macab/MacabResultSetMetaData \
    connectivity/source/drivers/macab/macabcondition \
    connectivity/source/drivers/macab/macaborder \
    connectivity/source/drivers/macab/MacabRecord \
    connectivity/source/drivers/macab/MacabRecords \
    connectivity/source/drivers/macab/MacabHeader \
    connectivity/source/drivers/macab/MacabGroup \
    connectivity/source/drivers/macab/MacabAddressBook \
))

# vim: set noet sw=4 ts=4:
