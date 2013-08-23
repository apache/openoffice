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

PRJ=..
PRJNAME=scp2
TARGET=helper

LANGUAGE_FILELIST=kde/module_kde.ulf \
quickstart/module_quickstart.ulf \
graphicfilter/module_graphicfilter.ulf \
onlineupdate/module_onlineupdate.ulf \
impress/folderitem_impress.ulf \
impress/registryitem_impress.ulf \
impress/module_impress.ulf \
impress/module_ogltrans.ulf \
sdkoo/module_sdkoo.ulf \
writer/folderitem_writer.ulf \
writer/registryitem_writer.ulf \
writer/module_writer.ulf \
math/module_math.ulf \
math/folderitem_math.ulf \
math/registryitem_math.ulf \
javafilter/module_javafilter.ulf \
javafilter/registryitem_javafilter.ulf \
winexplorerext/module_winexplorerext.ulf \
xsltfilter/module_xsltfilter.ulf \
draw/registryitem_draw.ulf \
draw/folderitem_draw.ulf \
draw/module_draw.ulf \
calc/folderitem_calc.ulf \
calc/registryitem_calc.ulf \
calc/module_calc.ulf \
ooo/registryitem_ooo.ulf \
ooo/module_systemint.ulf \
ooo/module_ooo.ulf \
ooo/module_java.ulf \
ooo/module_headless.ulf \
ooo/directory_ooo.ulf \
ooo/folderitem_ooo.ulf \
ooo/module_langpack.ulf \
gnome/module_gnome.ulf \
base/folderitem_base.ulf \
base/registryitem_base.ulf \
base/module_base.ulf \
activex/module_activex.ulf \
python/module_python_mailmerge.ulf \
python/module_python.ulf

.INCLUDE :	settings.mk

.INCLUDE :	target.mk
