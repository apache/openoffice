#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************


.IF "$(OOO_SUBSEQUENT_TESTS)" == ""
nothing .PHONY:
.ELSE

PRJ = ..$/..$/..$/..
PRJNAME = jurt
TARGET  = test_jurt

.IF "$(OOO_JUNIT_JAR)" != ""
PACKAGE = com$/sun$/star

# here store only Files which contain a @Test
JAVATESTFILES = \
    lib/util/NativeLibraryLoader_Test.java \
    lib/uno/environments/java/java_environment_Test.java \
    lib/uno/environments/remote/JobQueue_Test.java \
    lib/uno/environments/remote/ThreadPool_Test.java \
    lib/uno/environments/remote/JavaThreadPoolFactory_Test.java \
    lib/uno/environments/remote/ThreadId_Test.java \
    lib/uno/bridges/java_remote/BridgedObject_Test.java \
    lib/uno/bridges/java_remote/java_remote_bridge_Test.java \
    lib/uno/bridges/java_remote/ProxyFactory_Test.java \
    comp/bridgefactory/BridgeFactory_Test.java \
    comp/connections/PipedConnection_Test.java \
    uno/AnyConverter_Test.java \
    uno/UnoRuntime_EnvironmentTest.java \
    uno/WeakReference_Test.java

# put here all other files
JAVAFILES = $(JAVATESTFILES) \
    lib/uno/environments/remote/TestIWorkAt.java \
    lib/uno/environments/remote/TestWorkAt.java \
    lib/uno/environments/remote/TestReceiver.java \
    lib/uno/environments/remote/TestMessage.java

JARFILES        = ridl.jar unoil.jar jurt-test.jar juh.jar java_uno.jar test.jar OOoRunner.jar
EXTRAJARFILES = $(OOO_JUNIT_JAR)

# Sample how to debug
# JAVAIFLAGS+=-Xdebug  -Xrunjdwp:transport=dt_socket,server=y,address=9003,suspend=y

.END

.INCLUDE: settings.mk
.INCLUDE: target.mk
.INCLUDE: installationtest.mk

ALLTAR : javatest

.END

