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


ifeq ($(OS),WNT)
gb_Library_FILENAMES := $(patsubst apr:iapr%,apr:libapr-1%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst expat_xmltok:iexpat_xmltok%,expat_xmltok:expat_xmltok%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst expat_xmlparse:iexpat_xmlparse%,expat_xmlparse:expat_xmlparse%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst libeay32:ilibeay32%,libeay32:libeay32%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ssleay32:issleay32%,ssleay32:ssleay32%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst zlib:izlib%,zlib:zlib%,$(gb_Library_FILENAMES))
endif

#gb_Library_FILENAMES := $(patsubst apr-util:iapr-util%,apr:libapr-util-1%,$(gb_Library_FILENAMES))

# vim: set noet sw=4 ts=4:
