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



$(eval $(call gb_Package_Package,connectivity_inc,$(SRCDIR)/connectivity/inc))

$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/BlobHelper.hxx,connectivity/BlobHelper.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/CommonTools.hxx,connectivity/CommonTools.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/conncleanup.hxx,connectivity/conncleanup.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/ConnectionWrapper.hxx,connectivity/ConnectionWrapper.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/DateConversion.hxx,connectivity/DateConversion.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/dbcharset.hxx,connectivity/dbcharset.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/dbconversion.hxx,connectivity/dbconversion.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/dbexception.hxx,connectivity/dbexception.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/dbmetadata.hxx,connectivity/dbmetadata.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/dbtools.hxx,connectivity/dbtools.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/dbtoolsdllapi.hxx,connectivity/dbtoolsdllapi.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/DriversConfig.hxx,connectivity/DriversConfig.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/filtermanager.hxx,connectivity/filtermanager.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/formattedcolumnvalue.hxx,connectivity/formattedcolumnvalue.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/FValue.hxx,connectivity/FValue.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/IParseContext.hxx,connectivity/IParseContext.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/ParameterCont.hxx,connectivity/ParameterCont.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/parameters.hxx,connectivity/parameters.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/paramwrapper.hxx,connectivity/paramwrapper.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/PColumn.hxx,connectivity/PColumn.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/predicateinput.hxx,connectivity/predicateinput.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/IRefreshable.hxx,connectivity/sdbcx/IRefreshable.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VCatalog.hxx,connectivity/sdbcx/VCatalog.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VCollection.hxx,connectivity/sdbcx/VCollection.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VColumn.hxx,connectivity/sdbcx/VColumn.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VDescriptor.hxx,connectivity/sdbcx/VDescriptor.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VGroup.hxx,connectivity/sdbcx/VGroup.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VIndex.hxx,connectivity/sdbcx/VIndex.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VIndexColumn.hxx,connectivity/sdbcx/VIndexColumn.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VKey.hxx,connectivity/sdbcx/VKey.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VKeyColumn.hxx,connectivity/sdbcx/VKeyColumn.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VTable.hxx,connectivity/sdbcx/VTable.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VTypeDef.hxx,connectivity/sdbcx/VTypeDef.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VUser.hxx,connectivity/sdbcx/VUser.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sdbcx/VView.hxx,connectivity/sdbcx/VView.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sqlerror.hxx,connectivity/sqlerror.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sqliterator.hxx,connectivity/sqliterator.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sqlnode.hxx,connectivity/sqlnode.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/sqlparse.hxx,connectivity/sqlparse.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/SQLStatementHelper.hxx,connectivity/SQLStatementHelper.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/standardsqlstate.hxx,connectivity/standardsqlstate.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/statementcomposer.hxx,connectivity/statementcomposer.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/StdTypeDefs.hxx,connectivity/StdTypeDefs.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TColumnsHelper.hxx,connectivity/TColumnsHelper.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TIndex.hxx,connectivity/TIndex.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TIndexColumns.hxx,connectivity/TIndexColumns.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TIndexes.hxx,connectivity/TIndexes.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TKey.hxx,connectivity/TKey.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TKeyColumns.hxx,connectivity/TKeyColumns.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TKeys.hxx,connectivity/TKeys.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/TTableHelper.hxx,connectivity/TTableHelper.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/virtualdbtools.hxx,connectivity/virtualdbtools.hxx))
$(eval $(call gb_Package_add_file,connectivity_inc,inc/connectivity/warningscontainer.hxx,connectivity/warningscontainer.hxx))
