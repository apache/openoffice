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

from SCons.Script import *
from config import soenv
from globals import *

class AOOStaticLibrary:
    def __init__(self, target, group, sharedObjects):
        self.env = DefaultEnvironment().Clone()
        self.staticLib = self.env.StaticLibrary(
            target,
            source = sharedObjects.objects
        )
        self.env['AOO_THIS'] = self.staticLib[0]
        self.env.Append(LINKFLAGS=platform.getStaticLibraryLDFlags(soenv, DEBUGGING, DEBUGLEVEL))
        self.env.Append(LIBPATH=platform.getLDPATH(soenv))
        self.env['AOO_GROUP'] = group

    def InstallTo(self, path):
        self.env.Install(path, self.staticLib)
