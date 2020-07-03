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

class AOOGoogleTest:
    def __init__(self, name, objects):
        self.env = DefaultEnvironment().Clone()

        if soenv.get('ENABLE_UNIT_TESTS') == 'YES':
            outputDir = Dir('GoogleTest/' + name)
            logFile = File('log.xml', outputDir)

            self.exe = self.env.Program(
                File(name, outputDir),
                objects.objects
            )
            self.env.Append(LINKFLAGS=platform.getGoogleTestLDFlags(soenv, OUTDIRLOCATION, DEBUGGING, DEBUGLEVEL))
            self.env.Append(LIBPATH=platform.getLDPATH(soenv))
            self.env.Append(LIBS='gtest')
            
            self.env.Append(ENV=os.environ)
            self.env.Append(ENV=platform.getExecutableEnvironment(soenv))
            self.googleTestTarget = self.env.Command(logFile, self.exe, [
                self._run_GoogleTest
            ])
            AlwaysBuild(self.googleTestTarget)

    @staticmethod
    def _run_GoogleTest(target, source, env):
        completedProcess = subprocess.run(
            args=source[0].abspath + ' --gtest_output=xml:' + target[0].abspath,
            shell=True,
            env=env['ENV'])
        return completedProcess.returncode

    def AddLinkedLibs(self, libs):
        self.env.Append(LIBS=libs)
