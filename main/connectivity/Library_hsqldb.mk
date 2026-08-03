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



$(eval $(call gb_Library_Library,hsqldb))

$(eval $(call gb_Library_add_precompiled_header,hsqldb,$(SRCDIR)/connectivity/inc/pch/precompiled_hsqldb))

$(eval $(call gb_Library_set_componentfile,hsqldb,connectivity/source/drivers/hsqldb/hsqldb))

$(eval $(call gb_Library_set_include,hsqldb,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

# FIXME: Macs will presumably also need to do something with:
# SHL1CREATEJNILIB=TRUE

ifeq ($(SYSTEM_HSQLDB),YES)
$(eval $(call gb_Library_add_cxxflags,hsqldb,\
	-DSYSTEM_HSQLDB -DHSQLDB_JAR=\""file://$(HSQLDB_JAR)"\"
))
endif

$(eval $(call gb_Library_add_api,hsqldb,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,hsqldb,\
	comphelper \
	cppuhelper \
	cppu \
	dbtools \
	jvmfwk \
	sal \
	stl \
	tl \
	utl \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,hsqldb,\
    connectivity/source/drivers/hsqldb/HStorageMap \
    connectivity/source/drivers/hsqldb/HStorageAccess \
    connectivity/source/drivers/hsqldb/HDriver \
    connectivity/source/drivers/hsqldb/HConnection \
    connectivity/source/drivers/hsqldb/HTerminateListener \
    connectivity/source/drivers/hsqldb/StorageNativeOutputStream \
    connectivity/source/drivers/hsqldb/StorageNativeInputStream \
    connectivity/source/drivers/hsqldb/StorageFileAccess \
    connectivity/source/drivers/hsqldb/HTables \
    connectivity/source/drivers/hsqldb/HTable \
    connectivity/source/drivers/hsqldb/HView \
    connectivity/source/drivers/hsqldb/HViews \
    connectivity/source/drivers/hsqldb/HCatalog \
    connectivity/source/drivers/hsqldb/HColumns \
    connectivity/source/drivers/hsqldb/HUser \
    connectivity/source/drivers/hsqldb/HUsers \
    connectivity/source/drivers/hsqldb/Hservices \
    connectivity/source/drivers/hsqldb/HTools \
    connectivity/source/drivers/hsqldb/accesslog \
))

# vim: set noet sw=4 ts=4:
