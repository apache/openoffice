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



$(eval $(call gb_Library_Library,dbase))

$(eval $(call gb_Library_add_precompiled_header,dbase,$(SRCDIR)/connectivity/inc/pch/precompiled_dbase))

$(eval $(call gb_Library_set_componentfile,dbase,connectivity/source/drivers/dbase/dbase))

$(eval $(call gb_Library_set_include,dbase,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,dbase,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_linked_libs,dbase,\
	comphelper \
	cppuhelper \
	cppu \
	file \
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


$(eval $(call gb_Library_add_exception_objects,dbase,\
    connectivity/source/drivers/dbase/DCode \
    connectivity/source/drivers/dbase/DResultSet \
    connectivity/source/drivers/dbase/DStatement \
    connectivity/source/drivers/dbase/DPreparedStatement \
    connectivity/source/drivers/dbase/dindexnode \
    connectivity/source/drivers/dbase/DIndexIter \
    connectivity/source/drivers/dbase/DDatabaseMetaData \
    connectivity/source/drivers/dbase/DCatalog \
    connectivity/source/drivers/dbase/DColumns \
    connectivity/source/drivers/dbase/DIndexColumns \
    connectivity/source/drivers/dbase/DIndex \
    connectivity/source/drivers/dbase/DIndexes \
    connectivity/source/drivers/dbase/DTables \
    connectivity/source/drivers/dbase/DConnection \
    connectivity/source/drivers/dbase/Dservices \
    connectivity/source/drivers/dbase/DDriver \
))

# [kh] ppc linux gcc compiler problem
# [Don Lewis] clang 3.4 -O2 on amd64 tries to use R_X86_64_PC32 relocation with
#             symbol _ZThn192_N12connectivity4file10OFileTable7acquireEvj

# FIXME: no way to translate $(CCNUMVER)>=000300040000 && $(CCNUMVER)<000300050000)
ifeq ($(OS)$(COM)$(CPUNAME),LINUXGCCPOWERPC)
$(eval $(call gb_Library_add_cxxobjects,dbase,\
    connectivity/source/drivers/dbase/DTable \
    , $(gb_COMPILERNOOPTFLAGS) $(gb_LinkTarget_EXCEPTIONFLAGS) \
))
else
ifeq ($(OS)$(COM)$(CPUNAME),FREEBSDCLANGX86_64)
ifeq ("$(CCNUMVER)","000300040000")
$(eval $(call gb_Library_add_cxxobjects,dbase,\
    connectivity/source/drivers/dbase/DTable \
    , $(gb_COMPILERNOOPTFLAGS) $(gb_LinkTarget_EXCEPTIONFLAGS) \
))
else
$(eval $(call gb_Library_add_exception_objects,dbase,\
    connectivity/source/drivers/dbase/DTable \
))
endif
else
$(eval $(call gb_Library_add_exception_objects,dbase,\
    connectivity/source/drivers/dbase/DTable \
))
endif
endif

# vim: set noet sw=4 ts=4:
