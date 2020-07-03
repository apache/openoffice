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
import sys
import os
from config import soenv
from globals import *


###########################
# Envirionment
###########################

env = DefaultEnvironment(
    CC = soenv['CC'],
    CXX = soenv['CXX'],
    CPPPATH = platform.getIncludeStl(soenv) + platform.getInclude(soenv)
)
env.Append(CPPDEFINES = GLOBALDEFS)
env.Append(CFLAGS = platform.getCFlags(soenv))
env.Append(CXXFLAGS = platform.getCFlags(soenv))
env.Replace(CXXFILESUFFIX = '.cxx')

## Add some common paths to the SCons environment, without AOO_ prefix

env['SOLARSRC'] = soenv['SOLARSRC']
env['SRCDIR'] = soenv['SOLARSRC']
env['OUTDIR'] = soenv['OUTDIR']
env['WORKDIR'] = soenv['WORKDIR']
env['SOLARENV'] = soenv['SOLARENV']
env['LOCDIR'] = Dir(soenv['SOLARSRC']).Dir('..').abspath + '/extras'
env['INPATH'] = soenv['INPATH']

## Useful constants

if soenv.get('WITH_LANG') == 'ALL':
    env['AOO_WITH_LANG'] = loadJavaProperties(SOLENV + '/inc/langlist.mk')['completelangiso']
else:
    env['AOO_WITH_LANG'] = soenv.get('WITH_LANG')

###########################
# API
###########################

from sharedLibrary import AOOSharedLibrary
from sharedObjects import AOOSharedObjects
from AllLangRes import *
from JunitTest import *
from GoogleTest import *

def CreateSharedLibraryEnvironment(name, group):
    env = Environment()
    env.Replace(CC=soenv['CC'])
    env.Replace(CXX=soenv['CXX'])

    env.Append(CPPPATH=platform.getInclude(soenv))

    defines = GLOBALDEFS
    defines += platform.getLibraryDefs(soenv)
    env.Append(CPPDEFINES=defines)

    cflags = platform.getCFlags(soenv)
    cxxflags = platform.getCXXFlags(soenv)
    if DEBUGGING:
        cflags += platform.getDebugCFlags(soenv['COM'], soenv['ENABLE_SYMBOLS'])
        cxxflags += platform.getDebugCFlags(soenv['COM'], soenv['ENABLE_SYMBOLS'])
    env.Append(CFLAGS=cflags)
    print("APPENDING " + str(cxxflags))
    env.Replace(CXXFLAGS=cxxflags)
    print("CXXFLAGS is " + str(env['CXXFLAGS']))

    env.Append(LINKFLAGS=platform.getLibraryLDFlags(soenv, group, OUTDIRLOCATION, DEBUGGING, DEBUGLEVEL))
    env.Append(LIBPATH=platform.getLDPATH(soenv))

    env['SOLARENV'] = soenv['SOLARENV']
    env['AOO_GROUP'] = group
    env['AOO_LAYER'] = platform.getLibraryGroupLayer(group)
    env['AOO_COMPONENTPREFIX'] = platform.getLibraryLayerComponentPrefix(env['AOO_LAYER'])

    env.AddMethod(AddAPI)
    env.AddMethod(AddExceptionSharedObjects)
    env.AddMethod(AddNoExceptionSharedObjects)
    env.AddMethod(AddCxxSharedObjects)
    env.AddMethod(SetComponentFile)

    return env

def AddAPI(env, names):
    for name in names:
        env.Append(CPPPATH=OUTDIR + '/inc/' + name)

def AddExceptionSharedObjects(env, names):
    return AddCxxSharedObjects(env, names,
        platform.getExceptionFlags(soenv, True)
        + platform.getCompilerOptFlags(DEBUGLEVEL < 2, DEBUGLEVEL))

def AddNoExceptionSharedObjects(env, names):
    return AddCxxSharedObjects(env, names,
        platform.getExceptionFlags(soenv, False)
        + platform.getCompilerOptFlags(DEBUGLEVEL < 2, DEBUGLEVEL))

def AddCxxSharedObjects(env, names, flags):
    objects = []
    for name in names:
        objects.append(
            env.SharedObject(
                name,
                CXXFLAGS = env['CXXFLAGS'] + flags
            )
        )
    return objects

if soenv['SYSTEM_LIBXSLT'] == 'YES':
    _XSLTPROC = 'xsltproc'
else:
    _XSLTPROC = OUTDIR + '/bin/xsltproc'

def SetComponentFile(self, library, componentFileBaseName):
    env = self

    componentFile = File(componentFileBaseName + '.component')
    inbuildComponentFile = File(componentFileBaseName + '.inbuild.component')

    outdirComponentDir = Dir(OUTDIR + '/xml/component/' + componentFile.srcnode().dir.path)

    xsltenv = env.Clone()
    xsltenv.Append(ENV = platform.getExecutableEnvironment(soenv))
    finalComponent = xsltenv.Command(componentFile, componentFile.srcnode(),
        action = [
            _XSLTPROC + ' --nonet --stringparam uri "' + env['AOO_COMPONENTPREFIX'] + str(library[0]) + '"'
            + ' -o $TARGET ' + env['SOLARENV'] + '/bin/createcomponent.xslt $SOURCE'
        ]
    )
    inbuildComponent = xsltenv.Command(inbuildComponentFile, componentFile.srcnode(),
        action = [
            _XSLTPROC + ' --nonet --stringparam uri "' + platform.getLibraryLayerComponentPrefix('NONE') + str(library[0]) + '"'
            + ' -o $TARGET ' + env['SOLARENV'] + '/bin/createcomponent.xslt $SOURCE'
        ]
    )

    installs = []
    installs += env.Install(outdirComponentDir, finalComponent)
    installs += env.Install(outdirComponentDir, inbuildComponent)
    return installs
