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
import SCons.Tool.CScanner

class AllLangResTarget:
    def __init__(self, name):
        self.env = DefaultEnvironment().Clone()

        withLang = env.get('AOO_WITH_LANG')
        if withLang == None or withLang == '':
            langs = set()
        else:
            langs = set(env['AOO_WITH_LANG'].split())
        if 'en-US' not in langs:
            langs.add('en-US')

        self.resTargets = []
        for lang in langs:
            self.resTargets.add(ResTarget(name, lang))

# FIXME: necessary?
#    def AddSrs(self, srsPath):
        

class ResTarget:
    def __init__(self, library, lang):
        self.library = library
        self.lang = lang

class SrsTarget:
    def __init__(self, srsPath):
        self.env = DefaultEnvironment().Clone()

    def SetInclude(self, includes):
        self.env.Replace(CPPPATH=includes)

    def AddFiles(self, files):
        

class SrsPartTarget:
    def __init__(self):

class SrsPartMergeTarget:
    def __init__(self):
