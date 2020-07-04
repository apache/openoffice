/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

package org.apache.openoffice.gotoSCons;

import java.io.File;
import java.io.PrintStream;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import org.apache.openoffice.gotoSCons.targets.AllLangResTarget;
import org.apache.openoffice.gotoSCons.targets.AntTarget;
import org.apache.openoffice.gotoSCons.targets.BaseBinary;
import org.apache.openoffice.gotoSCons.targets.Executable;
import org.apache.openoffice.gotoSCons.targets.GoogleTest;
import org.apache.openoffice.gotoSCons.targets.JunitTest;
import org.apache.openoffice.gotoSCons.targets.Library;
import org.apache.openoffice.gotoSCons.targets.Module;
import org.apache.openoffice.gotoSCons.targets.Pkg;
import org.apache.openoffice.gotoSCons.targets.Repository;
import org.apache.openoffice.gotoSCons.targets.SrsTarget;
import org.apache.openoffice.gotoSCons.targets.StaticLibrary;

public class SConsConverter {
    private Repository repo;
    private PrintStream out;
    
    public SConsConverter(Repository repo, PrintStream out) {
        this.repo = repo;
        this.out = out;
    }
    
    public void convertToSCons(Module module) throws Exception {
        for (Library library : module.getLibraries().values()) {
            convertLibrary(library);
        }

        for (StaticLibrary library : module.getStaticLibraries().values()) {
            convertStaticLibrary(library);
        }
        
        for (Executable exe : module.getExecutables().values()) {
            convertExecutable(exe);
        }
        
        for (AntTarget antTarget : module.getAntTargets().values()) {
            convertAntTarget(antTarget);
        }
        
        for (AllLangResTarget allLangResTarget : module.getAllLangResTargets().values()) {
            convertAllLangResTarget(allLangResTarget);
        }
        
        for (Pkg pkg : module.getPackages().values()) {
            convertPackage(module, pkg);
        }
        
        for (JunitTest junitTest: module.getJunitTests().values()) {
            convertJunitTest(module, junitTest);
        }
        
        for (GoogleTest googleTest: module.getGoogleTests().values()) {
            convertGoogleTest(googleTest);
        }
    }
    
    private void convertLibrary(Library library) throws Exception {
        String objectsVariable = convertObjects(library);

        String layer = repo.getLibraryLayer(library.getName());
        out.println(String.format("%s = AOOSharedLibrary(", library.getName()));
        out.println(String.format("    '%s',", library.getName()));
        out.println(String.format("    '%s',", layer));
        out.println(String.format("    %s", objectsVariable));
        out.println(String.format(")"));

        if (!library.getLinkedLibs().isEmpty()) {
            out.println(String.format("%s.AddLinkedLibs([", library.getName()));
            boolean first = true;
            for (String linkedLib : library.getLinkedLibs()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + linkedLib + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        String componentFile = library.getComponentFile();
        if (componentFile != null) {
            int firstSlash = componentFile.indexOf('/');
            if (firstSlash < 0) {
                throw new Exception("Invalid filename " + componentFile);
            }

            out.println(String.format("%s.SetComponentFile('%s')",
                    library.getName(), componentFile.substring(firstSlash + 1)));
        }
        
        String versionScript = library.getVersionScript();
        if (versionScript != null) {
            if (versionScript.startsWith("$(SRCDIR)/")) {
                int firstSlash = versionScript.indexOf('/');
                int secondSlash = versionScript.indexOf('/', firstSlash + 1);
                versionScript = versionScript.substring(secondSlash + 1);
            }

            out.println(String.format("%s.SetVersionScript('%s')",
                    library.getName(), versionScript));
        }

        out.println(String.format("%s.InstallTo('${OUTDIR}/lib')", library.getName()));
        out.println();
    }
    
    private void convertStaticLibrary(StaticLibrary library) throws Exception {
        String objectsVariable = convertObjects(library);

        String layer = repo.getLibraryLayer(library.getName());
        out.println(String.format("%s = AOOStaticLibrary(", library.getName()));
        out.println(String.format("    '%s',", library.getName()));
        out.println(String.format("    '%s',", layer));
        out.println(String.format("    %s.objects", objectsVariable));
        out.println(String.format(")"));

        out.println(String.format("%s.InstallTo('${OUTDIR}/lib')", library.getName()));
        out.println();
    }
    
    private void convertExecutable(Executable exe) throws Exception {
        String objectsVariable = convertObjects(exe);

        String layer = repo.getExecutableLayer(exe.getName());
        out.println(String.format("%s = AOOExecutable(", exe.getName()));
        out.println(String.format("    '%s',", exe.getName()));
        out.println(String.format("    '%s',", layer));
        out.println(String.format("    %s.objects", objectsVariable));
        out.println(String.format(")"));
        
        if (exe.isTargetTypeSet()) {
            out.println(String.format("%s.SetTargetTypeGUI(%s)",
                    exe.getName(),
                    exe.isIsTargetTypeGUI() ? "True" : "False"));
        }

        if (!exe.getLinkedLibs().isEmpty()) {
            out.println(String.format("%s.AddLinkedLibs([", exe.getName()));
            boolean first = true;
            for (String linkedLib : exe.getLinkedLibs()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + linkedLib + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        out.println(String.format("%s.InstallTo('${OUTDIR}/bin')", exe.getName()));
        out.println();
    }
    
    private void convertAntTarget(AntTarget antTarget) throws Exception {
        String antVariableName = antTarget.getName() + "Ant";
        out.println(String.format("%s = AOOAnt('%s', '%s')",
                antVariableName, antTarget.getName(), antTarget.getPathToBuildXml()));
        if (antTarget.getComponentPath() != null) {
            out.println(String.format("%s.SetComponentFile('%s', '%s')",
                    antVariableName, antTarget.getComponentPath(), antTarget.getLayer()));
        }
        out.println();
    }
    
    private void convertGoogleTest(GoogleTest gtest) throws Exception {
        String objectsVariable = convertObjects(gtest);

        String layer = repo.getExecutableLayer(gtest.getName());
        out.println(String.format("%s = AOOGoogleTest(", gtest.getName()));
        out.println(String.format("    '%s',", gtest.getName()));
        out.println(String.format("    %s", objectsVariable));
        out.println(String.format(")"));
        
        if (!gtest.getLinkedLibs().isEmpty()) {
            out.println(String.format("%s.AddLinkedLibs([", gtest.getName()));
            boolean first = true;
            for (String linkedLib : gtest.getLinkedLibs()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + linkedLib + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        out.println();
    }
        
    private void convertAllLangResTarget(AllLangResTarget allLangResTarget) throws Exception {
        String srsVariableName = allLangResTarget.getName() + "Srs";
        if (allLangResTarget.getSrs().size() != 1) {
            throw new Exception("AllLangResTarget needs exactly 1 SrsTarget");
        }
        String srsName = allLangResTarget.getSrs().get(0);
        SrsTarget srsTarget = allLangResTarget.getSrsTargets().get(srsName);
        if (srsTarget == null) {
            throw new Exception("No SrsTarget found for " + srsName);
        }
        out.println(String.format("%s = AOOSrsTarget('%s', [", srsVariableName, srsName));
        boolean first = true;
        for (String file : srsTarget.getFiles()) {
            if (!first) {
                out.println(",");
            }
            out.print("    '" + file + "'");
            first = false;
        }
        out.println();
        out.println("])");
        
        if (!srsTarget.getIncludes().isEmpty()) {
            out.println(srsVariableName + ".AddInclude([");
            boolean firstInclude = true;
            for (String include : srsTarget.getIncludes()) {
                if (include.equals("$$(INCLUDE)")) {
                    continue;
                }
                if (include.startsWith("-I")) {
                    include = include.substring(2);
                }
                if (include.startsWith("$(SRCDIR)/")) {
                    int firstSlash = include.indexOf('/');
                    include = include.substring(firstSlash + 1);
                }

                if (!firstInclude) {
                    out.println(",");
                }
                out.print("    '" + include + "'");
                firstInclude = false;
            }
            out.println();
            out.println("])");
        }
        
        out.println(String.format("AOOAllLangResTarget('%s', %s)", allLangResTarget.getName(), srsVariableName));
        
        out.println();
    }
    
    private String convertObjects(BaseBinary binary) throws Exception {
        String objectsVariable = binary.getName() + "Objects";
        if (binary.getExceptionObjects().isEmpty() && binary.getNoexceptionObjects().isEmpty()) {
            throw new Exception("How can a binary have no source files?");
        }
        out.println(objectsVariable + " = AOOSharedObjects()");
        
        if (!binary.getApis().isEmpty()) {
            out.println(objectsVariable + ".AddAPI([");
            boolean first = true;
            for (String api : binary.getApis()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + api + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }
            
        if (!binary.getIncludes().isEmpty()) {
            out.println(objectsVariable + ".AddInclude([");
            boolean first = true;
            for (String include : binary.getIncludes()) {
                if (include.equals("$$(INCLUDE)")) {
                    continue;
                }
                if (include.startsWith("-I")) {
                    include = include.substring(2);
                }
                if (include.startsWith("$(SRCDIR)/")) {
                    int firstSlash = include.indexOf('/');
                    int secondSlash = include.indexOf('/', firstSlash + 1);
                    include = include.substring(secondSlash + 1);
                }

                if (!first) {
                    out.println(",");
                }
                out.print("    '" + include + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        if (!binary.getDefs().isEmpty()) {
            out.println(objectsVariable + ".AddDefs([");
            boolean first = true;
            for (String def : binary.getDefs()) {
                if (!first) {
                    out.println(",");
                }
                if (def.startsWith("-D")) {
                    def = def.substring(2);
                }
                out.print("    '" + def + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        if (!binary.getExceptionObjects().isEmpty()) {
            out.println(objectsVariable + ".AddCxxExceptionSources([");
            boolean first = true;
            for (String exceptionObject : binary.getExceptionObjects()) {
                if (!first) {
                    out.println(",");
                }
                // in: fileaccess/source/FileAccess
                // out: source/FileAccess.cxx
                int firstSlash = exceptionObject.indexOf('/');
                if (firstSlash < 0) {
                    throw new Exception("Invalid filename " + exceptionObject);
                }
                out.print("    '" + exceptionObject.substring(firstSlash + 1) + ".cxx'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        if (!binary.getNoexceptionObjects().isEmpty()) {
            out.println(objectsVariable + ".AddCxxNoExceptionSources([");
            boolean first = true;
            for (String noExceptionObject : binary.getNoexceptionObjects()) {
                if (!first) {
                    out.println(",");
                }
                // in: fileaccess/source/FileAccess
                // out: source/FileAccess.cxx
                int firstSlash = noExceptionObject.indexOf('/');
                if (firstSlash < 0) {
                    throw new Exception("Invalid filename " + noExceptionObject);
                }
                out.print("    '" + noExceptionObject.substring(firstSlash + 1) + ".cxx'");
                first = false;
            }
            out.println();
            out.println("])");
        }
        
        return objectsVariable;
    }
    
    private void convertPackage(Module module, Pkg pkg) throws Exception {
        Map<String, TreeSet<String>> filesByInstallDir = new TreeMap<>();
        
        for (Map.Entry<String, String> entry : pkg.getFilesToFrom().entrySet()) {
            String toPath = entry.getKey();
            String fromPath = pkg.getBaseDir() + '/' + entry.getValue();
            
            if (!filenameFromPath(toPath).equals(filenameFromPath(fromPath))) {
                throw new Exception("Src/dst file changes name: " + toPath + " vs " + fromPath);
            }

            File moduleDir = module.getFilename().getParentFile().getCanonicalFile();
            String expandedFromPath = fromPath.replace("$(SRCDIR)", moduleDir.getParentFile().getCanonicalPath());
            String moduleRelativeFromPath;
            if (expandedFromPath.startsWith(moduleDir.getCanonicalPath())) {
                moduleRelativeFromPath = expandedFromPath.substring(moduleDir.getCanonicalPath().length() + 1);
            } else {
                throw new Exception("Cannot find module relative path to " + fromPath);
            }
            
            String installDir = parentDir(toPath);
            TreeSet<String> installFiles = filesByInstallDir.get(installDir);
            if (installFiles == null) {
                installFiles = new TreeSet<>();
                filesByInstallDir.put(installDir, installFiles);
            }
            installFiles.add(moduleRelativeFromPath);
        }
        
        for (Map.Entry<String,TreeSet<String>> entry : filesByInstallDir.entrySet()) {
            out.println(String.format("Install('${OUTDIR}/%s', [", entry.getKey()));
            boolean first = true;
            for (String fromPath : entry.getValue()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + fromPath + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }
        out.println();
    }
    
    private void convertJunitTest(Module module, JunitTest junitTest) throws Exception {
        String junitTestVariableName = junitTest.getName() + "JunitTest";
        
        Set<String> sourceFiles = junitTest.getSourceFiles();
        String sourceFile = sourceFiles.iterator().next();
        // sot/qa/complex/olesimplestorage/OLESimpleStorageTest
        int firstSlash = sourceFile.indexOf('/');
        int secondSlash = sourceFile.indexOf('/', firstSlash + 1);
        int thirdSlash = sourceFile.indexOf('/', secondSlash + 1);
        String pathToJavaFiles = sourceFile.substring(firstSlash + 1, thirdSlash - firstSlash + 3);
        out.println(String.format("%s = AOOJunitTest('%s', '%s')",
                junitTestVariableName, junitTest.getName(), pathToJavaFiles));

        boolean first = true;
        if (!junitTest.getDefs().isEmpty()) {
            out.println(String.format("%s.SetDefs([", junitTestVariableName));
            first = true;
            for (String def : junitTest.getDefs()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + def + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }
        
        out.println(String.format("%s.AddJars([", junitTestVariableName));
        first = true;
        for (String jar : junitTest.getJars()) {
            if (!first) {
                out.println(",");
            }
            jar = jar.replace("$(OUTDIR)", "${OUTDIR}");
            out.print("    '" + jar + "'");
            first = false;
        }
        out.println();
        out.println("])");

        out.println(String.format("%s.AddTestClasses([", junitTestVariableName));
        first = true;
        for (String testClass : junitTest.getClasses()) {
            if (!first) {
                out.println(",");
            }
            out.print("    '" + testClass + "'");
            first = false;
        }
        out.println();
        out.println("])");
        out.println();
    }
    
    private static String parentDir(String path) throws Exception {
        int lastSlash = path.lastIndexOf('/');
        if (lastSlash >= 0) {
            return path.substring(0, lastSlash);
        } else {
            throw new Exception("Doesn't contain directory path: " + path);
        }
    }
    
    private static String filenameFromPath(String path) {
        int lastSlash = path.lastIndexOf('/');
        if (lastSlash >= 0) {
            return path.substring(lastSlash + 1);
        } else {
            return path;
        }
    }
}
