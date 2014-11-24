#TODO: Propgroups below UserMacros, ClCompile, Link, & Lib stuff, pre and postbuild events

#Issues: Which additional include directories to include?

import os
outputDir = 'converted-module-projects'
sourceExtensions = ['c', 'cpp', 'cxx','l','y','ll','yy', 'crc']
headerExtensions = ['h', 'hxx', 'hrc']
libExtensions = ['lib']

includeDebug = False

def main(moduleDir, mainDir):
        os.chdir(moduleDir)
        print (moduleDir)
        name = moduleDir.rsplit('\\',1)
        outputFile = open(mainDir+'\\' + outputDir + '\\'+name[len(name)-1] + ".vcxproj","w+")
        #printAndOutput("Name:",outputFile)
        #printAndOutput(name[1],outputFile)
        #printAndOutput('',outputFile)
        try:
                file = open('prj/d.lst')
        except IOError:
                print ('prj.lst not found, skipping folder')
                outputFile.close()
                return
        constructProjFile(outputFile, file, name[1], moduleDir, mainDir)
		
def constructProjFile(projFile, buildFile, name, moduleDir, mainDir):
	xmlVersionTag(projFile)
	projectTag(projFile, buildFile, name, mainDir, moduleDir)
	
def xmlVersionTag(projFile):
	printAndOutput(projFile, '<?xml version="1.0" encoding="utf-8"?>')

def projectTag(projFile, buildFile, name, mainDir, moduleDir):
	printAndOutput(projFile, '<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">')
	projectConfiguration(projFile)
	globalsProperty(projFile, name)
	if (includeDebug):
                conditionProperty(projFile)
	conditionProperty(projFile)
	importProject(projFile, 'props') # imports 'props' project
	extensionSettings(projFile)
	if (includeDebug):
                propertySheets(projFile) # Include if debug version is included
	propertySheets(projFile)
	userMacros(projFile)
	if (includeDebug):
                libPropertyGroup(projFile, mainDir, name) # include if debug version is included
	libPropertyGroup(projFile, mainDir, name)
	if (includeDebug):
                itemDefinitionGroup(projFile, mainDir, moduleDir)
	itemDefinitionGroup(projFile, mainDir, moduleDir)
	headerFiles(projFile, moduleDir)
	sourceFiles(projFile, moduleDir)
	importProject(projFile, 'targets') # imports 'targets' project
	extensionSettings(projFile)
	printAndOutput(projFile, '</Project>')

# Whether or not a debug version is included is defined here
def projectConfiguration(projFile):
        printAndOutput(projFile, '<ItemGroup Label="ProjectConfigurations">')
        printAndOutput(projFile, '<ProjectConfiguration Include="Debug|Win32">')
        printAndOutput(projFile, '<Configuration>Debug</Configuration>')
        printAndOutput(projFile, '<Platform>Win32</Platform>')
        printAndOutput(projFile, '</ProjectConfiguration>')
        printAndOutput(projFile, '<ProjectConfiguration Include="Release|Win32">')
        printAndOutput(projFile, '<Configuration>Release</Configuration>')
        printAndOutput(projFile, '<Platform>Win32</Platform>')
        printAndOutput(projFile, '</ProjectConfiguration>')
        printAndOutput(projFile, '</ItemGroup>')

# Requires a GUID of form: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (8-4-4-4-12 chars & ints)
def globalsProperty(projFile, name):
        printAndOutput(projFile, '<PropertyGroup Label="Globals">')
        printAndOutput(projFile, '<ProjectGuid>{' + ProjectGUID() + '}</ProjectGuid>')
        printAndOutput(projFile, '<Keyword>Win32Proj</Keyword>')
        printAndOutput(projFile, '<RootNamespace>' + name + '</RootNamespace>')
        printAndOutput(projFile, '</PropertyGroup>')

# I'm not sure how this is generated, but it looks like each VS Project has a unique identifier. This function should try to generate one.
# Could just use rand on each char in the sequence from 0-F
# Format is 8-4-4-4-12 hexadecimal characters (0-9A-F)
def ProjectGUID():
        return 'FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF'

