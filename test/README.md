# Apache OpenOffice<sup>®</sup> Test
Apache OpenOffice contains automated test suites that can be ran against the office. The test framework is written in **Java** 
and based on **JUnit 4**. 

Currently, the test suite in **trunk** and other main branches like **AOO41X** and **AOO42X** can be ran against a normal installation 
of the office or a just built "installed" office.

This currently requires the user running the tests to have previously built the office from source as some required dependencies 
for compiling the tests are used from the office build artifacts located in `main/solver`.
Primarily these dependencies are things the Java UNO jars like juh, jurt, ridl, and unoil and tools like javamaker, regmerge, 
and idlc.

This README is documentation for using the test suites without requiring an office build environment or having built the 
office from source in this **standalone-test** branch.

## Prerequisites

Apache Ant 1.10.x and Java JDK 1.8 are known to work. Windows platforms can use the 64-bit JDK even if OpenOffice has to use a 32-bit JRE.

## Caution
When switching branches especially between AOO versions it's recommended to delete the jars in `test/lib` which will make 
sure you get the correct versions of **JUnit** and **Hamcrest** libraries for your build since the test build will try to use the jars that are present but download the correct version if not found.

## Installed Type Build Setup
When we refer to an "installed" type build this means that you have built OpenOffice from source using build 
config flag `--with-package-format="installed"`.

Compiling the tests now require an SDK to be included in the office directory layout. When compiling tests using an "installed" 
type build, the office and the SDK are in built into separate directories and need combined. One method is to copy them into 
the test directory layout.

If you have just built an "installed" build of the office you should have environment variables set you can use.
For example on my Linux machine my local OpenOffice source repo is `/home/carl/dev-git/openoffice` and my variables are set like this:
```shell
SOURCE_ROOT_DIR=/home/carl/dev-git/openoffice
SRC_ROOT=/home/carl/dev-git/openoffice/main
INPATH=unxlngx6.pro
```

If you are using a new terminal you can run your environment setup script from `main` like:
```shell
source ./LinuxX86-64Env.Set.sh
```

If you have not previously ran tests you may not have a `testspace` directory under `test`.  If not create it:
```shell
mkdir $SOURCE_ROOT_DIR/test/testspace
```

And the directory for the office:
```shell
mkdir $SOURCE_ROOT_DIR/test/testspace/install
```

Now copy the "installed" office from the build location:
```shell
cp -r $SRC_ROOT/instsetoo_native/$INPATH/Apache_OpenOffice/installed/install/en-US/openoffice4 $SOURCE_ROOT_DIR/test/testspace/install/
```

Then copy the SDK into the office:
```shell
cp -r $SRC_ROOT/instsetoo_native/$INPATH/Apache_OpenOffice_SDK/installed/install/en-US/openoffice4/sdk $SOURCE_ROOT_DIR/test/testspace/install/openoffice4/
```
## Compiling the Tests
The tests are built using Ant and passing an `openoffice.home` property with the location of the office needed for dependencies.

"installed" build example:
```shell
ant -Dopenoffice.home="$PWD/testspace/install/openoffice4" compile
```
Or an office in the standard Linux installation directory:
```shell
ant -Dopenoffice.home="/opt/openoffice4" compile
```
Or Windows
```shell
ant -Dopenoffice.home="C:\Program Files (x86)\OpenOffice 4" compile
```

## Running Tests
There are three test suites available. Build Verification Test (BVT), Function Verification Test (FVT), and Performance Verification Test (PVT).

The **Build Verification Test** Suite includes approximately fifty unit tests used for general build verification.

The **Functional Verification Test** Suite includes almost nine hundred unit tests to more thoroughly test the office features and UI.  
_Not all tests are ran for each platform._

### Running a Complete Test Suite
BVT Example (Linux):
```shell
./run -Dopenoffice.home="$PWD/testspace/install/openoffice4" -tp bvt
```
BVT Example (Windows):
```shell
.\run.bat -Dopenoffice.home="C:\Program Files (x86)\OpenOffice 4" -tp bvt
```
FVT Example:
```shell
./run -Dopenoffice.home="$PWD/testspace/install/openoffice4" -tp fvt
```
### Running a Single Class
```shell
./run -Dopenoffice.home="$PWD/testspace/install/openoffice4" -tc bvt.gui.BasicFunctionTest
```
### Running a Single Test Method
```shell
./run -Dopenoffice.home="$PWD/testspace/install/openoffice4" -tm bvt:bvt.gui.BasicFunctionTest.testFindFormulasAndValues
```
### Run Help
```shell
./run -help
```

## Test Results
By default, the testing output is stored in `test/testspace/output.***` where *** is the test suite type.

* result.xml: Test result in HTML.
* result.html: Test result in XML.
* screenshot/: Screenshot pictures when test assert is failed.

Open `test/testspace/output.***/result.html` in your browser to see the testing report.

## Cleaning Up
Running clean will delete compiled classes and build directories.
```shell
ant clean
```
The `test/testspace` directory, test results and "installed" office if used need to be manually deleted when no longer needed.
