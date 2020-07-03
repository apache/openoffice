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

class AOOJunitTest:
    def __init__(self, name, javaFiles):
        self.env = DefaultEnvironment().Clone()

        if soenv.get('OOO_JUNIT_JAR'):
            classOutputDir = Dir('JunitTest/' + name + '/classes')
            userDir = Dir('JunitTest/' + name + '/user')
            logFile = File('JunitTest/' + name + '/log.txt')

            self.env['AOO_JAVA'] = soenv['JAVAINTERPRETER']
            self.env['JAVAC'] = soenv['JAVACOMPILER']
            if DEBUGGING:
                self.env.Append(JAVACFLAGS='-g')

            classPath = [
                soenv['XCLASSPATH'],
                classOutputDir.abspath,
                soenv['OOO_JUNIT_JAR']
            ]
            hamcrestJar = soenv.get('HAMCREST_CORE_JAR')
            if hamcrestJar != None and hamcrestJar != '':
                classPath.append(hamcrestJar)
            classPath.append('${OUTDIR}/lib')
            self.env.Replace(JAVACLASSPATH = classPath)

            aooPath = os.environ.get('OOO_TEST_SOFFICE')
            if (aooPath == None or aooPath == ''):
                aooPath = 'path:${SRCDIR}/instsetoo_native/${INPATH}/Apache_OpenOffice/installed/install/en-US/openoffice4/program/soffice'
            defs = [
                '-Dorg.openoffice.test.arg.soffice=' + aooPath,
                '-Dorg.openoffice.test.arg.env=LD_LIBRARY_PATH',
                '-Dorg.openoffice.test.arg.user=file://' + userDir.abspath
            ]
            self.env['AOO_DEFS'] = defs
            self.javaTarget = self.env.Java(classOutputDir, javaFiles)
            
            self.env['AOO_TEST_CLASSES'] = []
            self.env['AOO_JUNIT_CLASSPATH'] = soenv['PATH_SEPERATOR'].join([str(x) for x in classPath])
            self.env.Append(ENV=os.environ)
            
            buildTargets = [str(t) for t in BUILD_TARGETS]
            if 'subsequentcheck' in buildTargets or os.environ.get('OOO_SUBSEQUENT_TESTS'):
                self.junitTarget = self.env.Command(logFile, self.javaTarget, [
                    Delete(userDir),
                    Mkdir(userDir),
                    self._run_junit,
                    Delete(userDir)
                ])
                Alias('subsequentcheck', self.junitTarget)
                AlwaysBuild(self.junitTarget)
        

    @staticmethod
    def _run_junit(target, source, env):
        logfilename = str(target[0])
        with open(logfilename, 'w') as logfile:
            completedProcess = subprocess.run(
                args=env.subst('${AOO_JAVA} -cp ${AOO_JUNIT_CLASSPATH} ${AOO_DEFS} org.junit.runner.JUnitCore ${AOO_TEST_CLASSES}'),
                shell=True,
                stdout=logfile,
                stderr=subprocess.STDOUT)
        if completedProcess.returncode != 0:
            with open(logfilename, 'r') as logfile:
                print(logfile.read())
        return completedProcess.returncode

    def AddJars(self, jars):
        self.env.Append(JAVACLASSPATH=jars)
        self.env['AOO_JUNIT_CLASSPATH'] = soenv['PATH_SEPERATOR'].join([str(x) for x in self.env['JAVACLASSPATH']])

    def AddTestClasses(self, testClasses):
        self.env['AOO_TEST_CLASSES'] += testClasses

    def SetDefs(self, defs):
        self.env['AOO_DEFS'] = defs
