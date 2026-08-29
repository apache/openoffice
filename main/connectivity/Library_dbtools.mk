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



$(eval $(call gb_Library_Library,dbtools))

$(eval $(call gb_Library_add_package_headers,dbtools,connectivity_inc))

$(eval $(call gb_Library_add_precompiled_header,dbtools,$(SRCDIR)/connectivity/inc/pch/precompiled_dbtools))

$(eval $(call gb_Library_set_componentfile,dbtools,connectivity/source/dbtools/dbtools))

$(eval $(call gb_Library_set_include,dbtools,\
        $$(INCLUDE) \
	-I$(SRCDIR)/connectivity/inc \
	-I$(SRCDIR)/connectivity/inc/pch \
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(WORKDIR)/CustomTarget/connectivity/source/parse \
))

$(eval $(call gb_Library_add_api,dbtools,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_add_defs,dbtools,\
	-DOOO_DLLIMPLEMENTATION_DBTOOLS \
	-DBOOST_SPIRIT_USE_OLD_NAMESPACE \
	-DCONN_SHARED_RESOURCE_FILE=cnr \
))

$(eval $(call gb_Library_add_linked_libs,dbtools,\
	comphelper \
	cppuhelper \
	cppu \
	i18nisolang1 \
	jvmaccess \
	sal \
	stl \
	tl \
	utl \
	vos3 \
	$(gb_STDLIBS) \
))

# NETBSD: somewhere we have to instantiate the static data members.
# NETBSD-1.2.1 doesn't know about weak symbols so the default mechanism for GCC won't work.
# SCO and MACOSX: the linker does know about weak symbols, but we can't ignore multiple defined symbols
ifeq ($(OS),NETBSD)
$(eval $(call gb_Library_add_linked_libs,dbtools,ucbhelper))
endif
ifeq ($(OS),SCO)
$(eval $(call gb_Library_add_linked_libs,dbtools,ucbhelper))
endif
ifeq ($(OS)$(COM),OS2GCC)
$(eval $(call gb_Library_add_linked_libs,dbtools,ucbhelper))
endif
ifeq ($(OS),MACOSX)
$(eval $(call gb_Library_add_linked_libs,dbtools,ucbhelper))
endif


$(eval $(call gb_Library_add_exception_objects,dbtools,\
	connectivity/source/sdbcx/VDescriptor \
	connectivity/source/sdbcx/VCollection \
	connectivity/source/sdbcx/VColumn \
	connectivity/source/sdbcx/VIndexColumn \
	connectivity/source/sdbcx/VKeyColumn \
	connectivity/source/sdbcx/VUser \
	connectivity/source/sdbcx/VGroup \
	connectivity/source/sdbcx/VTable \
	connectivity/source/sdbcx/VKey \
	connectivity/source/sdbcx/VIndex \
	connectivity/source/sdbcx/VCatalog \
	connectivity/source/sdbcx/VView \
	connectivity/source/commontools/predicateinput \
	connectivity/source/commontools/ConnectionWrapper \
	connectivity/source/commontools/TConnection \
	connectivity/source/commontools/conncleanup \
	connectivity/source/commontools/dbtools \
	connectivity/source/commontools/dbtools2 \
	connectivity/source/commontools/dbexception \
	connectivity/source/commontools/CommonTools \
	connectivity/source/commontools/TColumnsHelper \
	connectivity/source/commontools/TTableHelper \
	connectivity/source/commontools/TKeys \
	connectivity/source/commontools/TKey \
	connectivity/source/commontools/TKeyColumns \
	connectivity/source/commontools/TIndexes \
	connectivity/source/commontools/TIndex \
	connectivity/source/commontools/TIndexColumns \
	connectivity/source/commontools/DateConversion \
	connectivity/source/commontools/FDatabaseMetaDataResultSetMetaData \
	connectivity/source/commontools/FDatabaseMetaDataResultSet \
	connectivity/source/commontools/TDatabaseMetaDataBase \
	connectivity/source/commontools/TPrivilegesResultSet \
	connectivity/source/commontools/TSkipDeletedSet \
	connectivity/source/commontools/dbmetadata \
	connectivity/source/commontools/TSortIndex \
	connectivity/source/commontools/dbcharset \
	connectivity/source/commontools/propertyids \
	connectivity/source/commontools/FValue \
	connectivity/source/commontools/paramwrapper \
	connectivity/source/commontools/statementcomposer \
	connectivity/source/commontools/RowFunctionParser \
	connectivity/source/commontools/sqlerror \
	connectivity/source/commontools/filtermanager \
	connectivity/source/commontools/parameters \
	connectivity/source/commontools/ParamterSubstitution \
	connectivity/source/commontools/DriversConfig \
	connectivity/source/commontools/formattedcolumnvalue \
	connectivity/source/commontools/BlobHelper \
	connectivity/source/commontools/warningscontainer \
	connectivity/source/parse/PColumn \
	connectivity/source/parse/sqliterator \
	connectivity/source/parse/sqlnode \
	connectivity/source/parse/wrap_sqlbison \
	connectivity/source/parse/internalnode \
	connectivity/source/simpledbt/charset_s \
	connectivity/source/simpledbt/staticdbtools_s \
	connectivity/source/simpledbt/parsenode_s \
	connectivity/source/simpledbt/parser_s \
	connectivity/source/resource/sharedresources \
))

$(eval $(call gb_Library_add_noexception_objects,dbtools,\
	connectivity/source/commontools/AutoRetrievingBase \
	connectivity/source/commontools/dbconversion \
	connectivity/source/parse/wrap_sqlflex \
	connectivity/source/simpledbt/refbase \
	connectivity/source/simpledbt/dbtfactory \
))

############
# BISON rule
############

# FIXME: no proper dependencies of other files on Yacc's outputs - needs "make clean" on any changes!

$(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.cxx : TRTEMPFILE := $(shell $(gb_MKTEMP))
$(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.cxx : $(SRCDIR)/connectivity/source/parse/sqlbison.y
	tr -d "\015" < $< > $(TRTEMPFILE)
	mkdir -p $(dir $@)
	bison -v -d -l -pSQLyy -bsql -o $@ $(TRTEMPFILE)
	rm $(TRTEMPFILE)

$(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.hxx : $(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.cxx

# Add dependencies. This is so ugly and doesn't cover the fact evoab2 also uses it:

$(SRCDIR)/connectivity/inc/connectivity/sqlparse.hxx : $(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.hxx

$(SRCDIR)/connectivity/source/parse/wrap_sqlbison.cxx : $(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.cxx

###########
# FLEX rule
###########

$(WORKDIR)/CustomTarget/connectivity/source/parse/sqlflex.cxx : $(SRCDIR)/connectivity/source/parse/sqlflex.l
	mkdir -p $(dir $@)
	flex -i -8 -PSQLyy -L -o$@ $<

$(SRCDIR)/connectivity/source/parse/wrap_sqlflex.cxx : $(WORKDIR)/CustomTarget/connectivity/source/parse/sqlflex.cxx $(WORKDIR)/CustomTarget/connectivity/source/parse/sqlbison.hxx

# vim: set noet sw=4 ts=4:
