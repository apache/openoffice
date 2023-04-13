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

import aooplatform

class Windows(aooplatform.Platform):
    def __init__(self):
        self.libraryGroupLayers = {}
        self.libraryGroupLayers['OOOLIBS'] = 'OOO'
        self.libraryGroupLayers['PLAINLIBS_URE'] = 'OOO'
        self.libraryGroupLayers['PLAINLIBS_OOO'] = 'OOO'
        self.libraryGroupLayers['RTLIBS'] = 'OOO'
        self.libraryGroupLayers['RTVERLIBS'] = 'OOO'
        self.libraryGroupLayers['STLLIBS'] = 'OOO'
        self.libraryGroupLayers['UNOLIBS_URE'] = 'OOO'
        self.libraryGroupLayers['UNOLIBS_OOO'] = 'OOO'
        self.libraryGroupLayers['UNOVERLIBS'] = 'OOO'
        self.layerComponentPrefixes = {}
        self.layerComponentPrefixes['OOO'] = 'vnd.sun.star.expand:$$OOO_BASE_DIR/program/'
        self.layerComponentPrefixes['URELIB'] = 'vnd.sun.star.expand:$$URE_INTERNAL_LIB_DIR/'
        self.layerComponentPrefixes['NONE'] = 'vnd.sun.star.expand:$$OOO_INBUILD_SHAREDLIB_DIR/'

    def getGUI(self):
        return 'WNT'

    def getCOMID(self):
        return 'MSC'

    def getCompilerDefs(self, soenv):
        defs = [
            'MSC',
            '_CRT_NON_CONFORMING_SWPRINTFS',
            '_CRT_NONSTDC_NO_DEPRECATE',
            '_CRT_SECURE_NO_DEPRECATE',
            '_MT',
            '_DLL',
            'CPPU_ENV=' + soenv['COMNAME'],
            'FULL_DESK',
            'M1500'
        ]
        if soenv['CPUNAME'] == 'INTEL':
            defs += ['BOOST_MEM_FN_ENABLE_CDECL']
        return defs;

    def getCPUDefs(self, soenv):
        defs = [
            'ALIGN=' + soenv['ALIGN'],
            'CPUNAME=' + soenv['CPUNAME']
        ]
        if soenv['CPUNAME'] == 'INTEL':
            defs += ['_X86_=1']
        elif soenv['CPUNAME'] == 'X86_64':
            defs += ['_AMD64_=1']
        return defs

    def getOSDefs(self, soenv):
        return [
            'WINVER=0x0500',
            '_WIN32_IE=0x0500',
            'NT351',
            'WIN32',
            'WNT'
        ]

    def getCFlags(self, soenv):
        flags = [
            '-Gd',
            '-GR',
            '-Gs',
            '-GS',
            '-nologo',
            '-Wall',
            '-wd4005',
            '-wd4061',
            '-wd4127',
            '-wd4180',
            '-wd4189',
            '-wd4191',
            '-wd4217',
            '-wd4250',
            '-wd4251',
            '-wd4255',
            '-wd4275',
            '-wd4290',
            '-wd4294',
            '-wd4350',
            '-wd4355',
            '-wd4365',
            '-wd4503',
            '-wd4505',
            '-wd4511',
            '-wd4512',
            '-wd4514',
            '-wd4611',
            '-wd4619',
            '-wd4625',
            '-wd4626',
            '-wd4640',
            '-wd4668',
            '-wd4675',
            '-wd4692',
            '-wd4710',
            '-wd4711',
            '-wd4738',
            '-wd4786',
            '-wd4800',
            '-wd4820',
            '-wd4826',
            '-Zc:wchar_t-',
            '-Zm500'
        ]
        if soenv['EXTERNAL_WARNINGS_NOT_ERRORS'] != 'TRUE':
            flags += ['-WX']
        return flags

    def getCXXFlags(self, soenv):
        flags = [
            '-Gd',
            '-GR',
            '-Gs',
            '-GS',
            '-Gy',
            '-nologo',
            '-Wall',
            '-wd4005',
            '-wd4061',
            '-wd4127',
            '-wd4180',
            '-wd4189',
            '-wd4191',
            '-wd4217',
            '-wd4250',
            '-wd4251',
            '-wd4275',
            '-wd4290',
            '-wd4294',
            '-wd4350',
            '-wd4355',
            '-wd4365',
            '-wd4503',
            '-wd4505',
            '-wd4511',
            '-wd4512',
            '-wd4514',
            '-wd4611',
            '-wd4619',
            '-wd4625',
            '-wd4626',
            '-wd4640',
            '-wd4668',
            '-wd4675',
            '-wd4692',
            '-wd4710',
            '-wd4711',
            '-wd4738',
            '-wd4786',
            '-wd4800',
            '-wd4820',
            '-wd4826',
            '-Zc:wchar_t-',
            '-Zm500'
        ]
        if soenv['EXTERNAL_WARNINGS_NOT_ERRORS'] != 'TRUE':
            flags += ['-WX']
        return flags

    def getInclude(self, soenv):
        includes = []
        for i in soenv['SOLARINC'].split(' '):
            if i.endswith('/stl'):
                continue;
            if i.startswith('-I'):
                includes.append(i[2:])
        return includes

    def getIncludeStl(self, soenv):
        includes = []
        for i in soenv['SOLARINC'].split(' '):
            if i.startswith('-I') and i.endswith('/stl'):
                includes.append(i[2:])
        return includes

    def getExceptionFlags(self, soenv, enabled):
        flags = []
        if enabled:
            flags += ['-DEXCEPTIONS_ON', '-EHa']
        else:
            flags += ['-DEXCEPTIONS_OFF']
        return flags

    def getCompilerOptFlags(self, enabled, debugLevel):
        if enabled:
            if debugLevel == 2:
                return []
            else:
                return ['-Ob1', '-Oxs', '-Oy-']
        else:
            return ['-Od']

    def getDebugCFlags(self, compiler, enableSymbols):
        return ['-Zi']

    def getLibraryDefs(self, soenv):
        return ['_DLL_']

    def getLDFlags(self, soenv, debugging, debugLevel):
        flags = [
            '-NODEFAULTLIB'
        ]
        if soenv['CPUNAME'] == 'INTEL':
            flags += ['-MACHINE:IX86']
        elif soenv['CPUNAME'] == 'X86_64':
            flags += ['-MACHINE:X64']
        if debugging:
            flags += ['-DEBUG']
        return flags;

    def getLDPATH(self, soenv):
        path = []
        ilib = soenv['ILIB']
        ilib = ilib.replace(';', ' ')
        for i in ilib.split(' '):
            if i != '.':
                path += [i]
        return path

    def getExecutableLDFlags(self, soenv, group, outDirLocation, debugging, debugLevel):
        flags = self.getLDFlags(soenv, debugging, debugLevel)
        flags += [
            '-RELEASE',
            '-BASE:0x1b000000',
            '-OPT:NOREF',
            '-INCREMENTAL:NO',
            '-DEBUG',
            '-NXCOMPAT',
            '-DYNAMICBASE'
        ]
        if soenv['CPUNAME'] == 'INTEL':
            flags += ['-SAFESEH']
        return flags

    def getGoogleTestLDFlags(self, soenv, outDirLocation, debugging, debugLevel):
        return self.getExecutableLDFlags(soenv, None, outDirLocation, debugging, debugLevel)

    def getLibraryLDFlags(self, soenv, group, outDirLocation, debugging, debugLevel):
        flags = self.getLDFlags(soenv, debugging, debugLevel)
        flags += [
            '-DLL',
            '-OPT:NOREF',
            '-NXCOMPAT',
            '-DYNAMICBASE',
        ]
        if soenv['CPUNAME'] == 'INTEL':
            flags += ['-SAFESEH']
        return flags

    def getStaticLibraryLDFlags(self, soenv, debugging, debugLevel):
        flags = self.getLDFlags(soenv, debugging, debugLevel)
        flags += ['-LIB']
        return flags

    def getStandardLibs(self):
        return [
            'kernel32',
            'msvcrt',
            'oldnames',
            'uwinapi'
        ]

    def getLibraryGroupLayer(self, group):
        return self.libraryGroupLayers[group]

    def getLibraryLayerComponentPrefix(self, layer):
        return self.layerComponentPrefixes[layer]

    def getExecutableEnvironment(self, soenv):
        return {
            'SOLARBINDIR' : soenv['OUTDIR'] + '/bin'
        }

    def getTargetTypeGUIFlags(self, isGUI):
        if isGUI:
            return '-SUBSYSTEM:WINDOWS'
        else:
            return '-SUBSYSTEM:CONSOLE'

    def getVersionScriptFlags(self, versionScript):
        return []
