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



$(eval $(call gb_Library_Library,jdbc))

$(eval $(call gb_Library_add_precompiled_header,jdbc,$(SRCDIR)/connectivity/inc/pch/precompiled_jdbc))

$(eval $(call gb_Library_set_componentfile,jdbc,connectivity/source/drivers/jdbc/jdbc))

$(eval $(call gb_Library_set_include,jdbc,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/jdbc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,jdbc,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,jdbc,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	jvmaccess \
	jvmfwk \
	sal \
	stl \
	utl \
	vos3 \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,jdbc,\
    connectivity/source/drivers/jdbc/Array \
    connectivity/source/drivers/jdbc/Blob \
    connectivity/source/drivers/jdbc/Boolean \
    connectivity/source/drivers/jdbc/CallableStatement \
    connectivity/source/drivers/jdbc/Class \
    connectivity/source/drivers/jdbc/Clob \
    connectivity/source/drivers/jdbc/ConnectionLog \
    connectivity/source/drivers/jdbc/DatabaseMetaData \
    connectivity/source/drivers/jdbc/Date \
    connectivity/source/drivers/jdbc/DriverPropertyInfo \
    connectivity/source/drivers/jdbc/Exception \
    connectivity/source/drivers/jdbc/InputStream \
    connectivity/source/drivers/jdbc/JConnection \
    connectivity/source/drivers/jdbc/JDriver \
    connectivity/source/drivers/jdbc/JStatement \
    connectivity/source/drivers/jdbc/Object \
    connectivity/source/drivers/jdbc/PreparedStatement \
    connectivity/source/drivers/jdbc/Reader \
    connectivity/source/drivers/jdbc/Ref \
    connectivity/source/drivers/jdbc/ResultSet \
    connectivity/source/drivers/jdbc/ResultSetMetaData \
    connectivity/source/drivers/jdbc/SQLException \
    connectivity/source/drivers/jdbc/SQLWarning \
    connectivity/source/drivers/jdbc/String \
    connectivity/source/drivers/jdbc/Throwable \
    connectivity/source/drivers/jdbc/Timestamp \
    connectivity/source/drivers/jdbc/jservices \
    connectivity/source/drivers/jdbc/JBigDecimal \
    connectivity/source/drivers/jdbc/tools \
    connectivity/source/drivers/jdbc/ContextClassLoader \
))

# vim: set noet sw=4 ts=4:
