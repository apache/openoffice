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

class AOOAnt:
    def __init__(self, name, pathToBuildXml):
        self.env = DefaultEnvironment().Clone()
        self.env.Append(ENV=os.environ)
        self.env['AOO_ANT'] = soenv['ANT']

        self.buildXml = File(pathToBuildXml)
        
        outputDir = Dir('Ant/' + name)
        depFile = File('deps', outputDir)
        targetName = Dir(name + '.jar', outputDir)

        if self.env.GetOption('clean'):
            # We cannot register a custom clean target with scons,
            # so do Ant's cleaning in the preparation phase of the build.
            cleanProcess = subprocess.run(
                args = ' '.join([
                    soenv['ANT'],
                    '-f',
                    self.buildXml.srcnode().abspath,
                    'clean']),
                shell = True,
                env = self.env['ENV']
            )
            if cleanProcess.returncode != 0:
                raise Exception('ant clean failed with exit code ' + cleanProcess.returncode)
        else:
            # Ant might require dependencies from other modules,
            # which we need to know to establish build order.
            depsProcess = subprocess.run(
                args = ' '.join([
                    soenv['ANT'],
                    '-Ddependencies.outfile=' + depFile.abspath,
                    '-f',
                    self.buildXml.srcnode().abspath,
                    'dependencies'
                ]),
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
            # Ant will depend on its internally known .java etc.
            # so we must always run it
            AlwaysBuild(self.target)
