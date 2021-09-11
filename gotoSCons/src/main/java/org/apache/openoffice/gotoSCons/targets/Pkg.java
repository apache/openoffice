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
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

public class Pkg extends BaseTarget {
    private File filename;
    private String name;
    private String baseDir;
    private Map<String, String> filesToFrom = new TreeMap<>();
    
    public Pkg(File filename) throws Exception {
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
            
            if (function.equals("gb_Package_Package")) {
                parsePackagePackage(args);
            } else if (function.equals("gb_Package_add_file")) {
                parseAddFile(args);
            } else {
                throw new Exception("UNHANDLED FUNCTION " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }

    private void parsePackagePackage(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 arg, got " + Arrays.toString(args));
        }
        this.name = args[0];
        this.baseDir = args[1];
    }

    private void parseAddFile(String[] args) throws Exception {
        if (args.length != 3) {
            throw new Exception("Expected 3 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Package isn't " + name);
        }
        if (filesToFrom.put(args[1], args[2]) != null) {
            throw new Exception("Duplicate package of file " + args[2] + " to " + args[1]);
        }
    }

    public String getBaseDir() {
        return baseDir;
    }

    public Map<String, String> getFilesToFrom() {
        return filesToFrom;
    }
}
