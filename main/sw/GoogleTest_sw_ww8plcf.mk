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


$(eval $(call gb_GoogleTest_GoogleTest,sw_ww8plcf))

$(eval $(call gb_GoogleTest_add_exception_objects,sw_ww8plcf, \
	sw/source/filter/ww8/qa/ww8plcf_test \
))

# WW8PLCF::IsValidLength is inline and references only compile-time constants,
# so the test compiles against the ww8 filter headers without linking msword.
$(eval $(call gb_GoogleTest_add_linked_libs,sw_ww8plcf, \
    sal \
    stl \
    sw \
    tl \
    $(gb_STDLIBS) \
))

$(eval $(call gb_GoogleTest_set_include,sw_ww8plcf,\
	$$(INCLUDE) \
	-I$(SRCDIR)/sw/inc \
	-I$(SRCDIR)/sw/inc/pch \
	-I$(SRCDIR)/sw/source/filter/inc \
	-I$(SRCDIR)/sw/source/filter/ww8 \
	-I$(OUTDIR)/inc/offuh \
	-I$(OUTDIR)/inc \
))

# vim: set noet sw=4 ts=4:
