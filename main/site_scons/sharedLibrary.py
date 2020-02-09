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

class AOOSharedLibrary:
    def __init__(self, target, group, sharedObjects):
        self.env = DefaultEnvironment().Clone()
        sharedObjects.env.Append(CPPDEFINES=platform.getLibraryDefs(soenv))
        self.sharedLib = self.env.SharedLibrary(
            target,
            source = sharedObjects.objects
        )
        self.env['AOO_THIS'] = self.sharedLib[0]
        self.env.Append(LINKFLAGS=platform.getLibraryLDFlags(soenv, group, OUTDIRLOCATION, DEBUGGING, DEBUGLEVEL))
        self.env.Append(LIBPATH=platform.getLDPATH(soenv))
        self.env['AOO_GROUP'] = group
        self.env['AOO_LAYER'] = platform.getLibraryGroupLayer(group)

    def AddLinkedLibs(self, libs):
        self.env.Append(LIBS=libs)

    def SetComponentFile(self, path):
        componentFile = File(path + '.component')
        inbuildComponentFile = File(path + '.inbuild.component')

        outdirComponentDir = Dir(OUTDIR + '/xml/component/' + componentFile.srcnode().dir.path)

        xsltenv = self.env.Clone()
        xsltenv.Append(ENV = platform.getExecutableEnvironment(soenv))
        if soenv['SYSTEM_LIBXSLT'] == 'YES':
            xsltenv['AOO_XSLTPROC'] = 'xsltproc'
        else:
            xsltenv['AOO_XSLTPROC'] = '${OUTDIR}/bin/xsltproc'
        xsltenv['AOO_COMPONENTPREFIX'] = platform.getLibraryLayerComponentPrefix(self.env['AOO_LAYER'])
        xsltenv['AOO_INBUILDCOMPONENTPREFIX'] = platform.getLibraryLayerComponentPrefix('NONE')

        finalComponent = xsltenv.Command(componentFile, componentFile.srcnode(),
            '${AOO_XSLTPROC} --nonet --stringparam uri "${AOO_COMPONENTPREFIX}${AOO_THIS.name}"' \
            ' -o $TARGET ${SOLARENV}/bin/createcomponent.xslt $SOURCE'
        )
        inbuildComponent = xsltenv.Command(inbuildComponentFile, componentFile.srcnode(),
            '${AOO_XSLTPROC} --nonet --stringparam uri "${AOO_INBUILDCOMPONENTPREFIX}${AOO_THIS.name}"' \
            ' -o $TARGET ${SOLARENV}/bin/createcomponent.xslt $SOURCE'
        )

        self.env.Install(outdirComponentDir, finalComponent)
        self.env.Install(outdirComponentDir, inbuildComponent)

    def SetVersionScript(self, path):
        versionScript = File(path)
        self.env.Append(LINKFLAGS=platform.getVersionScriptFlags(versionScript))

    def InstallTo(self, path):
        self.env.Install(path, self.sharedLib)
