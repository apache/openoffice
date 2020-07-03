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

from abc import ABC, abstractmethod

class Platform(ABC):
    @abstractmethod
    def getGUI(self):
        pass

    @abstractmethod
    def getCOMID(self):
        pass

    @abstractmethod
    def getCompilerDefs(self, soenv):
        pass

    @abstractmethod
    def getCPUDefs(self, soenv):
        pass

    @abstractmethod
    def getOSDefs(self, soenv):
        pass

    @abstractmethod
    def getLibraryDefs(self, soenv):
        pass

    @abstractmethod
    def getCFlags(self, soenv):
        pass

    @abstractmethod
    def getCXXFlags(self, soenv):
        pass

    @abstractmethod
    def getInclude(self, soenv):
        pass

    @abstractmethod
    def getIncludeStl(self, soenv):
        pass

    @abstractmethod
    def getExceptionFlags(self, soenv, enabled):
        pass

    @abstractmethod
    def getCompilerOptFlags(self, enabled, debugLevel):
        pass

    @abstractmethod
    def getDebugCFlags(self, compiler, enableSymbols):
        pass

    @abstractmethod
    def getExecutableLDFlags(self, soenv, group, outDirLocation, debugging, debugLevel):
        pass

    @abstractmethod
    def getGoogleTestLDFlags(self, soenv, outDirLocation, debugging, debugLevel):
        pass
    
    @abstractmethod
    def getLibraryLDFlags(self, soenv, group, outDirLocation, debugging, debugLevel):
        pass

    @abstractmethod
    def getStaticLibraryLDFlags(self, debugging, debugLevel):
        pass

    @abstractmethod
    def getStandardLibs(self):
        pass

    @abstractmethod
    def getLibraryGroupLayer(self, group):
        pass

    @abstractmethod
    def getLibraryLayerComponentPrefix(self, layer):
        pass

    @abstractmethod
    def getExecutableEnvironment(self, soenv):
        pass

    @abstractmethod
    def getTargetTypeGUIFlags(self, isGUI):
        pass

    @abstractmethod
    def getVersionScriptFlags(self, versionScript):
        pass

