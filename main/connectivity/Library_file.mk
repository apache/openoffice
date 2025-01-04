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



$(eval $(call gb_Library_Library,file))

$(eval $(call gb_Library_add_precompiled_header,file,$(SRCDIR)/connectivity/inc/pch/precompiled_file))

$(eval $(call gb_Library_set_include,file,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,file,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_defs,file,\
        -DOOO_DLLIMPLEMENTATION_FILE \
))

$(eval $(call gb_Library_add_linked_libs,file,\
	comphelper \
	cppuhelper \
	cppu \
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


$(eval $(call gb_Library_add_exception_objects,file,\
    connectivity/source/drivers/file/FNoException \
    connectivity/source/drivers/file/FConnection \
    connectivity/source/drivers/file/FResultSetMetaData \
    connectivity/source/drivers/file/FColumns \
    connectivity/source/drivers/file/FTables \
    connectivity/source/drivers/file/FTable \
    connectivity/source/drivers/file/FCatalog \
    connectivity/source/drivers/file/FResultSet \
    connectivity/source/drivers/file/FStatement \
    connectivity/source/drivers/file/FPreparedStatement \
    connectivity/source/drivers/file/FDatabaseMetaData \
    connectivity/source/drivers/file/FDriver \
    connectivity/source/drivers/file/fanalyzer \
    connectivity/source/drivers/file/fcomp \
    connectivity/source/drivers/file/fcode \
))

$(eval $(call gb_Library_add_noexception_objects,file,\
    connectivity/source/drivers/file/FStringFunctions \
    connectivity/source/drivers/file/FDateFunctions \
    connectivity/source/drivers/file/FNumericFunctions \
    connectivity/source/drivers/file/quotedstring \
))

# vim: set noet sw=4 ts=4:
