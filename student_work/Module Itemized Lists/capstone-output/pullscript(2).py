import os
sourceFiles = ['c','cxx','l','y','ll','yy', 'crc']
headerFiles = ['h', 'hxx', 'hrc']
patchFiles = ['patch']
makefileExtensions = ['mk','pmk','GB']
makefileNames = ['Makefile','makefile']
makefileValues = ['PRJNAME','TARGET','SLOFILES','OBJFILES','LINKLINKFILES','LINKADDEDFILES'
                  ,'LINKNAME','LINKADDEDDEPS','BUILD_ACTION','CONFIGURE_ACTION']
outputDir = 'capstone-output'

def main(curdir,startDir):
    os.chdir(curdir)
    print curdir
    name = curdir.rsplit('\\',1)
    outputFile = open(startDir+'\\' + outputDir + '\\'+name[len(name)-1] + ".txt","w+")
    printAndOutput("Name:",outputFile)
    printAndOutput(name[1],outputFile)
    printAndOutput('',outputFile)
    try:
        file = open('prj/d.lst')
    except IOError:
        print 'prj.lst not found, skipping folder'
        outputFile.close()
        return
    printDeps('prj/build.lst',outputFile)
    buildrules = file.read()
    printAndOutput("Build Rules:",outputFile)
    printAndOutput(buildrules,outputFile)
    printAndOutput("Source Files:",outputFile)
    loadFileTypes(sourceFiles,curdir,"",False,outputFile)
    printAndOutput('',outputFile)
    printAndOutput("Header Files:",outputFile)
    loadFileTypes(headerFiles,curdir,"",False,outputFile)
    printAndOutput('',outputFile)
    printAndOutput("Patch Files:",outputFile)
    loadFileTypes(patchFiles,curdir,"",False,outputFile)
    printAndOutput('',outputFile)
    printAndOutput("Makefiles:",outputFile)
    loadFileTypes(makefileExtensions,curdir,makefileNames,True,outputFile)
    outputFile.close()

def printDeps(path,outputFile):
    file = open(path)
    deps = file.readline()
    print deps
    while ':' not in deps:
        deps = file.readline()
        if deps == '':
            return
    deps = deps.split(':',1)
    printAndOutput("Dependencies:",outputFile)
    printAndOutput(deps[1].strip(),outputFile)
    printAndOutput('',outputFile)
    file.close()    

def parseMakefile(name,basePath,outputFile):
    printAndOutput('',outputFile)
    printAndOutput(name.replace(basePath+"\\",""),outputFile)
    #f = open(name,"r")
    #print f
    printLine = False
    for line in open(name,"r"):
        newline = line.split("=")
        if any(k == newline[0].strip() for k in makefileValues):
            if '\\' in line:
                printLine = True
            else:
                printLine = False
            for c in ['\n','\t','\r','\\',' ']:
                line = line.replace(c,'')
            printAndOutput('\t' + line,outputFile)
        elif printLine == True:
            if not '\\' in line:
                printLine = False
            for c in ['\n','\t','\r','\\',' ']:
                line = line.replace(c,'')
            printAndOutput('\t' + line,outputFile)

def checkFilename(inputName,names,extensions):
    temp = inputName.split(".")
    if temp[0] in names:
        return True
    elif len(temp) > 1 and temp[1] in extensions:
        return True
    else:
        return False

def directories(path):
    for file in os.listdir(path):
        if os.path.isdir(os.path.join(path, file)):
            yield file

def printAndOutput(out,outputFile):
    print out
    outputFile.write(out + '\r\n')

def loadFileTypes(extensions,path,matchName,isMakefile,outputFile):
    for root, dirs, files in os.walk(path):
        for name in files:
            tempName = name.rsplit('\\',1)
            p = os.path.join(root,name)
            if checkFilename(tempName[len(tempName)-1],matchName,extensions):
                if not isMakefile:
                    printAndOutput(p.replace(path+"\\",""),outputFile)
                else:
                    parseMakefile(p,path,outputFile)

def runEveryFolder(startDir):
    try:
        os.mkdir(startDir + '\\' + outputDir)
    except:
        pass
    for d in directories(startDir):
            print '######################'
            main(os.path.join(startDir + '\\' + d),startDir)
            print '######################'

runEveryFolder('C:\\Users\\Desktop\\Desktop\\main')
