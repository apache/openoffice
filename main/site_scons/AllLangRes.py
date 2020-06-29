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

from SCons.Scanner import ClassicCPP
from SCons.Script import *
from config import soenv
from globals import *
import shutil
import os.path

ResTarget_DEFIMAGESLOCATION='${SOLARSRC}/default_images/'

class AOOAllLangResTarget:
    def __init__(self, name, srsTarget):
        self.env = DefaultEnvironment().Clone()

        withLang = self.env.get('AOO_WITH_LANG')
        if withLang == None or withLang == '':
            langs = set()
        else:
            langs = set(self.env['AOO_WITH_LANG'].split())
        if 'en-US' not in langs:
            langs.add('en-US')

        self.resTargets = []
        for lang in langs:
            self.resTargets.append(AOOResTarget(name, lang, srsTarget.target))

    def SetResLocation(self, resLocation):
        for resTarget in self.resTargets:
            resTarget.SetResLocation(resLocation)

    def SetImageLocations(self, imageLocations):
        for resTarget in self.resTargets:
            resTarget.SetImageLocations(imageLocations)


class AOOResTarget:
    def __init__(self, library, lang, srsTargets):
        self.env = DefaultEnvironment().Clone()
        self.env.Append(ENV=platform.getExecutableEnvironment(soenv))
        
        self.library = library
        self.env['AOO_LANGUAGE'] = lang

        self.env['AOO_DEFIMAGESLOCATION'] = ResTarget_DEFIMAGESLOCATION
        self.resLocation = library
        self.imageLocations = []
        
        self.SetImageLocations(self.imageLocations)

        submodulePath = ''
        if os.path.exists(Dir(self.env.subst('${AOO_DEFIMAGESLOCATION}') + self.resLocation).abspath):
            submodulePath = Dir(self.env.subst('${AOO_DEFIMAGESLOCATION}') + self.resLocation)
        self.env['AOO_SUBMODULE'] = submodulePath
        imageListFile = File('Res/ResTarget/' + library + lang + '.ilst')
        self.env['AOO_IMAGELISTDIR'] = imageListFile.Dir('.').abspath
        targetFile = File('Res/ResTarget/' + library + lang + '.res')
        self.env.SideEffect(imageListFile, targetFile)
        self.env.Command(targetFile, srsTargets, ' '.join([
            '${OUTDIR}/bin/rsc',
            '-r',
            '-p',
            '-lg${AOO_LANGUAGE}',
            '-fs=${TARGET}',
            '${AOO_IMAGEARGS}',
            '-lip=${AOO_DEFIMAGESLOCATION}res/${AOO_LANGUAGE}',
            '-lip=${AOO_DEFIMAGESLOCATION}res',
            '-subMODULE=${AOO_SUBMODULE}',
            '-subGLOBALRES=${AOO_DEFIMAGESLOCATION}res',
            '-oil=${AOO_IMAGELISTDIR}',
            '${SOURCE}']))
        self.env.Install('${OUTDIR}/bin', targetFile)
        self.env.Install('${OUTDIR}/res/img', imageListFile)

    def SetReslocation(self, resLocation):
        self.resLocation = resLocation
        self.SetImageLocations(self.imageLocations)

    def SetImageLocations(self, imageLocations):
        self.imageLocations = imageLocations
        imageList = []
        for location in [ self.resLocation ] + self.imageLocations:
            self._addLocationIfExists(imageList, '${AOO_DEFIMAGESLOCATION}' + location + '/' + self.library)
            self._addLocationIfExists(imageList, '${AOO_DEFIMAGESLOCATION}' + location + '/imglist/${AOO_LANGUAGE}')
            self._addLocationIfExists(imageList, '${AOO_DEFIMAGESLOCATION}' + location + '/imglist')
            self._addLocationIfExists(imageList, '${AOO_DEFIMAGESLOCATION}' + location + '/res/${AOO_LANGUAGE}')
            self._addLocationIfExists(imageList, '${AOO_DEFIMAGESLOCATION}' + location + '/res')
            self._addLocationIfExists(imageList, '${AOO_DEFIMAGESLOCATION}' + location)
        imageArgs = ' '.join(imageList)
        self.env['AOO_IMAGEARGS'] = self.env.subst(imageArgs)

    @staticmethod
    def _addLocationIfExists(imageList, location):
        abspath = Dir(location).abspath
        if os.path.exists(abspath):
            imageList.append('-lip=' + abspath)


