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
PRJNAME=desktop
TARGET=helper

LANGUAGE_FILELIST=migration/wizard.src \
app/desktop.src \
deployment/unopkg/unopkg.src \
deployment/misc/dp_misc.src \
deployment/registry/script/dp_script.src \
deployment/registry/dp_registry.src \
deployment/registry/configuration/dp_configuration.src \
deployment/registry/help/dp_help.src \
deployment/registry/sfwk/dp_sfwk.src \
deployment/registry/package/dp_package.src \
deployment/registry/component/dp_component.src \
deployment/manager/dp_manager.src \
deployment/gui/dp_gui_versionboxes.src \
deployment/gui/dp_gui_dependencydialog.src \
deployment/gui/dp_gui_dialog.src \
deployment/gui/dp_gui_updateinstalldialog.src \
deployment/gui/dp_gui_updatedialog.src \
deployment/gui/dp_gui_backend.src \
deployment/gui/dp_gui_dialog2.src \
../win32/source/rebase/rebasegui.ulf \
../win32/source/setup/setup.ulf

.INCLUDE :	settings.mk

.INCLUDE :	target.mk
