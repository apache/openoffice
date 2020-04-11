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



$(eval $(call gb_Jar_Jar,juh,SRCDIR))

$(eval $(call gb_Jar_add_jars,juh,\
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/jurt.jar \
	$(OUTDIR)/bin/unoil.jar \
))

$(eval $(call gb_Jar_set_packageroot,juh,com))

$(eval $(call gb_Jar_set_manifest,juh,$(SRCDIR)/javaunohelper/util/manifest))

$(eval $(call gb_Jar_set_jarclasspath,juh,\
	ridl.jar \
	jurt.jar \
	../../lib/ \
	../bin/ \
))

$(eval $(call gb_Jar_add_sourcefiles,juh,\
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/UnoUrl \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/Factory \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/ComponentBase \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/WeakAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/MultiTypeInterfaceContainer \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/WeakBase \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/PropertySetMixin \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/PropertySet \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/helper/InterfaceContainer \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/adapter/ByteArrayToXInputStreamAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/adapter/XOutputStreamToByteArrayAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/adapter/XOutputStreamToOutputStreamAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/adapter/InputStreamToXInputStreamAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/adapter/XInputStreamToInputStreamAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/lib/uno/adapter/OutputStreamToXOutputStreamAdapter \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/JavaUNOHelperServices \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/juhtest/SmoketestCommandEnvironment \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/RegistryServiceFactory \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/UnoInfo \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/ComponentContext \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/BootstrapException \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/SharedLibraryLoader \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/Bootstrap \
	javaunohelper/java/juh/src/main/java/com/sun/star/comp/helper/ComponentContextEntry \
))

$(eval $(call gb_Jar_set_componentfile,juh,javaunohelper/util/juh,URE))

# vim: set noet sw=4 ts=4:
