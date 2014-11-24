import sys
import os.path

origFile = sys.argv[1]
newFile = sys.argv[2]
origStr = sys.argv[3]
newStr = sys.argv[4]
f = open(origFile,"r")
str = f.read()
f.close()
str = str.replace(origStr,newStr)
if os.path.isfile(newFile):
        print newFile + " exists, not creating"
else:
        f = open(newFile,"w")
        f.write(str)
        f.close()