# May need to look in build.lst to see what type to buiild to - dll, app, etc.
def conditionProperty(projFile):
        if (includeDebug):
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'" Label="Configuration">')
        else:
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'" Label="Configuration">')
        printAndOutput(projFile, '<ConfigurationType>Application</ConfigurationType>')
        printAndOutput(projFile, '<UseDebugLibraries>false</UseDebugLibraries>')
        printAndOutput(projFile, '<PlatformToolset>v110</PlatformToolset>') # Looks like this is where we can change the VS version - v110 is VS2012
        printAndOutput(projFile, '<WholeProgramOptimization>true</WholeProgramOptimization>')
        printAndOutput(projFile, '<CharacterSet>NotSet</CharacterSet>')
        printAndOutput(projFile, '</PropertyGroup>')

def importProject(projFile, proj):
        printAndOutput(projFile, '<Import Project="$(VCTargetsPath)\Microsoft.Cpp.' + proj + '" />')

def extensionSettings(projFile):
        printAndOutput(projFile, '<ImportGroup Label="ExtensionSettings">')
        printAndOutput(projFile, '</ImportGroup>')

def propertySheets(projFile):
        if (includeDebug):
                printAndOutput(projFile, '<ImportGroup Label="PropertySheets" Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'">')
        else:
                printAndOutput(projFile, '<ImportGroup Label="PropertySheets" Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'">')
        printAndOutput(projFile, '<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"')
        printAndOutput(projFile, 'Label="LocalAppDataPlatform" />')
        printAndOutput(projFile, '</ImportGroup>')

def userMacros(projFile):
        printAndOutput(projFile, '<PropertyGroup Label="UserMacros" />')

def libPropertyGroup(projFile, mainPath, projName):
        if (includeDebug):
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'">')
                printAndOutput(projFile, '<LinkIncremental>true</LinkIncremental>')
                printAndOutput(projFile, '<LibraryPath>' + mainPath + '\\solver\\410\\wntmsci12.pro\\lib;$(LibraryPath)</LibraryPath>') # Make sure this is actual path!
        else:
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'">')
                printAndOutput(projFile, '<LinkIncremental>false</LinkIncremental>')
                printAndOutput(projFile, '<OutDir>' + mainPath + '\\solver\\410\\wntmsci12.pro\\bin\\</OutDir>')
                printAndOutput(projFile, '<TargetName>' + projName + '</TargetName>')
        printAndOutput(projFile, '</PropertyGroup>')

