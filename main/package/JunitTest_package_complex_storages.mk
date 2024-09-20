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



$(eval $(call gb_JunitTest_JunitTest,package_complex_storages,SRCDIR))

$(eval $(call gb_JunitTest_set_defs,package_complex_storages,\
	$$(DEFS) \
))

$(eval $(call gb_JunitTest_add_jars,package_complex_storages,\
	$(OUTDIR)/bin/OOoRunner.jar \
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/test.jar \
	$(OUTDIR)/bin/unoil.jar \
	$(OUTDIR)/bin/jurt.jar \
	$(OUTDIR)/bin/juh.jar \
))

$(eval $(call gb_JunitTest_add_sourcefiles,package_complex_storages,\
	package/qa/storages/BorderedStream \
	package/qa/storages/RegressionTest_114358 \
	package/qa/storages/RegressionTest_125919 \
	package/qa/storages/RegressionTest_i26398 \
	package/qa/storages/RegressionTest_i27773 \
	package/qa/storages/RegressionTest_i29169 \
	package/qa/storages/RegressionTest_i29321 \
	package/qa/storages/RegressionTest_i30400 \
	package/qa/storages/RegressionTest_i30677 \
	package/qa/storages/RegressionTest_i35095 \
	package/qa/storages/RegressionTest_i46848 \
	package/qa/storages/RegressionTest_i49755 \
	package/qa/storages/RegressionTest_i55821 \
	package/qa/storages/RegressionTest_i59886 \
	package/qa/storages/RegressionTest_i61909 \
	package/qa/storages/RegressionTest_i84234 \
	package/qa/storages/StorageTest \
	package/qa/storages/StorageUnitTest \
	package/qa/storages/Test01 \
	package/qa/storages/Test02 \
	package/qa/storages/Test03 \
	package/qa/storages/Test04 \
	package/qa/storages/Test05 \
	package/qa/storages/Test06 \
	package/qa/storages/Test07 \
	package/qa/storages/Test08 \
	package/qa/storages/Test09 \
	package/qa/storages/Test10 \
	package/qa/storages/Test11 \
	package/qa/storages/Test12 \
	package/qa/storages/Test13 \
	package/qa/storages/Test14 \
	package/qa/storages/Test15 \
	package/qa/storages/Test16 \
	package/qa/storages/Test17 \
	package/qa/storages/Test18 \
	package/qa/storages/TestHelper \
))

$(eval $(call gb_JunitTest_add_classes,package_complex_storages,\
	complex.storages.StorageUnitTest \
))

# vim: set noet sw=4 ts=4:
