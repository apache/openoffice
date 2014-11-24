#TODO: 

import os
import string
import random
import sys
import fnmatch
import copy
import glob
libProj = 'libProj.vcxproj'
sourceExtensions = ['c', 'cpp', 'cxx','l','y','ll','yy', 'crc']
headerExtensions = ['h', 'hxx', 'hrc']
libExtensions = ['lib']
deliverFileName = "d.lst"
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
badMapStr = "-map:"
outStr = "/OUT:"
moduleName = ""
buildFolder = "wntmsci12.pro"
curDir = ""
it = 0
#Keep track of everywhere we have seen a source file at
#and automatically add it as an include directory
#This fixes some issues with include directories
#not being detected properly when we have changed into that
#directory via cd
extraIncludeDirs = []
outputString = ""
deliverBatName = "WinDeliver.bat"
FILESIZE_LIMIT = 1024*500

#prjTuple = prjName, prjId, outputFile, dependencies, fileName
allProjects = []

includeDebug = False

postBuildName = 'postbuild.bat'

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
                libOut = ""
                mapName = ""
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
                        fileath = filePath.replace("\n","")
                        newFilePath = filePath.split("/",1)
                        #Automatically include header file directories
                        #where source files were found as a safeguard against
                        #improper header detection
                        #THIS MAY CAUSE ISSUES IN SOME MODULES
                        dirPath = filePath.rsplit("/",1)
                        dirPath = dirPath[len(dirPath)-2]
                        if (not dirPath in extraIncludeDirs):
                                extraIncludeDirs.append(dirPath)
                        #Generally the compiling command includes the name
                        #of the directory that we are in, so we slice it off
                        #here
                        fileNames.append(newFilePath[1])

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
                        #outputString[old_it:end_it]
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
                                #We don't want the output command included in additional
                                #options because Visual Studio does not like that
                                tmp_it = cmd.find(outStr)
                                if (tmp_it != -1):
                                        libOut = cmd[tmp_it+len(outStr):cmd.find(" ",tmp_it)]
                                        libOut = fixPaths(libOut)
                                        cmd = cmd.replace(cmd[tmp_it:cmd.find(" ",tmp_it)],"")
                                for skipCmd in skipCommands:
                                        if (cmd[0:len(skipCmd)]):
                                                doSkip = 1
                                                break
                                if (cmd != "" and doSkip == 0):
                                        cmd = fixPaths(cmd)
                                        libArgs+= cmd

                        #Find the next instance of lib in the output
                        #If there are multiple instances, we want to make
                        #note of this and copy the first file to be build
                        #to the second location
                        old_lib_it = lib_it
                        #lib_it = outputString.find("\nlib ",lib_it+1,end_it)
                        while (lib_it != -1 and 0):
                                tmp_it = outputString.find("/OUT:", lib_it,end_it)+len("/OUT:")
                                end_tmp_it = outputString.find(" ",tmp_it,end_it)
                                #print tmp_it
                                #print outputString[tmp_it:end_it]
                                extraLib = fixPaths(outputString[tmp_it:end_tmp_it])
                                extraLibFiles.append(extraLib)
                                old_lib_it = lib_it
                                lib_it = outputString.find("\nlib ",lib_it+1,end_it)
                                print "Extra build locations for lib file: "
                                print extraLibFiles
                        lib_it = old_lib_it
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
                #Okay we may need to replace these with environment variables
                        if arg.find("~") == -1:
                                newArg.append(arg)

                if (link_it != -1):
                        #First go back and figure out what our output filename is
                        old_it = link_it + len("\nlink ")
                        out_it = outputString.rfind("-out:",it+len("-out:"),end_it)+len("-out:")
                        out_file = outputString[out_it:outputString.find(" ",out_it,end_it)]
                        out_file = fixPaths(out_file)
                        #Then use the type to select what type of project we are using
                        extensionType = out_file[out_file.rfind(".")+1:]
                        extensionType = extensionType.strip()
                        if (extensionType == "exe"):
                                cfgType = "Application"
                        elif (extensionType == "dll"):
                                cfgType = "DynamicLibrary"
                        else:
                                print "Linking with unknown output file extension type: " + extensionType + "<END>"
                                print "Defaulting to dll project type"
                                cfgType = "DynamicLibrary"
                        tmp_it = outputString.find("linking ",old_it)
                        tmpString = outputString[old_it:tmp_it]
                        #print tmpString
                        at_it = tmpString.find("@")
                        if (at_it != -1):
                                tmpString = tmpString.replace(tmpString[at_it:tmpString.find("\n",at_it)],"")+"\n"   
                        cmds = tmpString.split("\n")
                        #Also we need to move the -map: command into the XML proper
                        #Remove any argument beginning with an @  
                        for cmd in cmds:
                                doSkip = 0
                                tmp_it = cmd.find("-out:")
                                if (tmp_it != -1):
                                        cmd = cmd.replace(cmd[tmp_it:cmd.find(" ",tmp_it)],"")
                                tmp_it = cmd.find(badMapStr)
                                if (tmp_it != -1):
                                        #The map argument needs to go in the XML in order to
                                        #work properly, and should be removed from the
                                        #additional options field
                                        mapName = cmd[tmp_it+len(badMapStr):cmd.find(" ",tmp_it)]
                                        cmd = cmd.replace(cmd[tmp_it:cmd.find(" ",tmp_it)],"")
                                        mapName = fixPaths(mapName)
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
                libPrjName = prjName
                exePrjName = prjName
                #print prjName
                if lib_it != -1 and link_it != -1:
                        libPrjName += "_lib"
                        exePrjName += "_" + extensionType
                if (lib_it != -1):
                        patchVCProjLib(libPrjName,fileNames,newArg,libArgs,linkOut,objOut,extraLibFiles,libOut)
                if (link_it != -1):
                        patchVCProjExeDll(exePrjName,fileNames,newArg,exeArgs,cfgType,objOut,out_file,mapName)
                if lib_it == -1 and link_it == -1:
                        print "Neither lib nor linking commands found, vcProj will not be generated"
                it = end_it+1
                #print "Restarting with remaining output of: " + outputString[end_it:]

def tryToFindFile(inStr,repCmd,initStr):
        retFiles = []
        origStr = inStr
        origRepCmd = repCmd
        numDirectoriesUp = -1
        minStr = min(inStr.find("/"),inStr.find("."))
        maxStr = inStr.rfind("/")
        fileName = inStr[maxStr+1:]
        inStr = inStr[minStr:maxStr+1]
        repCmd = repCmd.replace(inStr,"",1)
        pathStr = repCmd.replace("..","*")
        if (fileName.find("*") == -1):
                pathStr = os.path.join(moduleName,pathStr+fileName)
        else:
                pathStr = os.path.join(moduleName,pathStr)
        pathStr = pathStr.replace("\\","/")
        files = glob.glob(pathStr)
        if len(files) == 0:
                print "WARNING! Path " + origStr + " was unable to be found"
                print "Using an initial string of " + initStr
                print "Manual action is required to fix this"
                print "Assuming that the named directory does exist"
        if len(files) > 1:
                print "WARNING! Multiple matching paths found for path "
                print origStr + ". Defaulting to include all paths"
                print "User action may be required to fix this"
                print "Matches found: "
                for f in files:
                        print f
        for f in files:
                f = f[len(moduleName)+1:]
                if f != "":
                        retFiles.append(f)
        return retFiles

def fixPaths(cmd):
        newCmds = cmd.split()
        retCmd = ""
        repCmd = ""
        for newCmd in newCmds:
                build_it = newCmd.find(buildFolder+"/")
                if (build_it == -1):
                        continue
                repCmd = newCmd[:build_it]
                repCmd = repCmd[repCmd.find("."):]
                #print "RepCmd found: " + repCmd
                break
        for newCmd in newCmds:
                #print newCmd
                newerCmd = newCmd
                newerCmd = newerCmd.replace(repCmd,"")
                if newerCmd.find("../") != -1:
                        if (repCmd == ""):
                                print "No wntmsci12.pro path found, unable to determine proper file location"
                        else:
                                foundFiles = tryToFindFile(newerCmd,repCmd,newCmd)
                                if len(foundFiles) > 0:
                                        newerCmd = ""
                                for f in foundFiles:
                                        newerCmd += "-I"+f+" "
                newerCmd = newerCmd.replace(mainPath,"..")
                #print newerCmd
                retCmd += newerCmd + " "

        return retCmd

