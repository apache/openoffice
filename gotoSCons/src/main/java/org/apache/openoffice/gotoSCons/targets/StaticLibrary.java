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
import org.apache.openoffice.gotoSCons.GBuildParser;
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

public class StaticLibrary extends BaseBinary {
    private File filename;

    public StaticLibrary(File filename) throws Exception {
        super(filename);
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
            
            if (function.equals("gb_StaticLibrary_StaticLibrary")) {
                parseStaticLibraryStaticLibrary(args);
            } else if (function.equals("gb_StaticLibrary_add_api")) {
                parseAddApi(args);
            } else if (function.equals("gb_StaticLibrary_add_defs")) {
                parseAddDefs(args);
            } else if (function.equals("gb_StaticLibrary_add_exception_objects")) {
                parseAddExceptionObjects(args);
            } else if (function.equals("gb_StaticLibrary_add_noexception_objects")) {
                parseAddNoExceptionObjects(args);
            } else if (function.equals("gb_StaticLibrary_add_package_headers")) {
                parseAddPackageHeaders(args);
            } else if (function.equals("gb_StaticLibrary_add_precompiled_header")) {
                parseAddPrecompiledHeader(args);
            } else if (function.equals("gb_StaticLibrary_set_include")) {
                parseSetInclude(args);
            } else {
                throw new Exception("UNHANDLED FUNCTION " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }

    private void parseStaticLibraryStaticLibrary(String[] args) throws Exception {
        if (args.length != 1) {
            throw new Exception("Expected 1 arg, got " + Arrays.toString(args));
        }
        this.name = args[0];
    }
}
