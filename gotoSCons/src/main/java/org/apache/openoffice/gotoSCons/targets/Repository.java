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
import org.apache.openoffice.gotoSCons.GBuildParser;
import org.apache.openoffice.gotoSCons.Utils;
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

public class Repository extends BaseTarget {
    private File filename;
    private String name;
    private Map<String, String> libraryLayers = new TreeMap<>();
    private Map<String, String> executableLayers = new TreeMap<>();
    private Map<String, String> staticLibraryLayers = new TreeMap<>();
    
    public Repository(File filename) throws Exception {
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
            
            String function = tokens[0];
            String[] args = Arrays.copyOfRange(tokens, 1, tokens.length);
            
            if (function.equals("gb_Helper_register_repository")) {
                parseRegisterRepository(args);
            } else if (function.equals("gb_Helper_register_executables")) {
                parseRegisterExecutables(args);
            } else if (function.equals("gb_Helper_register_libraries")) {
                parseRegisterLibraries(args);
            } else if (function.equals("gb_Helper_register_static_libraries")) {
                parseRegisterStaticLibraries(args);
            } else {
                throw new Exception("Unhandled function " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }

    private void parseRegisterRepository(String[] args) throws Exception {
        if (args.length != 1) {
            throw new Exception("Expected 1 arg, got " + args);
        }
        this.name = args[0];
    }    
    
    private void parseRegisterExecutables(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + args);
        }
        String layer = args[0];
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (executableLayers.put(arg, layer) != null) {
                throw new Exception("Duplicate executable " + arg);
            }
        }
    }

    private void parseRegisterLibraries(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + args);
        }
        String layer = args[0];
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (libraryLayers.put(arg, layer) != null) {
                throw new Exception("Duplicate librarry " + arg);
            }
        }
    }

    private void parseRegisterStaticLibraries(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + args);
        }
        String layer = args[0];
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (staticLibraryLayers.put(arg, layer) != null) {
                throw new Exception("Duplicate librarry " + arg);
            }
        }
    }
    
    public String getExecutableLayer(String executable) {
        return executableLayers.get(executable);
    }
    
    public String getLibraryLayer(String library) {
        return libraryLayers.get(library);
    }
    
    public String getStaticLibraryLayer(String library) {
        return staticLibraryLayers.get(library);
    }
}
