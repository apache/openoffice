#***************************************************************
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#***************************************************************

$(eval $(call gb_Configuration_Configuration,connectivity,SRCDIR))

$(call gb_Configuration_add_schemas,connectivity,connectivity/registry,\
	org/openoffice/Office/DataAccess/adabas.xcs \
	org/openoffice/Office/DataAccess/ado.xcs \
	org/openoffice/Office/DataAccess/calc.xcs \
	org/openoffice/Office/DataAccess/dbase.xcs \
	org/openoffice/Office/DataAccess/evoab2.xcs \
	org/openoffice/Office/DataAccess/flat.xcs \
	org/openoffice/Office/DataAccess/hsqldb.xcs \
	org/openoffice/Office/DataAccess/jdbc.xcs \
	org/openoffice/Office/DataAccess/macab.xcs \
	org/openoffice/Office/DataAccess/mysql.xcs \
	org/openoffice/Office/DataAccess/odbc.xcs \
	org/openoffice/Office/DataAccess/sdbc_jdbc.xcs \
	org/openoffice/Office/DataAccess/sdbc_postgresql.xcs \
)

$(call gb_Configuration_add_datas,connectivity,connectivity/registry,\
	org/openoffice/Office/DataAccess/adabas.xcu \
	org/openoffice/Office/DataAccess/ado.xcu \
	org/openoffice/Office/DataAccess/calc.xcu \
	org/openoffice/Office/DataAccess/dbase.xcu \
	org/openoffice/Office/DataAccess/evoab2.xcu \
	org/openoffice/Office/DataAccess/flat.xcu \
	org/openoffice/Office/DataAccess/hsqldb.xcu \
	org/openoffice/Office/DataAccess/jdbc.xcu \
	org/openoffice/Office/DataAccess/macab.xcu \
	org/openoffice/Office/DataAccess/mysql.xcu \
	org/openoffice/Office/DataAccess/odbc.xcu \
	org/openoffice/Office/DataAccess/sdbc_jdbc.xcu \
	org/openoffice/Office/DataAccess/sdbc_postgresql.xcu \
)

$(call gb_Configuration_add_localized_datas,connectivity,connectivity/registry,\
	org/openoffice/Office/DataAccess/adabas.xcu \
	org/openoffice/Office/DataAccess/ado.xcu \
	org/openoffice/Office/DataAccess/calc.xcu \
	org/openoffice/Office/DataAccess/dbase.xcu \
	org/openoffice/Office/DataAccess/evoab2.xcu \
	org/openoffice/Office/DataAccess/flat.xcu \
	org/openoffice/Office/DataAccess/hsqldb.xcu \
	org/openoffice/Office/DataAccess/jdbc.xcu \
	org/openoffice/Office/DataAccess/macab.xcu \
	org/openoffice/Office/DataAccess/mysql.xcu \
	org/openoffice/Office/DataAccess/odbc.xcu \
	org/openoffice/Office/DataAccess/sdbc_jdbc.xcu \
	org/openoffice/Office/DataAccess/sdbc_postgresql.xcu \
)
