###############################################################
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
###############################################################



$(eval $(call gb_Jar_Jar,sdbc_hsqldb,SRCDIR))

ifeq ($(SYSTEM_HSQLDB),YES)
# FIXME: not used anywhere else in AOO! Test!
$(eval $(call gb_Jar_add_system_jars,sdbc_hsqldb,\
	$(HSQLDB_JAR) \
))
else
$(eval $(call gb_Jar_add_jars,sdbc_hsqldb,\
	$(OUTDIR)/bin/hsqldb.jar \
))
endif

$(eval $(call gb_Jar_set_packageroot,sdbc_hsqldb,com org))

$(eval $(call gb_Jar_set_manifest,sdbc_hsqldb,$(SRCDIR)/connectivity/com/sun/star/sdbcx/comp/hsqldb/manifest))

$(eval $(call gb_Jar_set_jarclasspath,sdbc_hsqldb,\
	hsqldb.jar \
	.. \
))

$(eval $(call gb_Jar_add_sourcefiles,sdbc_hsqldb,\
	connectivity/com/sun/star/sdbcx/comp/hsqldb/FileSystemRuntimeException \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/NativeInputStreamHelper \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/NativeLibraries \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/NativeOutputStreamHelper \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/NativeStorageAccess \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/StorageAccess \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/StorageFileAccess \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/StorageNativeInputStream \
	connectivity/com/sun/star/sdbcx/comp/hsqldb/StorageNativeOutputStream \
))

# vim: set noet sw=4 ts=4:
