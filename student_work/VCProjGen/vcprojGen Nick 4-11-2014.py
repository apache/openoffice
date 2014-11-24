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
#The path to the main OO directory
#If this isn't working on a new machine, this
#is probably why
mainPath = "C:/steve/TestARea/main"
#Use this to figure out which step we are in
#Most modules will end with the same string
#that they began with
findModuleStr = "Making:"
#These commands are in the output but should not be part
#of the resulting input
skipCommands = ["Microsoft", "Copyright", "-------", "using:"]
outStr = "/OUT:"
moduleName = ""

FILESIZE_LIMIT = 1024*500

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
        file = open(fileName,"r")
        outputString = file.read()
        it = 0
        while (1):
                if (it > len(outputString)):
                        return
                libArgs = ""
                linkOut = ""
                objOut = ""
                exeArgs = ""
                binName = ""
                extraLibFiles = []
                #Limit our processing to be between the first 'Making' output
                #and the next: this should correspond to one Makefile/project
                it = outputString.find(findModuleStr,it)
                #Sometimes 'Making' commands are not matched at both ends;
                #go until the next one in that case
                #Find the first line containing the Making command: the rest
                #of it should be the call to cl.exe or lib; we want the
                #arguments to cl.exe to put under additional dependencies
                makeStr = outputString[it:outputString.find("\n",it)]
                end_it = outputString.find(makeStr,it+len(makeStr))
                if (end_it == -1):
                        parseAuxiliary(outputString[it:])
                fileNames = []
                compIt = it
                #Process any files that were compiled and add their paths
                #to an array: this ensures that only files that should
                #actually be processed are used
                while (1):
                        compIt = outputString.find("Compiling:",compIt,end_it)
                        if compIt == -1:
                                break
                        compIt += len("Compiling:")
                        endCompIt = outputString.find("\n",compIt,end_it)
                        filePath = outputString[compIt:endCompIt]
                        filePath = filePath.replace(" ","")
                        filePath = filePath.split("/",1)
                        fileNames.append(filePath[1])

                #If we failed to find a 'Making' word then we are out of things
                #to process
                if (it == -1):
                        break
                #Figure out which command we are dealing with
                old_it = outputString.find("cl.exe",it)
                lib_it = outputString.find("\nlib ",it,end_it)
                link_it = outputString.find("\nlink ",it,end_it)
                if (lib_it > 0 and link_it > 0):
                        print "Warning: both lib and link matched in output"
                        print "Defaulting to lib parsing"
                        #print outputString[old_it:lib_it]
                        #print end_it, it
                        #print outputString[old_it:end_it]
                if (lib_it != -1):
                        out_it = outputString.find("/OUT:")+len("/OUT:")
                        outDir = outputString[out_it:outputString.find(" ",out_it)]
                        moduleStart_it = outputString.find(findModuleStr,lib_it)
                        tmpString = outputString[lib_it+len("\nlib "):moduleStart_it]
                        #Remove any argument beginning with an @
                        at_it = tmpString.find("@")
                        if (at_it != -1):
                                tmpString = tmpString.replace(tmpString[at_it:tmpString.find("\n",at_it)],"")+"\n"                      
                        cmds = tmpString.split("\n")
                        for cmd in cmds:
                                doSkip = 0
                                for skipCmd in skipCommands:
                                        if (cmd[0:len(skipCmd)] == skipCmd):
                                                doSkip = 1
                                                break
                                if (cmd != "" and doSkip == 0):
                                        cmd = fixPaths(cmd)
                                        libArgs+= cmd

                        #Find the next instance of lib in the output
                        #If there are multiple instances, we want to make
                        #note of this and copy the first file to be build
                        #to the second location
                        lib_it = outputString.find("\nlib ",lib_it+1,end_it)
                        while (lib_it != -1):
                             tmp_it = outputString.find("/OUT:", lib_it,end_it)+len("/OUT:")
                             end_tmp_it = outputString.find(" ",tmp_it,end_it)
                             #print tmp_it
                             #print outputString[tmp_it:end_it]
                             extraLib = fixPaths(outputString[tmp_it:end_tmp_it])
                             extraLibFiles.append(extraLib)
                             lib_it = outputString.find("\nlib ",lib_it+1,end_it)
                             print "Extra build locations for lib file: "
                             print extraLibFiles
                        
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
                                arg = arg.replace(mainPath,"..")
                                #We need to split off the filename in the output
                                #parameter as we are running multiple files
                                #and therefore just need the directory name
                        if arg.find("-Fo") != -1:
                                temp = arg.rsplit("/",1)
                                arg = temp[0] + "/"
                                objOut = arg[3:]
                                objOut = objOut.strip(" \t\n\r")
                                #print objOut
                                continue
                #We tend to not need these paths
                        if arg.find("~") == -1:
                                newArg.append(arg)

                if (link_it != -1):
                        #First go back and figure out what our output filename is
                        old_it = link_it + len("\nlink ")
                        out_it = outputString.rfind("-out:",it+len("-out:"),end_it)+len("-out:")
                        out_file = outputString[out_it:outputString.find(" ",out_it,end_it)]
                        out_file = fixPaths(out_file)
                        extensionType = out_file[out_file.rfind(".")+1:]
                        if (extensionType == "exe"):
                                cfgType = "Application"
                        elif (extensionType == "dll"):
                                cfgType = "DynamicLibrary"
                        else:
                                print "Linking with unknown output file extension type: " + extensionType
                                print "Defaulting to dll project type"
                                cfgType = "DynamicLibrary"
                        tmp_it = outputString.find("linking ",old_it)
                        tmpString = outputString[old_it:tmp_it]
                        at_it = tmpString.find("@")
                        if (at_it != -1):
                                tmpString = tmpString.replace(tmpString[at_it:tmpString.find("\n",at_it)],"")+"\n"   
                        cmds = tmpString.split("\n")
                        print cmds
                        #Remove any argument beginning with an @  
                        for cmd in cmds:
                                doSkip = 0
                                tmp_it = cmd.find("-out:")
                                if (tmp_it != -1):
                                        cmd = cmd.replace(cmd[tmp_it:cmd.find(" ",tmp_it)],"")
                                for skipCmd in skipCommands:
                                        if (cmd[0:len(skipCmd)] == skipCmd):
                                                doSkip = 1
                                                break
                                if (cmd != "" and doSkip == 0):
                                        cmd = fixPaths(cmd)
                                        exeArgs += cmd
                        #Possible Issue: Detect multiple link operations in one file
                        #and copy previously built output file to new file location

                #Take off the last two slashes to get the deepest directory name
                #(plus the first filename) and set the project name to that
                prjName = fileNames[0].rsplit("/",2)
                prjName = prjName[1]
                #print prjName
                if (lib_it != -1):
                        patchVCProjLib(prjName,fileNames,newArg,libArgs,linkOut,objOut,extraLibFiles)
                elif (link_it != -1):
                        patchVCProjExeDll(prjName,fileNames,newArg,exeArgs,cfgType,objOut,out_file)
                it = end_it+1
                #print "Restarting with remaining output of: " + outputString[end_it:]

def fixPaths(cmd):
        cmd = cmd.replace("../../","")
        cmd = cmd.replace("../","source")
        return cmd

def patchVCProjExeDll(prjName,files,arguments,exeArgs,cfgType,objOut,outputFile):
        print "EXE/DLL Project"
        print outputFile
        rootFile = "exeProj.vcxproj"
        cfgStr = "^CFG_TYPE^"
        compileStr = "^BEGIN_CLCOMPILE^"
        endCompileStr = "^END_CLCOMPILE^"
        clFileName = "^CLFILENAME^"
        guidStr = "^GUID^"
        addStr = "^ADDITIONAL_OPTIONS^"
        nameStr = "^PROJ_NAME^"
        outStr = "^OUTPUT^"
        linkStr = "^LINK_OPTIONS^"
        linkOutStr = "^LINK_OUT^"
        objStr = "^OBJ_FILE_NAME^"
        preStr = "^PREBUILD^"
        postStr = "^POSTBUILD^"
        libDirStr = "^LIBRARY_DIR^"
        libraryPath = "..\\solver\\410\\wntmsci12.pro\\lib"
        #Fix Me: Dynamically find if exe or dll
        if (cfgType == "Application"):
                linkOut = "wntmsci12.pro\\bin\\"+ prjName+ ".exe"
        elif (cfgType == "DynamicLibrary"):
                linkOut = "wntmsci12.pro\\bin\\"+prjName + ".dll"
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
        origFile = origFile.replace(linkStr,exeArgs)
        origFile = origFile.replace(objStr,objOut)
        origFile = origFile.replace(preStr,"")
        origFile = origFile.replace(postStr,"")
        origFile = origFile.replace(cfgStr,cfgType)
        origFile = origFile.replace(outStr,objOut)
        origFile = origFile.replace(linkOutStr,linkOut)
        origFile = origFile.replace(libDirStr,libraryPath)
        outFile = open(moduleName + "\\" + prjName + ".vcxproj","w")
        if (len(origFile) > FILESIZE_LIMIT):
                print "Warning: string size exceeds 500kb, file will not be written"
                return
        outFile.write(origFile)
        return

def patchVCProjLib(prjName,files,arguments,libArgs,linkOut,objOut,extraLibFiles):
        #extraLibFiles needs to be added as a copy command in postbuild from the
        #output of the original build
        print "Lib project"
        rootFile = "libProj.vcxproj"
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
        outFile = open(moduleName + "\\" + prjName + ".vcxproj","w")
        if (len(origFile) > FILESIZE_LIMIT):
                print "Warning: string size exceeds 500kb, file will not be written"
                return
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
if (len(sys.argv) != 2):
        print "Usage: python vcGen.py moduleName"
moduleName = sys.argv[1]
print ###############################################
try:
        os.mkdir(os.getcwd() + "\\" + moduleName)
except:
        pass
parseOutputFile(moduleName + ".txt")
print ###############################################
