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



$(eval $(call gb_Module_Module,connectivity))

$(eval $(call gb_Module_add_targets,connectivity,\
	AllLangResTarget_cnr \
	AllLangResTarget_hsqldb \
	AllLangResTarget_sdbcl \
	AllLangResTarget_sdberr \
	Ant_dbtools \
	Ant_sdbc_hsqldb \
	Ant_sdbc_jdbc \
	Ant_sdbc_postgresql \
	Configuration_connectivity \
	Library_adabas \
	Library_calc \
	Library_dbase \
	Library_dbpool2 \
	Library_dbtools \
	Library_file \
	Library_flat \
	Library_hsqldb \
	Library_mysql \
	Library_odbc \
	Library_odbcbase \
	Library_sdbc2 \
	Package_inc \
	Package_generated_inc \
))

ifeq ($(GUI),WNT)
$(eval $(call gb_Module_add_targets,connectivity,\
	Library_ado \
))
endif

ifeq ($(ENABLE_EVOAB2),TRUE)
$(eval $(call gb_Module_add_targets,connectivity,\
        Library_evoab2 \
))
endif

ifeq ($(GUI),UNX)
ifeq ($(OS),MACOSX)
$(eval $(call gb_Module_add_targets,connectivity,\
	Library_macab1 \
	Library_macabdrv1 \
))
endif
endif

# vim: set noet sw=4 ts=4:
