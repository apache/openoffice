#TODO: Propgroups below UserMacros, ClCompile, Link, & Lib stuff, pre and postbuild events, custom compiler directives
# parsed from makefiles

import os
import string
import random
import sys
outputDir = 'converted-module-projects'
libProj = 'libProj.vcxproj'
sourceExtensions = ['c', 'cpp', 'cxx','l','y','ll','yy', 'crc']
headerExtensions = ['h', 'hxx', 'hrc']
libExtensions = ['lib']

includeDebug = False

postBuildName = 'postbuild.bat'

def main(moduleDir, mainDir):
        os.chdir(moduleDir)
        print (moduleDir)
        name = moduleDir.rsplit('\\',1)
        outputFile =(mainDir+'\\' + outputDir + '\\'+name[len(name)-1] + ".vcxproj","w+")
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
        indentationLevel = 0;
        xmlVersionTag(projFile, indentationLevel)
        projectTag(projFile, buildFile, name, mainDir, moduleDir, indentationLevel)
        
def xmlVersionTag(projFile, indentationLevel):
        printAndOutput(projFile, '<?xml version="1.0" encoding="utf-8"?>', indentationLevel)

def projectTag(projFile, buildFile, name, mainDir, moduleDir, indentationLevel):
        printAndOutput(projFile, '<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">', indentationLevel)
        projectConfiguration(projFile, indentationLevel + 1)
        globalsProperty(projFile, name, indentationLevel + 1)
        if (includeDebug):
                conditionProperty(projFile, indentationLevel + 1)
        conditionProperty(projFile, indentationLevel + 1)
        importProject(projFile, 'props', indentationLevel + 1) # imports 'props' project
        extensionSettings(projFile, indentationLevel + 1)
        if (includeDebug):
                propertySheets(projFile, indentationLevel + 1) # Include if debug version is included
        propertySheets(projFile, indentationLevel + 1)
        userMacros(projFile, indentationLevel + 1)
        if (includeDebug):
                libPropertyGroup(projFile, mainDir, name, indentationLevel + 1) # include if debug version is included
        libPropertyGroup(projFile, mainDir, name, indentationLevel + 1)
        if (includeDebug):
                itemDefinitionGroup(projFile, mainDir, moduleDir, indentationLevel + 1)
        itemDefinitionGroup(projFile, mainDir, moduleDir, indentationLevel + 1)
        headerFiles(projFile, moduleDir, indentationLevel + 1)
        sourceFiles(projFile, moduleDir, indentationLevel + 1)
        importProject(projFile, 'targets', indentationLevel + 1) # imports 'targets' project
        extensionSettings(projFile, indentationLevel + 1)
        printAndOutput(projFile, '</Project>', indentationLevel)

# Whether or not a debug version is included is defined here
def projectConfiguration(projFile, indentationLevel):
        printAndOutput(projFile, '<ItemGroup Label="ProjectConfigurations">', indentationLevel)
        printAndOutput(projFile, '<ProjectConfiguration Include="Debug|Win32">', indentationLevel + 1)
        printAndOutput(projFile, '<Configuration>Debug</Configuration>', indentationLevel + 2)
        printAndOutput(projFile, '<Platform>Win32</Platform>', indentationLevel + 2)
        printAndOutput(projFile, '</ProjectConfiguration>', indentationLevel + 1)
        printAndOutput(projFile, '<ProjectConfiguration Include="Release|Win32">', indentationLevel + 1)
        printAndOutput(projFile, '<Configuration>Release</Configuration>', indentationLevel + 2)
        printAndOutput(projFile, '<Platform>Win32</Platform>', indentationLevel + 2)
        printAndOutput(projFile, '</ProjectConfiguration>', indentationLevel + 1)
        printAndOutput(projFile, '</ItemGroup>', indentationLevel)

# Requires a GUID of form: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (8-4-4-4-12 chars & ints)
def globalsProperty(projFile, name, indentationLevel):
        printAndOutput(projFile, '<PropertyGroup Label="Globals">', indentationLevel)
        printAndOutput(projFile, '<ProjectGuid>{' + ProjectGUID() + '}</ProjectGuid>', indentationLevel + 1)
        printAndOutput(projFile, '<Keyword>Win32Proj</Keyword>', indentationLevel + 1)
        printAndOutput(projFile, '<RootNamespace>' + name + '</RootNamespace>', indentationLevel + 1)
        printAndOutput(projFile, '</PropertyGroup>', indentationLevel)

# I'm not sure how this is generated, but it looks like each VS Project has a unique identifier. This function should try to generate one.
# Could just use rand on each char in the sequence from 0-F
# Format is 8-4-4-4-12 hexadecimal characters (0-9A-F)
def ProjectGUID():
        output = ""
        for x in range(0,36):
                if x == 8 or x == 12+1 or x == 16+2 or x == 20+3:
                        output += '-'
                else:
                        output += random.choice('0123456789ABCDEF')
        return output

# May need to look in build.lst to see what type to buiild to - dll, app, etc.
def conditionProperty(projFile, indentationLevel):
        if (includeDebug):
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'" Label="Configuration">', indentationLevel)
        else:
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'" Label="Configuration">', indentationLevel)
        printAndOutput(projFile, '<ConfigurationType>Application</ConfigurationType>', indentationLevel + 1)
        printAndOutput(projFile, '<UseDebugLibraries>false</UseDebugLibraries>', indentationLevel + 1)
        printAndOutput(projFile, '<PlatformToolset>v110</PlatformToolset>', indentationLevel + 1) # Looks like this is where we can change the VS version - v110 is VS2012
        #printAndOutput(projFile, '<WholeProgramOptimization>true</WholeProgramOptimization>', indentationLevel + 1)
        printAndOutput(projFile, '<CharacterSet>NotSet</CharacterSet>', indentationLevel + 1)
        printAndOutput(projFile, '</PropertyGroup>', indentationLevel)

def importProject(projFile, proj, indentationLevel):
        printAndOutput(projFile, '<Import Project="$(VCTargetsPath)\Microsoft.Cpp.' + proj + '" />', indentationLevel)

def extensionSettings(projFile, indentationLevel):
        printAndOutput(projFile, '<ImportGroup Label="ExtensionSettings">', indentationLevel)
        printAndOutput(projFile, '</ImportGroup>', indentationLevel)

def propertySheets(projFile, indentationLevel):
        if (includeDebug):
                printAndOutput(projFile, '<ImportGroup Label="PropertySheets" Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'">', indentationLevel)
        else:
                printAndOutput(projFile, '<ImportGroup Label="PropertySheets" Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'">', indentationLevel)
        printAndOutput(projFile, '<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"', indentationLevel + 1)
        printAndOutput(projFile, 'Label="LocalAppDataPlatform" />', indentationLevel + 1)
        printAndOutput(projFile, '</ImportGroup>', indentationLevel)

def userMacros(projFile, indentationLevel):
        printAndOutput(projFile, '<PropertyGroup Label="UserMacros" />', indentationLevel)

def libPropertyGroup(projFile, mainPath, projName, indentationLevel):
        if (includeDebug):
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'">', indentationLevel)
                printAndOutput(projFile, '<LinkIncremental>true</LinkIncremental>', indentationLevel + 1)
                printAndOutput(projFile, '<LibraryPath>' + mainPath + '\\solver\\410\\wntmsci12.pro\\lib;$(LibraryPath)</LibraryPath>', indentationLevel + 1) # Make sure this is actual path!
        else:
                printAndOutput(projFile, '<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'">', indentationLevel)
                printAndOutput(projFile, '<LinkIncremental>false</LinkIncremental>', indentationLevel + 1)
                printAndOutput(projFile, '<OutDir>' + mainPath + '\\solver\\410\\wntmsci12.pro\\bin\\</OutDir>', indentationLevel + 1)
                printAndOutput(projFile, '<TargetName>' + projName + '</TargetName>', indentationLevel + 1)
        printAndOutput(projFile, '</PropertyGroup>', indentationLevel)

def itemDefinitionGroup(projFile, mainPath, modulePath, indentationLevel):
        if (includeDebug):
                printAndOutput(projFile, '<ItemDefinitionGroup Condition="\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'">', indentationLevel)
        else:
                printAndOutput(projFile, '<ItemDefinitionGroup Condition="\'$(Configuration)|$(Platform)\'==\'Release|Win32\'">', indentationLevel)
        clCompile(projFile, mainPath, modulePath, indentationLevel + 1)
        link(projFile, mainPath, indentationLevel + 1)
        printAndOutput(projFile, '</ItemDefinitionGroup>', indentationLevel)

def clCompile(projFile, mainPath, modulePath, indentationLevel):
        printAndOutput(projFile, '<ClCompile>', indentationLevel)
        if (includeDebug):
                printAndOutput(projFile, '<WarningLevel>Level3</WarningLevel>', indentationLevel + 1)
                printAndOutput(projFile, '<Optimization>Disabled</Optimization>', indentationLevel + 1)
        else:
                printAndOutput(projFile, '<WarningLevel>Level3</WarningLevel>', indentationLevel + 1)
                printAndOutput(projFile, '<Optimization>MaxSpeed</Optimization>', indentationLevel + 1)
                printAndOutput(projFile, '<FunctionLevelLinking>true</FunctionLevelLinking>', indentationLevel + 1)
                printAndOutput(projFile, '<IntrinsicFunctions>true</IntrinsicFunctions>', indentationLevel + 1)
        printAndOutput(projFile, '<PrecompiledHeader>', indentationLevel + 1)
        printAndOutput(projFile, '</PrecompiledHeader>', indentationLevel + 1)
        additionalIncludeDirectories(projFile, modulePath, indentationLevel + 1) # Debug & non are equivalent
        printAndOutput(projFile, '<PreprocessorDefinitions>WIN32;_DEBUG;_WINDOWS;WNT;CPPU_ENV;%(PreprocessorDefinitions)', indentationLevel + 1) # Needs to be found through makefile
        printAndOutput(projFile, '</PreprocessorDefinitions>', indentationLevel + 1) # Important, may need editing
        printAndOutput(projFile, '<TreatWChar_tAsBuiltInType>false</TreatWChar_tAsBuiltInType>', indentationLevel + 1) # Necessary?
        printAndOutput(projFile, '</ClCompile>', indentationLevel)

def link(projFile, mainPath, indentationLevel):
        printAndOutput(projFile, '<Link>', indentationLevel)
        if (includeDebug):
                printAndOutput(projFile, '<GenerateDebugInformation>true</GenerateDebugInformation>', indentationLevel + 1)
        else:
                printAndOutput(projFile, '<GenerateDebugInformation>false</GenerateDebugInformation>', indentationLevel + 1)
                printAndOutput(projFile, '<EnableCOMDATFolding>true</EnableCOMDATFolding>', indentationLevel + 1)
                printAndOutput(projFile, '<OptimizeReferences>true</OptimizeReferences>', indentationLevel + 1)
        printAndOutput(projFile, '<SubSystem>Windows</SubSystem>', indentationLevel + 1)
        additionalDependencies(projFile, mainPath, indentationLevel + 1)
        printAndOutput(projFile, '<AdditionalLibraryDirectories>' + mainPath + '<\\solver\\410\\wntmscipro12.pro\\lib;', indentationLevel + 1)
        printAndOutput(projFile, '%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>', indentationLevel + 1)
        printAndOutput(projFile, '<IgnoreAllDefaultLibraries>false</IgnoreAllDefaultLibraries>', indentationLevel + 1)
        printAndOutput(projFile, '<IgnoreSpecificDefaultLibraries>LIBCMT</IgnoreSpecificDefaultLibraries>', indentationLevel + 1)
        postBuildEvent(projFile, indentationLevel + 1)
        printAndOutput(projFile, '</Link>', indentationLevel)
        if (includeDebug):
                printAndOutput(projFile, '<Lib>', indentationLevel) # Doubt this is needed
                printAndOutput(projFile, '<AdditionalDependencies>sfx.lib;%(AdditionalDependencies)</AdditionalDependencies>', indentationLevel + 1)
                printAndOutput(projFile, '</Lib', indentationLevel)

def postBuildEvent(projFile, indentationLevel):
        printAndOutput(projFile, '<PostBuildEvent>', indentationLevel)
        printAndOutput(projFile, '<Command>batch\\' + postBuildName + '</Command>', indentationLevel + 1)
        printAndOutput(projFile, '</PostBuildEvent>', indentationLevel)

# Not sure which directories this will need to include
def additionalIncludeDirectories(projFile, modulePath, indentationLevel):
        printAndOutput(projFile, '<AdditionalIncludeDirectories>', indentationLevel)
        printAndOutput(projFile, modulePath + '\\inc\\pch;', indentationLevel + 1)
        printAndOutput(projFile, modulePath + '\\source', indentationLevel + 1)
        # hwpfilter\win32\hwpfilter\hwpfilter - shouldn't be needed
        cwd = modulePath.rpartition('\\')[0] # Goes back a directory from module directory
        printAndOutput(projFile, cwd + '\\solver\\410\\wntmsci12.pro\\inc', indentationLevel + 1)
        printAndOutput(projFile, cwd + '\\solver\\410\\wntmsci12.pro\inc\\external', indentationLevel + 1)
        printAndOutput(projFile, cwd + '\\solver\\410\\wntmsci12.pro\\inc\\offuh', indentationLevel + 1)
        printAndOutput(projFile, '%(AdditionalIncludeDirectories)', indentationLevel + 1)
        printAndOutput(projFile, '</AdditionalIncludeDirectories>', indentationLevel)

def additionalDependencies(projFile, mainPath, indentationLevel):
        printAndOutput(projFile, '<AdditionalDependencies>', indentationLevel)
        loadLibFiles(projFile, mainPath, indentationLevel + 1)
        #loadFileTypes(libExtensions, mainPath, '', projFile, 2) # Loads lib files
        printAndOutput(projFile, '%(AdditionalDependencies)', indentationLevel + 1)
        printAndOutput(projFile, '</AdditionalDependencies>', indentationLevel)

def headerFiles(projFile, curDir, indentationLevel):
        printAndOutput(projFile, '<ItemGroup>', indentationLevel)
        loadFileTypes(headerExtensions, curDir, '', projFile, 0, indentationLevel + 1)
        printAndOutput(projFile, '</ItemGroup>', indentationLevel)

def sourceFiles(projFile, curDir, indentationLevel):
        printAndOutput(projFile, '<ItemGroup>', indentationLevel)
        loadFileTypes(sourceExtensions, curDir, '', projFile, 1, indentationLevel + 1)
        printAndOutput(projFile, '</ItemGroup>', indentationLevel)

def printAndOutput(outputFile, out, indentationLevel):
        for i in range(indentationLevel):
                outputFile.write('\t')
                print ('\t')
        print (out)
        outputFile.write(out + '\r\n')

def loadLibFiles(projFile, mainPath, indentationLevel):
        path = mainPath + '\\solver\\410\\wntmsci12.pro\\lib\\' # This should always be the lib path
        for file in os.listdir(path):
                fileExt = file.split('.') # index 0 is name, 1 is ext
                if (len(fileExt) > 1 and fileExt[1] in libExtensions):
                        printAndOutput(projFile, file + ';', indentationLevel)

def parseOutputFile(fileName):
        #try:
        file = open(fileName,"r")
        outputString = file.read()
        it = 0
        while (1):
                libArgs = ""
                linkOut = ""
                objOut = ""
                #Limit our processing to be between the first 'Making' output
                #and the next: this should correspond to one Makefile/project
                it = outputString.find("Making: ",it)
                #Find the first line containing the Making command: the rest
                #of it should be the call to cl.exe or lib; we want the
                #arguments to cl.exe to put under additional dependencies
                makeStr = outputString[it:outputString.find("\n",it)]
                end_it = outputString.find(makeStr,it+len("Making: "))
                fileNames = []
                compIt = it
                #Process any files that were compiled and add their paths
                #to an array: this ensures that only files that should
                #actually be processed are used
                while (1):
                        compIt = outputString.find("Compiling: ",compIt,end_it)
                        if compIt == -1:
                                break
                        compIt += len("Compiling: ")
                        endCompIt = outputString.find("\n",compIt,end_it)
                        filePath = outputString[compIt:endCompIt]
                        filePath = filePath.replace(" ","")
                        filePath = filePath.split("/",1)
                        fileNames.append(filePath[1])
                #print fileNames
                #If we failed to find a 'Making' word then we are out of things
                #to process
                if (it == -1):
                        break
                #Figure out which command we are dealing with
                old_it = outputString.find("cl.exe",it)
                lib_it = outputString.find("\nlib ",it,end_it)
                if (lib_it != -1):
                        out_it = outputString.find("/OUT:")+len("/OUT:")
                        outDir = outputString[out_it:outputString.find(" ",out_it)]
                        tmp_it = lib_it
                        lib_it = outputString.find("Making: ",lib_it)
                        tmpString = outputString[tmp_it+len("\nlib "):lib_it]
                        #Remove any argument beginning with an @
                        at_it = tmpString.find("@")
                        if (at_it != -1):
                                tmpString = tmpString.replace(tmpString[at_it:tmpString.find("\n",at_it)],"")+"\n"                      
                        cmds = tmpString.split("\n")
                        for cmd in cmds:
                                cmd = fixPaths(cmd)
                                if (cmd[0:len("Microsoft")] == "Microsoft" or cmd[0:len("Copyright")] == "Copyright"):
                                        continue
                                libArgs+= cmd
                        #print outbuf
                it = old_it
                #Offset the index of 'cl.exe' to beyond it so we
                #don't find it again
                old_it += len("cl.exe")
                if (it == -1):
                        break
                #Get the index of the end of the string
                it = outputString.find("\n",it)
                if (it == -1):
                        break
                cmd = outputString[old_it:it]
                #These strings generally need to be changed
                #due to differences in the working directory
                cmd = fixPaths(cmd)
                cmd = str.split(cmd)
                #Get rid of the last element since it specifies the filename
                cmd.pop()
                newArg = []
                for arg in cmd:
                #Replace full paths
                        if arg.find(":") != -1:
                                #Don't want absolute paths
                                arg = arg.replace("C:/steve/TestARea/main","..")
                                #We need to split off the filename in the output
                                #parameter as we are running multiple files
                                #and therefor just need the directory name
                        if arg.find("-Fo") != -1:
                                temp = arg.rsplit("/",1)
                                arg = temp[0] + "/"
                                objOut = arg[3:]
                                objOut = objOut.strip(" \t\n\r")
                                continue
                #We tend to not need these paths
                        if arg.find("~") == -1:
                                newArg.append(arg)
                #Take off the last two slashes to get the deepest directory name
                #(plus the first filename) and set the project name to that
                prjName = fileNames[0].rsplit("/",2)
                prjName = prjName[1]
                patchVCProjLib(prjName,fileNames,newArg,libArgs,linkOut,objOut)
                it = end_it
def fixPaths(cmd):
        cmd = cmd.replace("../../","")
        cmd = cmd.replace("../","source")
        return cmd

def patchVCProjLib(prjName,files,arguments,libArgs,linkOut,objOut):
        rootFile = "libProj.vcxproj"
        print objOut
        #print "##########################################"
        #print prjName
        #print files
        #print arguments
        #print outDir
        compileStr = "^BEGIN_CLCOMPILE^"
        endCompileStr = "^END_CLCOMPILE^"
        clFileName = "^CLFILENAME^"
        guidStr = "^GUID^"
        addStr = "^ADDITIONAL_OPTIONS^"
        nameStr = "^PROJ_NAME^"
        outStr = "^OUTPUT^"
        libStr = "^LIB_OPTIONS^"
        linkOut = "^LINK_OUT^"
        objStr = "^OBJ_FILE_NAME^"
        f = open(rootFile,"r")
        origFile = f.read()
        it = origFile.find(compileStr)+len(compileStr)
        end_it = origFile.find(endCompileStr,it)
        compileLine = origFile[it:end_it]
        fileOut = ""
        for fName in files:
                fileOut += compileLine.replace(clFileName,fName) + "\n\t"
        origFile = origFile.replace(origFile[it-len(compileStr):end_it+len(endCompileStr)],fileOut)
        origFile = origFile.replace(guidStr,ProjectGUID())
        origFile = origFile.replace(addStr, ' '.join(map(str, arguments)))
        origFile = origFile.replace(nameStr,prjName)
        origFile = origFile.replace(libStr,libArgs)
        origFile = origFile.replace(objStr,objOut)
        outFile = open(prjName + ".vcxproj","w")
        outFile.write(origFile)

def loadFileTypes(extensions, path, matchName, projFile, extType, indentationLevel):
    for root, dirs, files in os.walk(path):
        for name in files:
            tempName = name.rsplit('\\',1)
            p = os.path.join(root,name)
            if checkFilename(tempName[len(tempName)-1], matchName, extensions):
                if (extType == 0): # 0 is headers
                        printAndOutput(projFile, '<ClInclude Include="..\\..\\..\\' + p.replace(path+"\\","") + '" />', indentationLevel)
                elif (extType == 1): # 1 is sources
                        printAndOutput(projFile, '<ClCompile Include="..\\..\\..\\' + p.replace(path+"\\","") + '" />', indentationLevel)
                else: # 2/otherwise is libs
                        printAndOutput(projFile, name + ';', indentationLevel) # Is lib file
                            
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

# Run the script from the AOO main directory - alternatively, pass in the path to the main directory below
#if len(sys.argv) == 1:
#        runEveryFolder(os.getcwd())
for arg in sys.argv:
        if arg == sys.argv[0]:
                continue
        main(arg, os.getcwd())
parseOutputFile("rdbmaker.txt")
print("\n##############################################\n")
#parseOutputFile("idl.txt")