def itemDefinitionGroup(projFile, mainPath, modulePath):
        if (includeDebug):
                printAndOutput(projFile, '<ItemDefinitionGroup Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'">')
        else:
                printAndOutput(projFile, '<ItemDefinitionGroup Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'">')
        clCompile(projFile, mainPath, modulePath)
        link(projFile, mainPath)
        printAndOutput(projFile, '</ItemDefinitionGroup>')

def clCompile(projFile, mainPath, modulePath):
        printAndOutput(projFile, '<ClCompile>')
        if (includeDebug):
                printAndOutput(projFile, '<WarningLevel>Level3</WarningLevel>')
                printAndOutput(projFile, '<Optimization>Disabled</Optimization>')
                printAndOutput(projFile, '<PrecompiledHeader>')
                printAndOutput(projFile, '</PrecompiledHeader>')
                printAndOutput(projFile, '<PreprocessorDefinitions>WIN32;_DEBUG;_WINDOWS;WNT;CPPU_ENV;%(PreprocessorDefinitions)')
                printAndOutput(projFile, '</PreprocessorDefinitions>') # Important, may need editing
        else:
                printAndOutput(projFile, '<WarningLevel>Level3</WarningLevel>')
                printAndOutput(projFile, '<Optimization>MaxSpeed</Optimization>')
                printAndOutput(projFile, '<FunctionLevelLinking>true</FunctionLevelLinking>')
                printAndOutput(projFile, '<IntrinsicFunctions>true</IntrinsicFunctions>')
        additionalIncludeDirectories(projFile, modulePath) # Debug & non are equivalent
        printAndOutput(projFile, '</ClCompile>')

def link(projFile, mainPath):
        printAndOutput(projFile, '<Link>')
        if (includeDebug):
                printAndOutput(projFile, '<SubSystem>Windows</SubSystem>')
                printAndOutput(projFile, '<GenerateDebugInformation>true</GenerateDebugInformation>')
                printAndOutput(projFile, '<AdditionalLibraryDirectories>' + mainPath + '<\\solver\\410\\wntmscipro12.pro\\lib;')
                printAndOutput(projFile, '%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>')
        else:
                printAndOutput(projFile, '<GenerateDebugInformation>true</GenerateDebugInformation>')
                printAndOutput(projFile, '<EnableCOMDATFolding>true</EnableCOMDATFolding>')
                printAndOutput(projFile, '<OptimizeReferences>true</OptimizeReferences>')
        additionalDependencies(projFile, mainPath)
        printAndOutput(projFile, '<IgnoreAllDefaultLibraries>false</IgnoreAllDefaultLibraries>')
        printAndOutput(projFile, '<IgnoreSpecificDefaultLibraries>LIBCMT</IgnoreSpecificDefaultLibraries>')
        printAndOutput(projFile, '</Link>')
        if (includeDebug):
                printAndOutput('<Lib><AdditionalDependencies>sfx.lib;%(AdditionalDependencies)</AdditionalDependencies></Lib>') # Doubt this is needed

# Not sure which directories this will need to include
def additionalIncludeDirectories(projFile, modulePath):
        printAndOutput(projFile, '<AdditionalIncludeDirectories>')
        printAndOutput(projFile, modulePath + '\\inc\\pch;')
        printAndOutput(projFile, modulePath + '\\source')
        # hwpfilter\win32\hwpfilter\hwpfilter - shouldn't be needed
        cwd = modulePath.rpartition('\\')[0]
        printAndOutput(projFile, cwd + '\\solver\\410\\wntmsci12.pro\\inc')
        printAndOutput(projFile, cwd + '\\solver\\410\\wntmsci12.pro\inc\\external')
        printAndOutput(projFile, cwd + '\\solver\\410\\wntmsci12.pro\\inc\\offuh')
        printAndOutput(projFile, '%(AdditionalIncludeDirectories)')
        printAndOutput(projFile, '</AdditionalIncludeDirectories>')

def additionalDependencies(projFile, mainPath):
        printAndOutput(projFile, '<AdditionalDependencies>')
        loadLibFiles(projFile, mainPath)
        #loadFileTypes(libExtensions, mainPath, '', projFile, 2) # Loads lib files
        printAndOutput(projFile, '%(AdditionalDependencies)')
        printAndOutput(projFile, '</AdditionalDependencies>')

def headerFiles(projFile, curDir):
        printAndOutput(projFile, '<ItemGroup>')
        loadFileTypes(headerExtensions, curDir, '', projFile, 0)
        printAndOutput(projFile, '</ItemGroup>')

def sourceFiles(projFile, curDir):
        printAndOutput(projFile, '<ItemGroup>')
        loadFileTypes(sourceExtensions, curDir, '', projFile, 1)
        printAndOutput(projFile, '</ItemGroup>')

def printAndOutput(outputFile, out):
    print (out)
    outputFile.write(out + '\r\n')

def loadLibFiles(projFile, mainPath):
        path = mainPath + '\\solver\\410\\wntmsci12.pro\\lib\\'
        for file in os.listdir(path):
                fileExt = file.split('.') # index 0 is name, 1 is ext
                if (len(fileExt) > 1 and fileExt[1] in libExtensions):
                        printAndOutput(projFile, file + ';')


def loadFileTypes(extensions, path, matchName, projFile, extType):
    for root, dirs, files in os.walk(path):
        for name in files:
            tempName = name.rsplit('\\',1)
            p = os.path.join(root,name)
            if checkFilename(tempName[len(tempName)-1], matchName, extensions):
                if (extType == 0): # 0 is headers
                        printAndOutput(projFile, '<ClInclude Include="..\\..\\..\\' + p.replace(path+"\\","") + '" />')
                elif (extType == 1): # 1 is sources
                        printAndOutput(projFile, '<ClCompile Include="..\\..\\..\\' + p.replace(path+"\\","") + '" />')
                else: # 2/otherwise is libs
                        printAndOutput(projFile, name + ';') # Is lib file
                            
def checkFilename(inputName, names, extensions):
    temp = inputName.split(".")
    if temp[0] in names:
        return True
    elif (len(temp) > 1 and temp[1] in extensions):
        return True
    else:
        return False
        
def directories(path):
    for file in os.listdir(path):
        if os.path.isdir(os.path.join(path, file)):
            yield file
		
def runEveryFolder(startDir):
    try:
        os.mkdir(startDir + '\\' + outputDir)
    except:
        pass
    for d in directories(startDir):
            print ('######################')
            main(os.path.join(startDir + '\\' + d),startDir)
            print ('######################')

runEveryFolder(os.getcwd())
