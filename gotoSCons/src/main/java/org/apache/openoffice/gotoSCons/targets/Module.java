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

package org.apache.openoffice.gotoSCons.targets;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;
import java.util.TreeSet;
import org.apache.openoffice.gotoSCons.GBuildParser;
import org.apache.openoffice.gotoSCons.Utils;
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

public class Module extends BaseTarget {
    private File filename;
    private String name;
    private Map<String, AllLangResTarget> allLangResTargets = new TreeMap<>();
    private Map<String, Library> libraries = new TreeMap<>();
    private Map<String, Executable> executables = new TreeMap<>();
    private Map<String, StaticLibrary> staticLibraries = new TreeMap<>();
    private Map<String, AntTarget> antTargets = new TreeMap<>();
    private Map<String, ZipTarget> zipTargets = new TreeMap<>();
    private TreeSet<String> targets = new TreeSet<>();
    private Map<String, Pkg> packages = new TreeMap<>();
    private Map<String, JunitTest> junitTests = new TreeMap<>();
    private Map<String, GoogleTest> googleTests = new TreeMap<>();
    
    public Module(File filename) throws Exception {
        this.filename = filename;
        try (
                BufferedReader reader = new BufferedReader(new InputStreamReader(
                        new FileInputStream(filename)))
                ) {
            ListNode rootNode = new GBuildParser().parse(reader);
            parse(rootNode);
        }
    }
    
    @Override
    protected void parseEvalCall(Node argsNode) throws Exception {
        if (argsNode instanceof ValueNode) {
            String value = ((ValueNode)argsNode).value;
            String[] tokens = value.split(",");
            
            String function = tokens[0].trim();
            String[] args = Arrays.copyOfRange(tokens, 1, tokens.length);
            
            if (function.equals("gb_Module_Module")) {
                parseModuleModule(args);
            } else if (function.equals("gb_Module_add_targets")) {
                parseModuleAddTargets(args);
            } else if (function.equals("gb_Module_add_check_targets")) {
                parseModuleAddCheckTargets(args);
            } else if (function.equals("gb_Module_add_subsequentcheck_targets")) {
                parseModuleAddSubsequentCheckTargets(args);
            } else {
                throw new Exception("Unhandled function " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }
    
    private void parseModuleModule(String[] args) throws Exception {
        if (args.length != 1) {
            throw new Exception("Expected 1 arg, got " + Arrays.toString(args));
        }
        this.name = args[0];
    }
    
    private void parseModuleAddTargets(String[] args) throws Exception {
        if (args.length < 1 || args.length > 2) {
            throw new Exception("Expected 1-2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Module isn't " + name);
        }
        if (args.length == 1) {
            return; // file list empty
        }
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            File makefile = new File(filename.getParentFile(), arg + ".mk");
            try {
                if (arg.startsWith("AllLangResTarget_")) {
                    AllLangResTarget allLangResTarget = new AllLangResTarget(makefile);
                    if (allLangResTargets.put(arg, allLangResTarget) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else if (arg.startsWith("Executable_")) {
                    Executable exe = new Executable(makefile);
                    if (executables.put(arg, exe) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else if (arg.startsWith("Library_")) {
                    Library library = new Library(makefile);
                    if (libraries.put(arg, library) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else if (arg.startsWith("StaticLibrary_")) {
                    StaticLibrary staticLibrary = new StaticLibrary(makefile);
                    if (staticLibraries.put(arg, staticLibrary) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else if (arg.startsWith("Ant_")) {
                    AntTarget antTarget = new AntTarget(makefile);
                    if (antTargets.put(arg, antTarget) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else if (arg.startsWith("Zip_")) {
                    ZipTarget zipTarget = new ZipTarget(makefile);
                    if (zipTargets.put(arg, zipTarget) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else if (arg.startsWith("Package_")) {
                    Pkg pkg = new Pkg(makefile);
                    if (packages.put(arg, pkg) != null) {
                        throw new Exception("Duplicate add of target " + arg);
                    }
                } else {
                    throw new Exception("Unsupported target " + arg);
                }
            } catch (Exception ex) {
                throw new Exception(makefile.getPath() + ":" + ex.getMessage());
            }
        }
    }

    private void parseModuleAddCheckTargets(String[] args) throws Exception {
        if (args.length < 1 || args.length > 2) {
            throw new Exception("Expected 1-2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Module isn't " + name);
        }
        if (args.length == 1) {
            return; // file list empty
        }
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            File makefile = new File(filename.getParentFile(), arg + ".mk");
            if (arg.startsWith("GoogleTest_")) {
                GoogleTest googleTest = new GoogleTest(makefile);
                if (googleTests.put(arg, googleTest) != null) {
                    throw new Exception("Duplicate add of target " + arg);
                }
            } else {
                throw new Exception("Unsupported target " + arg);
            }
        }
    }
    
    private void parseModuleAddSubsequentCheckTargets(String[] args) throws Exception {
        if (args.length < 1 || args.length > 2) {
            throw new Exception("Expected 1-2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Module isn't " + name);
        }
        if (args.length == 1) {
            return; // file list empty
        }
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            File makefile = new File(filename.getParentFile(), arg + ".mk");
            if (arg.startsWith("JunitTest_")) {
                JunitTest junitTest = new JunitTest(makefile);
                if (junitTests.put(arg, junitTest) != null) {
                    throw new Exception("Duplicate add of target " + arg);
                }
            } else {
                throw new Exception("Unsupported target " + arg);
            }
        }
    }
    
    public String getName() {
        return name;
    }

    public File getFilename() {
        return filename;
    }

    public Map<String, AllLangResTarget> getAllLangResTargets() {
        return allLangResTargets;
    }
    
    public Map<String, Library> getLibraries() {
        return libraries;
    }

    public Map<String, StaticLibrary> getStaticLibraries() {
        return staticLibraries;
    }
    
    public Map<String, Executable> getExecutables() {
        return executables;
    }

    public Map<String, AntTarget> getAntTargets() {
        return antTargets;
    }
    
    public Map<String, Pkg> getPackages() {
        return packages;
    }

    public Map<String, ZipTarget> getZipTargets() {
        return zipTargets;
    }
    
    public Map<String, JunitTest> getJunitTests() {
        return junitTests;
    }

    public Map<String, GoogleTest> getGoogleTests() {
        return googleTests;
    }

    @Override
    public String toString() {
        return "Module{" + "filename=" + filename + ", name=" + name + ", targets=" + targets + '}';
    }
}