class AOOSrsTarget:
    def __init__(self, srsPath, srcFiles):
        self.env = DefaultEnvironment().Clone()
        self.env.Append(ENV=platform.getExecutableEnvironment(soenv))

        # This will come apart at the seams if it finds a .src file
        # with UTF-8 BOM but non-UTF-8 contents, like we produce with transex3.
        # So use a different filename extension for those.
        self.env.Append(SCANNERS=ClassicCPP("AOOSRCScanner", '.src', "CPPPATH", '^[ \t]*#[ \t]*(?:include|import)[ \t]*(<|")([^>"]+)(>|")'))
        self.srsPath = srsPath

        self.parts = []
        partTargets = []
        for srcFile in srcFiles:
            srsPartTarget = AOOSrsPartTarget(self.env, srcFile)
            self.parts.append(srsPartTarget)
            partTargets.append(srsPartTarget.target)

        self.target = self.env.Command(
            'Res/SrsTarget/' + self.srsPath + '.srs',
            partTargets,
            Action(self.build_srs))

    def AddInclude(self, includes):
        for srsPart in self.parts:
            srsPart.AddInclude(includes)

    @staticmethod
    def build_srs(target, source, env):
        # Concatenates the sources into the target.
        # shutil.copyfileobj() was benchmarked as being the fastest:
        # https://stackoverflow.com/questions/13613336/python-concatenate-text-files
        with open(target[0].abspath, 'wb') as outfd:
            for f in source:
                with open(f.abspath, 'rb') as infd:
                    shutil.copyfileobj(infd, outfd)
        return 0

class AOOSrsPartTarget:
    def __init__(self, env, file):
        srcFile = File(file)
        dstFile = File('Res/SrsPartTarget/' + file + '.part')

        # We need private env changes
        self.env = env.Clone()

        # If we're translating, there is an extra step
        withLang = self.env.get('AOO_WITH_LANG')
        if withLang == None or withLang == '':
            translatedFile = srcFile
        else:
            sdf = '${LOCDIR}/l10n/${INPATH}/misc/sdf/' + srcFile.Dir('.').srcnode().path + '/localize.sdf'
            self.env['AOO_SDF'] = sdf
            self.env['AOO_PRJ'] = srcFile.srcnode().path.split('/')[0]
            translatedFile = File('Res/SrsPartMergeTarget/' + file + '.partmerge')
            self.env.Depends(translatedFile, sdf)
            self.env.Command(translatedFile, srcFile.srcnode(), ' '.join([
                '${OUTDIR}/bin/transex3',
                '-p ${AOO_PRJ}',
                '-i ${SOURCE}',
                '-o ${TARGET}',
                '-m ${AOO_SDF}',
                '-l all']))

        # Because we're using a custom builder, we have to build includes and defs manually
        self.env['RSC_CPPPATH'] = '-I' + ' -I'.join(self.env['CPPPATH'])
        self.env['RSC_CPPDEFINES'] = '-D' + ' -D'.join(self.env['CPPDEFINES'])
        self.target = self.env.Command(dstFile, translatedFile, ' '.join([
            '${OUTDIR}/bin/rsc',
            '-s',
            '${RSC_CPPPATH}',
            '${RSC_CPPDEFINES}',
            '-fp=$TARGET',
            '$SOURCE']))

    def AddInclude(self, includes):
        self.env.Append(CPPPATH=includes)
        self.env['RSC_CPPPATH'] = '-I' + ' -I'.join(self.env['CPPPATH'])
