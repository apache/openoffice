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
import os
import subprocess

javaComponentPrefixes = {}
javaComponentPrefixes['OOO'] = 'vnd.sun.star.expand:\\$$OOO_BASE_DIR/program/classes/'
javaComponentPrefixes['URE'] = 'vnd.sun.star.expand:\\$$URE_INTERNAL_JAVA_DIR/'
javaComponentPrefixes['INTERN'] = 'vnd.sun.star.expand:\\$$OOO_INBUILD_JAVA_DIR/'

class AOOAnt:
    def __init__(self, name, pathToBuildXml):
        self.env = DefaultEnvironment().Clone()
        self.env.Append(ENV=os.environ)
        self.env['AOO_ANT'] = soenv['ANT']

        outputDir = self.env.Dir('${WORKDIR}/Ant/' + name)
        self.jarName = name + '.jar'
        targetName = self.env.File('${WORKDIR}/Ant/' + name + '.jar')
        outdirTargetName = self.env.File('${OUTDIR}/bin/' + name + '.jar')

        self.buildXml = File(pathToBuildXml)
        self.env['AOO_BUILDXML'] = self.buildXml.srcnode().abspath

        depFile = File('deps', outputDir)
        self.env['AOO_DEPFILE'] = depFile.abspath
        
        if self.env.GetOption('clean'):
            # We cannot register a custom clean target with scons,
            # so do Ant's cleaning in the preparation phase of the build.
            self.env.Execute('${AOO_ANT} -f ${AOO_BUILDXML} clean')
            Execute(Delete(outdirTargetName.abspath))
        else:
            # Ant might require dependencies from other modules,
            # which we need to know to establish build order.
            depsProcess = subprocess.run(
                args = self.env.subst('${AOO_ANT} -Ddependencies.outfile=${AOO_DEPFILE} -f ${AOO_BUILDXML} dependencies'),
                shell = True,
                env = self.env['ENV']
            )
            if depsProcess.returncode == 0:
                with open(depFile.abspath, 'rt') as f:
                    for dep in f.readline().split():
                        Depends(targetName, depFile)
            else:
                raise Exception('ant failed with exit code ' + depsProcess.returncode)

            self.target = self.env.Command(targetName, self.buildXml.srcnode(),
                '${AOO_ANT} -f ${SOURCE}')
            # The JAR will in turn depend on its internally known .java etc.
            # so we must always run Ant
            AlwaysBuild(self.target)
            self.env.Command(outdirTargetName, self.target,
                Copy('${TARGET}', '${SOURCE}'))

    def SetComponentFile(self, path, layer):
        componentFile = File(path + '.component')

        outdirComponentDir = Dir(OUTDIR + '/xml/component/' + componentFile.srcnode().dir.path)

        xsltenv = self.env.Clone()
        xsltenv.Append(ENV = platform.getExecutableEnvironment(soenv))
        if soenv['SYSTEM_LIBXSLT'] == 'YES':
            xsltenv['AOO_XSLTPROC'] = 'xsltproc'
        else:
            xsltenv['AOO_XSLTPROC'] = '${OUTDIR}/bin/xsltproc'

        componentPrefix = javaComponentPrefixes.get(layer)
        if componentPrefix == None:
            raise Exception('Invalid layer ' + layer + ' for component ' + path)
        xsltenv['AOO_COMPONENTPREFIX'] = componentPrefix
        xsltenv['AOO_JARNAME'] = self.jarName
        finalComponent = xsltenv.Command(componentFile, componentFile.srcnode(),
            '${AOO_XSLTPROC} --nonet --stringparam uri "${AOO_COMPONENTPREFIX}${AOO_JARNAME}"' \
            ' -o $TARGET ${SOLARENV}/bin/createcomponent.xslt $SOURCE'
        )

        self.env.Install(outdirComponentDir, finalComponent)

