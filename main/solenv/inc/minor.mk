# *************************************************************
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
# *************************************************************
RSCVERSION=510
# The build id ends up in the third field of every file's version in the
# Windows installer (see get_fileversion() in
# solenv/bin/modules/installer/windows/file.pm).  Windows Installer limits
# each version field to 65535, and a package that exceeds it fails at
# InstallValidate with "Internal Error 2725 -- Invalid database tables",
# so BUILD must stay <= 65535.
BUILD=9900
LAST_MINOR=m1
# Derived, so the build id is written in exactly one place.
RSCREVISION=$(RSCVERSION)$(LAST_MINOR)(Build:$(BUILD))
SOURCEVERSION=AOO510
