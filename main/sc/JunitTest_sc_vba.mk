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



$(eval $(call gb_JunitTest_JunitTest,sc_vba,SRCDIR))

$(eval $(call gb_JunitTest_add_jars,sc_vba,\
	$(OUTDIR)/bin/OOoRunner.jar \
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/test.jar \
	$(OUTDIR)/bin/unoil.jar \
	$(OUTDIR)/bin/jurt.jar \
	$(OUTDIR)/bin/juh.jar \
))

$(eval $(call gb_JunitTest_add_sourcefiles,sc_vba,\
	sc/qa/vba/src/VbaCompatTest \
	sc/qa/vba/src/VbaFormatConditionTest \
	sc/qa/vba/src/VbaFileSearchTest \
	sc/qa/vba/src/VbaSaveAsTest \
	sc/qa/vba/src/VbaApplicationPropsTest \
	sc/qa/vba/src/VbaQueryTableTest \
	sc/qa/vba/src/VbaRangeTest \
	sc/qa/vba/src/VbaDictionaryTest \
	sc/qa/vba/src/VbaSapColumnOpsTest \
	sc/qa/vba/src/VbaSapFeatureGapsTest \
	sc/qa/vba/src/VbaSapRangeOpsTest \
	sc/qa/vba/src/VbaSapOutlineTest \
	sc/qa/vba/src/VbaSapStringTest \
))

$(eval $(call gb_JunitTest_add_classes,sc_vba,\
	org.openoffice.sc.qa.vba.VbaFormatConditionTest \
	org.openoffice.sc.qa.vba.VbaFileSearchTest \
	org.openoffice.sc.qa.vba.VbaSaveAsTest \
	org.openoffice.sc.qa.vba.VbaApplicationPropsTest \
	org.openoffice.sc.qa.vba.VbaQueryTableTest \
	org.openoffice.sc.qa.vba.VbaRangeTest \
	org.openoffice.sc.qa.vba.VbaDictionaryTest \
	org.openoffice.sc.qa.vba.VbaSapColumnOpsTest \
	org.openoffice.sc.qa.vba.VbaSapFeatureGapsTest \
	org.openoffice.sc.qa.vba.VbaSapRangeOpsTest \
	org.openoffice.sc.qa.vba.VbaSapOutlineTest \
	org.openoffice.sc.qa.vba.VbaSapStringTest \
))

# vim: set noet sw=4 ts=4:
