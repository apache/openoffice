gotoSCons is a Java program whose aim is to automatically convert gbuild
build instructions into SCons script files.


Compilation
===========
$ cd gotoSCons
$ mvn package


Usage
=====
$ cd gotoSCons
$ java -jar target/gotoSCons-0.1-SNAPSHOT.jar parsingAnalysis ../main


Other sources of information
============================
Check the following threads on the dev@ mailing list:

 - about build environment development (was: declaring gbuild target
   dependencies)
   https://lists.apache.org/thread.html/rc50de5d076314f6adf2ee4fe764834e4130601b39d854be06150d118%40%3Cdev.openoffice.apache.org%3E

 - Automated gbuild -> SCons convertibility now at 88.57%
   https://lists.apache.org/thread.html/r911b40a582019f641e93253df07341370cb9aeb9d1dc50474e48aa09%40%3Cdev.openoffice.apache.org%3E
