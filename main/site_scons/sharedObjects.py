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

class AOOSharedObjects:
    def __init__(self):
        self.env = DefaultEnvironment().Clone()
        self.env.Append(CPPDEFINES=platform.getLibraryDefs(soenv))
        self.env.Replace(CXXFILESUFFIX='.cxx')
        self.env.Replace(YACCHXXFILESUFFIX='.hxx')
        self.env.Append(YACCFLAGS='-d')
        if DEBUGGING:
            self.env.Append(CFLAGS=platform.getDebugCFlags(soenv['COM'], soenv.get('ENABLE_SYMBOLS')))
            self.env.Append(CXXFLAGS=platform.getDebugCFlags(soenv['COM'], soenv.get('ENABLE_SYMBOLS')))
        self.objects = []

    def AddDefs(self, defs):
        self.env.Append(CPPDEFINES=defs)

    def AddInclude(self, includes):
        self.env.Append(CPPPATH=includes)

    def AddAPI(self, names):
        for name in names:
            self.env.Append(CPPPATH='${OUTDIR}/inc/' + name)

    def AddCSources(self, names):
        for name in names:
            self.objects.append(
                self.env.SharedObject(
                    name,
                    CFLAGS = self.env['CFLAGS']
                )
            )

    def AddCxxExceptionSources(self, names):
        self.AddCxxSources(names,
            platform.getExceptionFlags(soenv, True)
            + platform.getCompilerOptFlags(DEBUGLEVEL < 2, DEBUGLEVEL))

    def AddCxxNoExceptionSources(self, names):
        self.AddCxxSources(names,
            platform.getExceptionFlags(soenv, False)
            + platform.getCompilerOptFlags(DEBUGLEVEL < 2, DEBUGLEVEL))

    def AddCxxSources(self, names, flags):
        for name in names:
            self.objects.append(
                self.env.SharedObject(
                    name,
                    CXXFLAGS = self.env['CXXFLAGS'] + flags
               )
           )

    def AddBisonFiles(self, bisonFiles):
        # We #include this file in other files instead of compiling it
        for bisonFile in bisonFiles:
            self.env.CXXFile(File(bisonFile))

    def AddFlexFiles(self, flexFiles):
        # We #include this file in other files instead of compiling it
        for flexFile in flexFiles:
            self.env.CXXFile(File(flexFile))
