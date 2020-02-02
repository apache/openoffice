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

package org.apache.opeonoffice.gotoSCons.targets;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.Set;
import java.util.TreeSet;
import org.apache.openoffice.gotoSCons.GBuildParser;
import org.apache.openoffice.gotoSCons.Utils;
import org.apache.openoffice.gotoSCons.raw.ListNode;

public abstract class BaseBinary extends BaseTarget {
    private File filename;
    protected String name;
    private Set<String> packageHeaders = new TreeSet<>();
    private String precompiledHeader;
    private Set<String> apis = new TreeSet<>();
    private Set<String> defs = new TreeSet<>();
    private Set<String> includes = new TreeSet<>();
    private Set<String> exceptionObjects = new TreeSet<>();
    private Set<String> noexceptionObjects = new TreeSet<>();
    private Set<String> linkedLibs = new TreeSet<>();

    public BaseBinary(File filename) throws Exception {
        this.filename = filename;
        try (
                BufferedReader reader = new BufferedReader(new InputStreamReader(
                        new FileInputStream(filename)))
                ) {
            ListNode rootNode = new GBuildParser().parse(reader);
            parse(rootNode);
        }
    }

    protected void parseAddApi(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!apis.add(arg)) {
                throw new Exception("Duplicate API " + arg);
            }
        }
    }
    
    protected void parseAddDefs(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!defs.add(arg)) {
                throw new Exception("Duplicate def " + arg);
            }
        }
    }
    
    protected void parseAddExceptionObjects(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }

        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!exceptionObjects.add(arg)) {
                throw new Exception("Duplicate exception object " + arg);
            }
        }
    }

    protected void parseAddNoExceptionObjects(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }

        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!noexceptionObjects.add(arg)) {
                throw new Exception("Duplicate noexception object " + arg);
            }
        }
    }
    
    protected void parseAddLinkedLibs(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }

        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!linkedLibs.add(arg)) {
                throw new Exception("Duplicate linked lib " + arg);
            }
        }
    }
    
    protected void parseAddPackageHeaders(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }

        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!packageHeaders.add(arg)) {
                throw new Exception("Duplicate package header " + arg);
            }
        }
    }
    
    protected void parseAddPrecompiledHeader(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        if (precompiledHeader != null) {
            throw new Exception("Precompiled header already set");
        } else {
            precompiledHeader = args[1];
        }
    }
    
    protected void parseSetInclude(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }

        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!includes.add(arg)) {
                throw new Exception("Duplicate include " + arg);
            }
        }
    }

    public String getName() {
        return name;
    }

    public Set<String> getApis() {
        return apis;
    }

    public Set<String> getDefs() {
        return defs;
    }

    public Set<String> getExceptionObjects() {
        return exceptionObjects;
    }

    public Set<String> getIncludes() {
        return includes;
    }

    public Set<String> getLinkedLibs() {
        return linkedLibs;
    }

    public Set<String> getNoexceptionObjects() {
        return noexceptionObjects;
    }

    public Set<String> getPackageHeaders() {
        return packageHeaders;
    }

    public String getPrecompiledHeader() {
        return precompiledHeader;
    }

}
