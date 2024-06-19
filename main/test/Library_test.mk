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



$(eval $(call gb_Library_Library,test))

$(eval $(call gb_Library_add_package_headers,test,test_inc))

$(eval $(call gb_Library_add_precompiled_header,test,$(SRCDIR)/test/inc/pch/precompiled_test))

$(eval $(call gb_Library_set_include,test,\
	$$(INCLUDE) \
	-I$(SRCDIR)/test/inc \
	-I$(SRCDIR)/test/inc/pch \
	-I$(SRCDIR)/test/source/cpp \
	-I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_add_api,test, \
	offapi \
        udkapi \
))

$(eval $(call gb_Library_add_defs,test,\
	-DOOO_DLLIMPLEMENTATION_TEST \
))

$(eval $(call gb_Library_add_linked_libs,test,\
	cppu \
	cppuhelper \
	gtest \
	sal \
	stl \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,test,\
	test/source/cpp/getargument \
	test/source/cpp/gettestargument \
	test/source/cpp/officeconnection \
	test/source/cpp/toabsolutefileurl \
	test/source/cpp/uniquepipename \
))

# vim: set noet sw=4 ts=4:

