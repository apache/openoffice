# Apache OpenOffice<sup>®</sup> Test
Apache OpenOffice contains automated test suites that can be ran against the office. The test framework is written in Java and based on JUnit 4. 

Currently the test suite in trunk and other main branches like AOO41X and AOO42X can be ran against a normal installation of the office, a just built "installed" office, or
the RPM or DEB build artifacts.
This currently requires the user running the tests to have previously built the office from source as some required dependencies for compiling the tests are used from the office build artifacts located in main/solver.
Primarily these dependencies are things the Java UNO jars like juh, jurt, ridl, and unoil and tools like javamaker, regmerge, and idlc.

This README is documentation for using the test suites without requiring an office build environment or having built the office from source in this standalone-test branch.


When switching branches especially between AOO versions it's recommended to delete the jars in test/lib which will make sure you get the correct versions of JUnit and Hamcrest libraries for your build since the test build will try to use the jars that are present but download the correct version if not found.
