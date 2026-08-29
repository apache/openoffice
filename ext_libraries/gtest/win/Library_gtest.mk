#/**************************************************************
# *
# * Licensed to the Apache Software Foundation (ASF) under one
# * or more contributor license agreements.  See the NOTICE file
# * distributed with this work for additional information
# * regarding copyright ownership.  The ASF licenses this file
# * to you under the Apache License, Version 2.0 (the
# * "License"); you may not use this file except in compliance
# * with the License.  You may obtain a copy of the License at
# *
# *   http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing,
# * software distributed under the License is distributed on an
# * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# * KIND, either express or implied.  See the License for the
# * specific language governing permissions and limitations
# * under the License.
# *
# *************************************************************/

gb_Cxx_extension=cc

$(eval $(call gb_Library_Library,gtest))

$(eval $(call gb_Library_add_package_headers,gtest,gtest_inc))

$(eval $(call gb_Library_set_include,gtest,\
	$$(INCLUDE) \
	-I. \
	-I./include \
))

$(eval $(call gb_Library_add_defs,gtest,\
	-DGTEST_CREATE_SHARED_LIBRARY=1 \
))

# Was the literal list "kernel32 msvcrt oldnames", which is exactly
# gb_STDLIBS without uwinapi -- so say that instead, and the UCRT's extra CRT
# pieces arrive here the same way they arrive everywhere else.  Without them
# the link fails on memcpy/memset and on the POSIX names oldnames.lib
# forwards (dup2, read, write), which moved into ucrt.lib and vcruntime.lib.
# On VC9 gb_STDLIBS is "uwinapi kernel32 msvcrt oldnames", so this expands to
# the identical list it had before.
$(eval $(call gb_Library_add_linked_libs,gtest,\
    $(filter-out uwinapi,$(gb_STDLIBS)) \
    $(gb_Library_STLLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,gtest, \
	gtest/$(INPATH)/misc/build/gtest-1.7.0/src/gtest-all \
))