def addExtraDirs(arguments):
        newArg = ""
        for includeDir in extraIncludeDirs:
                includeDir.strip()
                includeDir = includeDir.split("/",1)[1]
                includeDir = " -I" + includeDir + " "
                newArg += " " + includeDir
        
        arguments.append(newArg)
        return arguments

def parseDLst(path):
        repStrs = [("%_DEST%","..\solver\\410\\" + buildFolder),("%_EXT%",""),("%__SRC%",buildFolder)]
        copyStr = "xcopy \"^SRC^\" \"^DEST^\\\" /D /Y"
        try:
                d = open(path)
        except:
                print ("Warning: Failed to load prj.lst; WinDeliver.bat will not be made")
                return
        data = d.read()
        d.close()
        data = data.split("\n")
        outputBuf = ""
        for line in data:
                #We need to be one directory up from where we started
                tmpLines = line.split()
                line = ""
                for tmpLine in tmpLines:
                        tmpLine = tmpLine.replace("..\\","",1)
                        line += tmpLine + " "
                for repStr in repStrs:
                        line = line.replace(repStr[0],repStr[1])
                if (tmpLines != []):
                        lines = line.split(":",1)
                        #print lines
                        cmd = ""
                        #print lines
                        if (len(lines) == 1):
                                isDir = True
                                #Don't need the second filenames with XCopy
                                #print tmpLines
                                #tmpLines[1] = (tmpLines[1].rsplit("\\"))[0]
                                copyCmd = copyStr
                                files = line.split(" ")
                                newFiles = []
                                for f in files:
                                        if f == "":
                                                continue
                                        if f.find("*") != -1:
                                                tmp = tryToFindFile(f,"","")
                                                if len(tmp) == 0:
                                                        print "Warning: no match for file " + f + " found"
                                                elif len(tmp) > 1:
                                                        print "Warning: Multiple matches for file: " + f + " found"
                                                        print "Defaulting to use the first match"
                                                        f = tmp[0]
                                                else:
                                                        print len(tmp)
                                                        print tmp[0]
                                                        f = tmp[0]
                                        f = f.replace("/","\\")
                                        if f.strip() != "" and f.strip() != []:
                                                newFiles.append(f)

                                files = newFiles
                                dot_it = newFiles[0].rfind(".")
                                if dot_it > newFiles[0].rfind("\\"):
                                        isDir = False
                                #print files
                                if (len(files) != 2):
                                        print "Warning: unable to parse d.lst copy command"
                                        #print files
                                        continue
                                files[1] = (files[1].rsplit("\\",1))[0]
                                copyCmd = copyCmd.replace("^SRC^",files[0])
                                copyCmd = copyCmd.replace("^DEST^",files[1])
                                #Add these commands to copy over folders (including empty)
                                #and specify that the destination is a folder as well
                                if isDir:
                                        copyCmd += " /i /E"
                                outputBuf += copyCmd + "\r\n"
                        else:
                                if (lines[0].strip() == "mkdir"):
                                        outputBuf += lines[0].strip() + " " + lines[1] + "\n"
                                else:
                                        print "Unknown command " + lines[0].strip() + "; ignoring"
        out = open(os.path.join(moduleName,deliverBatName),"w")
        out.write(outputBuf)
        out.close()
        #Open the last file used and add the batch file to its post build events
        patchLastVcProj()

def insert(original, new, pos):
        '''Inserts new inside original at pos.'''
        return original[:pos] + new + original[pos:]

def patchLastVcProj():
        postBuildStr = "<PostBuildEvent>"
        postBuildStrEnd = "</Command>"
        prj = allProjects[-1]
        fileName = prj[4]
        f = open(fileName,"r")
        data = f.read()
        f.close()
        start_it = data.find(postBuildStr)
        it = start_it + len(postBuildStr)+1
        it = data.find("<Command>",it)+len("<Command>")
        end_it = data.find(postBuildStrEnd,start_it)
        prevCmd = data[it:end_it]
        if prevCmd != "":
                prevCmd += " & "
        finalStr = prevCmd + deliverBatName
        data = insert(data,finalStr,it)
        f = open(fileName,"w")
        f.write(data)

def patchSolution(projects,fileName):
        masterFileName = "solutionMaster.sln"
        prjHeader = "\nMicrosoft Visual Studio Solution File, Format Version 12.00\n# Visual Studio 2012\n"
        startDependStr = "\tProjectSection(ProjectDependencies) = postProject\n"
        endDependStr = "\tEndProjectSection\n"
        dependStr = "\t\t{^DEPENDENCY_GUID^} = {^DEPENDENCY_GUID^}"
        dependStrId = "^DEPENDENCY_GUID^"
        startPrjStr = "Project(\"{^SOLUTION_GUID^}\") = \"^PRJ_NAME^\", \"^FILENAME^\", \"{^THIS_PRJ_ID^}\""
        endPrjStr = "EndProject\n"
        prjNameStr = "^PRJ_NAME^"
        fileNameStr = "^FILENAME^"
        thisPrjStr = "^THIS_PRJ_ID^"
        slnIdStr = "^SOLUTION_GUID^"
        globalStartStr = "Global\n"
        globalEndStr = "EndGlobal\n"
        globalPreStr = "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n\t\tRelease|Win32 = Release|Win32\n\tEndGlobalSection\n"
        globalPreSolution = "\tGlobalSection(SolutionProperties) = preSolution\n\t\tHideSolutionNode = FALSE\n\tEndGlobalSection\n"
        startGlobalPostStr = "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n"
        endGlobalPostStr = "EndGlobalSection\n"
	globalSettingsStr = "\t\t{^PRJ_ID^}.Release|Win32.ActiveCfg = Release|Win32\n\t\t{^PRJ_ID^}.Release|Win32.Build.0 = Release|Win32\n"
        #Not 100% certain if the first GUID is supposed to be the solution's
        #but I can't find that GUID in any of the generated files so...
        slnIdStr = "^SOLUTION_GUID^"
        outFile = open(fileName,"w")
        outputBuf = prjHeader
        slnId = ProjectGUID()
        allPrjIds = []
        if len(projects) > 0:
                for prj in projects:
                        allPrjIds.append(prj[1])
                        prjStr = startPrjStr
                        prjStr = prjStr.replace(prjNameStr,prj[0])
                        tempPrj = prj[4]
                        tempPrj = tempPrj.replace(moduleName + "\\","",1)
                        prjStr = prjStr.replace(fileNameStr,tempPrj)
                        prjStr = prjStr.replace(slnIdStr,slnId)
                        prjStr = prjStr.replace(thisPrjStr,prj[1])
                        outputBuf += prjStr + "\n"
                        #Add any dependencies
                        if len(prj[3]) > 0:
                                outputBuf += startDependStr
                                allDepsStr = ""
                                #outputBuf += 
                                #Add each dependency
                                for dep in prj[3]:
                                        newDepStr = dependStr.replace(dependStrId, dep)
                                        allDepsStr += newDepStr + "\n"
                                outputBuf += allDepsStr
                                outputBuf += endDependStr
                        outputBuf += endPrjStr
                outputBuf += globalStartStr
                outputBuf += globalPreStr
                outputBuf += startGlobalPostStr
                for prjId in allPrjIds:
                        outputBuf += globalSettingsStr.replace("^PRJ_ID^",prjId)
                outputBuf += endGlobalPostStr
                outputBuf += globalPreSolution
                outputBuf += globalEndStr+"\n"
                outFile.write(outputBuf)
        else:
                print "No projects were found for this solution, so the solution file will not be written"
        outFile.close()

def patchVCProjExeDll(prjName,files,arguments,exeArgs,cfgType,objOut,outputFile,mapName):
        print "EXE/DLL Project"
        prjName = sanitizeArg(prjName)
        #Remove any file extenions from the project name
        if (prjName.find(".") != -1):
                prjName = prjName[0:prjName.find(".")]
        rootFile = "exeProj.vcxproj"
        cfgStr = "^CFG_TYPE^"
        compileStr = "^BEGIN_CLCOMPILE^"
        endCompileStr = "^END_CLCOMPILE^"
        clFileName = "^CLFILENAME^"
        guidStr = "^GUID^"
        addStr = "^ADDITIONAL_OPTIONS^"
        nameStr = "^PROJ_NAME^"
        outputStr = "^OUTPUT^"
        linkStr = "^LINK_OPTIONS^"
        linkOutStr = "^LINK_OUT^"
        objStr = "^OBJ_FILE_NAME^"
        preStr = "^PREBUILD^"
        postStr = "^POSTBUILD^"
        libDirStr = "^LIBRARY_DIR^"
        doMapStr = "^BOOL_DO_MAP^"
        mapFileStr = "^MAP_NAME^"
        outputFileStr = "^OUTPUT_FILE_NAME^"
        arguments = addExtraDirs(arguments)
        tmp = findDependencies(exeArgs)
        dependencies = tmp[0]
        exeArgs = tmp[1]
        if mapName != "":
                doMap = "true"
        else:
                doMap = ""
        libraryPath = "..\\solver\\410\\wntmsci12.pro\\lib;wntmsci12.pro\lib"
        linkOut = "wntmsci12.pro\\bin\\"+ outputFile[outputFile.rfind("/")+1:]
        targetName = outputFile[max(outputFile.rfind("/"),outputFile.rfind("\\"))+1:outputFile.rfind(".")]
        targetNameWExt = outputFile[max(outputFile.rfind("/"),outputFile.rfind("\\"))+1:]
        f = open(rootFile,"r")
        origFile = f.read()
        f.close()
        it = origFile.find(compileStr)+len(compileStr)
        end_it = origFile.find(endCompileStr,it)
        compileLine = origFile[it:end_it]
        prjId = sanitizeArg(ProjectGUID())
        fileOut = ""
        for fName in files:
                fileOut += compileLine.replace(clFileName,sanitizeArg(fName)) + "\n\t"
        origFile = origFile.replace(origFile[it-len(compileStr):end_it+len(endCompileStr)],fileOut)
        origFile = origFile.replace(guidStr,prjId)
        origFile = origFile.replace(addStr, ' '.join(map(str, arguments)))
        origFile = origFile.replace(nameStr,sanitizeArg(prjName))
        origFile = origFile.replace(linkStr,exeArgs)
        origFile = origFile.replace(objStr,sanitizeArg(objOut))
        origFile = origFile.replace(preStr,"")
        origFile = origFile.replace(postStr,"")
        origFile = origFile.replace(cfgStr,sanitizeArg(cfgType))
        origFile = origFile.replace(linkOutStr,sanitizeArg(linkOut))
        origFile = origFile.replace(libDirStr,sanitizeArg(libraryPath))
        origFile = origFile.replace(doMapStr,sanitizeArg(doMap))
        origFile = origFile.replace(mapFileStr,sanitizeArg(mapName))
        origFile = origFile.replace(outputStr,objOut)
        origFile = origFile.replace(outputFileStr,sanitizeArg(targetName))
        moduleFileName = os.path.join(moduleName, prjName + ".vcxproj")
        print moduleFileName
        outFile = open(moduleFileName,"w")
        if (len(origFile) > FILESIZE_LIMIT):
                print "Warning: string size exceeds 500kb, file will not be written"
                return
        prjTuple = prjName, prjId, targetNameWExt, dependencies, moduleFileName
        allProjects.append(prjTuple)
        outFile.write(origFile)
        outFile.close()
        return

def sanitizeArg(arg):
        arg = arg.replace("\n","")
        arg = arg.replace("\r","")
        arg = arg.strip()
        return arg

#Finds all library files and looks through previous
#modules in order to find any that were the output
#of that module, and if so marks that module as a dependency
def findDependencies(args):
        args = args.split()
        deps = []
        newArgs = ""
        for f in args:
                #If we do find a dependency with differing paths to the same file
                #eg wntmsci12.pro/lib/codemaker.lib vs codemaker.lib
                #use the filename that was generated as output as the path 
                newArg = f
                if f.find(".lib") != -1:
                        file_it = f.rfind("/")
                        if file_it != -1:
                                f = f[file_it+1:]
                        #We want to check just the filenames of the output files
                        #not the paths, as they may choose different locations
                        #for each file
                        #This may be incorrect, however
                        for prj in allProjects:
                                file_it = prj[2].rfind("/")
                                if (file_it != -1):
                                        prj_out = (prj[2])[file_it+1:]
                                else:
                                        prj_out = prj[2]
                                #Check if found filename was the output of a previous
                                #module
                                print "Comparing"
                                print f.strip(), prj_out.strip()
                                if (f.strip() == prj_out.strip()):
                                        print "Dep added: " + prj_out
                                        deps.append(prj[1].strip())
                                        newArg = prj[2]
                                        print prj[2]
                                        break
                newArg = sanitizeArg(newArg)
                newArgs += newArg + " "
        return (deps, newArgs)

def patchVCProjLib(prjName,files,arguments,libArgs,linkOut,objOut,extraLibFiles,libOut):
        #extraLibFiles needs to be added as a copy command in postbuild from the
        #output of the original build
        print "Lib project"
        rootFile = "libProj.vcxproj"
        prjName = sanitizeArg(prjName)
        #Remove any file extenions from the project name
        if (prjName.find(".") != -1):
                 prjName = prjName[0:prjName.find(".")]
        print prjName
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
        outputStr = "^OUTPUT^"
        libStr = "^LIB_OPTIONS^"
        linkOut = "^LINK_OUT^"
        objStr = "^OBJ_FILE_NAME^"
        libStrOut = "^LIB_OUTPUT_FILE^"
        preStr = "^PREBUILD^"
        postStr = "^POSTBUILD^"
        outputFileStr = "^OUTPUT_FILE_NAME^"
        libOut = "wntmsci12.pro\\lib\\"+ libOut[libOut.rfind("/")+1:]
        targetName = libOut[max(libOut.rfind("/"),libOut.rfind("\\"))+1:libOut.rfind(".")]
        targetNameWExt = libOut[max(libOut.rfind("/"),libOut.rfind("\\"))+1:]
        dependencies = []
        outputFile = libOut
        arguments = addExtraDirs(arguments)
        f = open(rootFile,"r")
        origFile = f.read()
        f.close()
        it = origFile.find(compileStr)+len(compileStr)
        end_it = origFile.find(endCompileStr,it)
        compileLine = origFile[it:end_it]
        fileOut = ""
        for fName in files:
                fName = fName.replace("\n","")
                fName = fName.replace("\r","")
                fileOut += compileLine.replace(clFileName,fName) + "\n\t"
        prjId = sanitizeArg(ProjectGUID())
        origFile = origFile.replace(origFile[it-len(compileStr):end_it+len(endCompileStr)],sanitizeArg(fileOut))
        origFile = origFile.replace(guidStr,prjId)
        origFile = origFile.replace(preStr,"")
        origFile = origFile.replace(postStr,"")
        origFile = origFile.replace(addStr, ' '.join(map(str, arguments)))
        origFile = origFile.replace(nameStr,sanitizeArg(prjName))
        origFile = origFile.replace(libStr,sanitizeArg(libArgs))
        origFile = origFile.replace(objStr,sanitizeArg(objOut))
        origFile = origFile.replace(libStrOut,sanitizeArg(outputFile))
        print "Lib out: " + libOut
        moduleFileName = os.path.join(moduleName, prjName + ".vcxproj")
        outFile = open(moduleFileName,"w")
        print moduleFileName
        if (len(origFile) > FILESIZE_LIMIT):
                print "Warning: string size exceeds 500kb, file will not be written"
                return
        prjTuple = prjName.strip(), prjId, targetNameWExt, dependencies, moduleFileName
        allProjects.append(prjTuple)
        outFile.write(origFile)
        outFile.close()
                            
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
        exit
moduleName = sys.argv[1]
print ###############################################
try:
        os.mkdir(os.getcwd() + "\\" + moduleName)
except:
        pass
parseOutputFile(moduleName + ".txt")
print ###############################################
print allProjects
patchSolution(allProjects,os.path.join(moduleName, moduleName + ".sln"))
parseDLst(os.path.join(moduleName,"prj",deliverFileName))
