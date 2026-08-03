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



$(eval $(call gb_Package_Package,chart2_uiconfig,$(SRCDIR)/chart2/uiconfig))

$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/menubar/menubar.xml,menubar/menubar.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/basicshapes.xml,toolbar/basicshapes.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/symbolshapes.xml,toolbar/symbolshapes.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/standardbar.xml,toolbar/standardbar.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/toolbar.xml,toolbar/toolbar.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/calloutshapes.xml,toolbar/calloutshapes.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/arrowshapes.xml,toolbar/arrowshapes.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/flowchartshapes.xml,toolbar/flowchartshapes.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/drawbar.xml,toolbar/drawbar.xml))
$(eval $(call gb_Package_add_file,chart2_uiconfig,xml/uiconfig/modules/schart/toolbar/starshapes.xml,toolbar/starshapes.xml))
