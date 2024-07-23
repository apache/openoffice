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



$(eval $(call gb_JunitTest_JunitTest,package_complex_ofopxmlstorages,SRCDIR))

$(eval $(call gb_JunitTest_set_defs,package_complex_ofopxmlstorages,\
	$$(DEFS) \
))

$(eval $(call gb_JunitTest_add_jars,package_complex_ofopxmlstorages,\
	$(OUTDIR)/bin/OOoRunner.jar \
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/test.jar \
	$(OUTDIR)/bin/unoil.jar \
	$(OUTDIR)/bin/jurt.jar \
	$(OUTDIR)/bin/juh.jar \
))

$(eval $(call gb_JunitTest_add_sourcefiles,package_complex_ofopxmlstorages,\
	package/qa/ofopxmlstorages/StorageTest \
	package/qa/ofopxmlstorages/StorageUnitTest \
	package/qa/ofopxmlstorages/Test01 \
	package/qa/ofopxmlstorages/Test02 \
	package/qa/ofopxmlstorages/Test03 \
	package/qa/ofopxmlstorages/Test04 \
	package/qa/ofopxmlstorages/Test05 \
	package/qa/ofopxmlstorages/Test06 \
	package/qa/ofopxmlstorages/Test07 \
	package/qa/ofopxmlstorages/Test08 \
	package/qa/ofopxmlstorages/TestHelper \
))

$(eval $(call gb_JunitTest_add_classes,package_complex_ofopxmlstorages,\
	complex.ofopxmlstorages.StorageUnitTest \
))

# vim: set noet sw=4 ts=4:
