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
# The path to the main OO directory
# If this isn't working on a new machine, this
# is probably why
mainPaths = ["C:/steve/TestARea/main","C:/steve/TestArea/main"]
# Use this to figure out which step we are in
# Most modules will end with the same string
# that they began with
findModuleStr = "Making:"
# These commands are in the output but should not be part
# of the resulting input
skipCommands = ["Microsoft", "Copyright", "-------", "using:"]
badMapStr = "-map:"
outStr = "/OUT:"
moduleName = ""
buildFolder = "wntmsci12.pro"
curDir = ""
it = 0
# Keep track of everywhere we have seen a source file at
# and automatically add it as an include directory
# This fixes some issues with include directories
# not being detected properly when we have changed into that
# directory via cd
extraIncludeDirs = []
outputString = ""
deliverBatName = "WinDeliver.bat"
FILESIZE_LIMIT = 1024*500
inFileGroup = []
outFileGroup = []
# prjTuple = prjName, prjId, outputFile, dependencies, fileName
allProjects = []
# Visual studio doesn't like projects sharing the same name
# so keep track of ones that we've seen and append a 1 to the end of them
usedPrjNames = []
includeDebug = False

postBuildName = 'postbuild.bat'

## NEW
def getDirPath(filePath):
        dirPath = filePath.rsplit("/",1)
        return dirPath[0]

beginDepComment =  "#Begin Dependencies\n"
endDepComment = "#End Dependencies\n"


# Description:
#       Generates a random 32-character project id. Each character represents
# a hexadecimal character (0-9, A-F). Visual Studio uses the following format
# for project ids: 8-4-4-4-12 hexadecimal characters. For example, the string
# "0462A45F-E456-A56B-0289-CF4630B744E2" is a valid project id. Dashes ("-")
# are inserted between characters as specified by the format, resulting in a
# string with a length of 32+4.
# Note:
# return: 36-character string.
def ProjectGUID():
        output = ""
        for x in range(0,36):
                if x == 8 or x == 12+1 or x == 16+2 or x == 20+3:
                        output += '-'
                else:
                        output += random.choice('0123456789ABCDEF')
        return output

def findNext(str,it):
        findStr = "Entering /"
        ret_it = len(str)+1
        fStr = "null"
        tmp_it = str.find(findStr,it)
        if (tmp_it < ret_it and tmp_it != -1):
                ret_it = tmp_it
                fStr = findStr
        fStr = fStr.strip()
        return ret_it, fStr

def parseNext(str,moduleName):
        it = 0
        while (1):
                it, type = findNext(str,it)
                end_it = -1
                tmp_it = -1
                tmp = []
        #Obj Tuple = Name, inFiles, outFiles, args
                if (type != "null"):
                        it = str.find(" ",it)+1
                        it = str.find("\n",it)
                        start_it = str.rfind("/",0,it)+1
                        prjName = str[start_it:it]
                        prjName = prjName.strip()
                        it = it+2
                        end_it = str.find("\n",it)
                        args = str[it:end_it]
                        args = args.strip()
                        args = fixPaths(args)
                #Don't want to do anything with this
                        if (prjName == "inc"):
                                continue
                        patchVCProjJava(prjName,moduleName)
                else:
                        break
        return it

def newParse(fileName,moduleName):
        output = open(fileName)
        origFile = output.read()
        output.close()
        parseNext(origFile,moduleName)

# while(more cmds):
#       if(can find buildFolder+"/" in curCmd):
#               repCmd = text between "." and buildFolder+"/"
#               break
# while(more cmds):
#       tmpCmd = curCmd.remove(repCmd)
#       if(can find "../"):
#               error if repCmd = ''
#               
def fixPaths(cmd):
        cmd = cmd.replace("C:/steve/TestARea/main","..")
        cmd = cmd.replace("C:/steve/TestArea/main","..")
        return cmd

def parseDLst(path):
        repStrs = [("%_DEST%","..\solver\\410\\" + buildFolder),("%_EXT%",""),("%__SRC%",buildFolder)]
        copyStr = "xcopy \"^SRC^\" \"^DEST^\\\" /D /Y /c"
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
                                                        isDir = isDir
                                                        #print "Warning: no match for file " + f + " found"
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
        #patchLastVcProj()

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
        nestStart = "GlobalSection(NestedProjects) = preSolution\n"
        nestMiddle = "{^PRJ_ID^} = {^FOLDER_ID^}\n"
        nestEnd = "EndGlobalSection\n"
        globalSettingsStr = "\t\t{^PRJ_ID^}.Release|Win32.ActiveCfg = Release|Win32\n\t\t{^PRJ_ID^}.Release|Win32.Build.0 = Release|Win32\n"
        #Not 100% certain if the first GUID is supposed to be the solution's
        #but I can't find that GUID in any of the generated files so...
        slnIdStr = "^SOLUTION_GUID^"
        outFile = open(fileName,"w")
        outputBuf = prjHeader
        slnId = ProjectGUID()
        allPrjIds = []
        if len(projects) > 0:
                #ToDo: Add all projects from dependency
                #solution files if they exist, or add placeholder
                #Makefile projects
                for prj in projects:
                        prjStr = startPrjStr
                        prjStr = prjStr.replace(prjNameStr,prj[0])
                        tempPrj = prj[0]
                        tempPrj = tempPrj.replace(moduleName + "\\","",1)
                        tempPrj += ".vcxproj"
                        prjStr = prjStr.replace(fileNameStr,tempPrj)
                        prjStr = prjStr.replace(slnIdStr,slnId)
                        prjStr = prjStr.replace(thisPrjStr,prj[1])
                        outputBuf += prjStr + "\n"
                        outputBuf += endPrjStr
                outputBuf += beginDepComment
                outputBuf += globalStartStr
                #Include projects from the global dependencies here
                outputBuf += globalEndStr
                outFile.write(outputBuf)
        else:
                print "No projects were found for this solution, so the solution file will not be written"
        outFile.close()

def findFiles(moduleName,prjName):
        retFiles = []
        pathStr = os.path.join(moduleName,"source",prjName,"*.java")
        files = glob.glob(pathStr)
        #Change this to not use glob as it does not work recursively
        for f in files:
                f = f[len(moduleName)+1:]
                if f != "":
                        retFiles.append(f)
        return retFiles

def patchVCProjJava(prjName,moduleName):
        rootFile = "java.template"
        prjNameStr = "^PRJ_NAME^"
        guidStr = "^GUID^"
        solutionStr = "^SOLUTION_NAME^"
        fileNameStr = "^FILES^"
        includeStr = "<ClCompile Include=\"^FILE^\"/>"
        files = findFiles(moduleName,prjName)
        fileStr = ""
        for f in files:
                f = fixPaths(f)
                f = includeStr.replace("^FILE^",f)
                fileStr.append(f+"\n")
        javaFile = open(os.path.join(moduleName,"java.template"))
        origFile = javaFile.read()
        javaFile.close()
        guid = ProjectGUID()
        origFile = origFile.replace(prjNameStr,prjName)
        origFile = origFile.replace(guidStr,guid)
        origFile = origFile.replace(fileNameStr,fileStr)
        origFile = origFile.replace(solutionStr,moduleName)
        outFile = open(os.path.join(moduleName,prjName+".vcxproj"),"w")
        outFile.write(origFile)
        outFile.close()
        allProjects.append((prjName,guid))

def patchVCProjMake(prjName,isPostBuild,buildCmd,rebuildCmd,cleanCmd):
        rootFile = "make_proj_master.vcxproj"
        f = open(rootFile,"r")
        guidStr = "^GUID^"
        prjNameStr = "^PRJ_NAME^"
        buildTypeStr = "^BUILD_TYPE^"
        buildCmdStr = "^BUILD_CMD^"
        rebuildCmdStr = "^REBUILD_CMD^"
        cleanCmdStr = "^CLEAN_CMD^"
        origFile = f.read()
        moduleFileName = ""
        if isPostBuild:
                buildType = "deliver"
        else:
                buildType = "prebuild"
        f.close()
        if (isPostBuild):
                moduleFileName = prjName+"/"+prjName+"_deliver.vcxproj"
                f = open(moduleFileName,"w")
        prjId = ProjectGUID()
        origFile = origFile.replace(guidStr,sanitizeArg(prjId))
        origFile = origFile.replace(prjNameStr,sanitizeArg(prjName))
        origFile = origFile.replace(buildTypeStr,sanitizeArg(buildType))
        origFile = origFile.replace(buildCmdStr,buildCmd)
        origFile = origFile.replace(rebuildCmdStr,rebuildCmd)
        origFile = origFile.replace(cleanCmdStr,cleanCmd)
        f.write(origFile)
        f.close()
        dependencies = []
        for prj in allProjects:
                dependencies.append(prj[1])
        prjTuple = prjName.strip(), prjId, "", dependencies, moduleFileName[moduleFileName.rfind("/")+1:], []
        allProjects.append(prjTuple)        
                            
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
        print "Usage: python vcGen.py moduleName\nRun from main AOO directory and have output file in the name <modulename>/<modulename>.txt"
        exit
#moduleName = sys.argv[1]
moduleName = "xmerge"
print ###############################################
try:
        os.mkdir(os.getcwd() + "\\" + moduleName)
except:
        pass
newParse(moduleName + "/" + moduleName + ".txt",moduleName)

patchSolution(allProjects,os.path.join(moduleName, moduleName + ".sln"))
parseDLst(os.path.join(moduleName,"prj",deliverFileName))
print allProjects
