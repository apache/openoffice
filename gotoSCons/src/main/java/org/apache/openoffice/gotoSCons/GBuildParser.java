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

import java.io.BufferedReader;
import java.util.ArrayList;

import org.apache.openoffice.gotoSCons.raw.FunctionNode;
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

/** Parser of gbuild files. */
public class GBuildParser {
    
    /** Parses a gbuild file.
     *
     * @param reader object to read the gbuild file's contents.
     *
     * @throws Exception in case of problems in the input.
     */
    public ListNode parse(BufferedReader reader) throws Exception {
        String fullLine = "";
        String line;
        ArrayList<Node> nodes = new ArrayList<>();
        while ((line = reader.readLine()) != null) {
            if (line.endsWith("\\")) {
                fullLine += line.substring(0, line.length() - 1);
            } else {
                Node node = parseLine(fullLine + line);
                fullLine = "";
                if (node != null) {
                    nodes.add(node);
                }
            }
        }
        
        ListNode listNode = new ListNode(nodes.toArray(new Node[0]));
        return listNode;
    }
    
    private Node parseLine(String line) throws Exception {
        if (line.startsWith("#")) {
            return null;
        }
        
        if (line.isEmpty()) {
            return null;
        }
        
        //System.out.println(line);
        return evaluateExpression(line);
    }
    
    private Node evaluateExpression(String expr) throws Exception {
        // $(gb_STDLIBS)
        // $(call gb_Library_Library,fileacc)
        // $(call gb_Library_set_include,fileacc,$$(INCLUDE) -I$(SRCDIR)/fileaccess/inc)
        if (expr.startsWith("$(")) {
            StringBuilder token = new StringBuilder();
            int brackets = 1;
            int i;
            for (i = 2; i < expr.length(); i++) {
                if (expr.charAt(i) == ')') {
                    --brackets;
                    if (brackets == 0) {
                        break;
                    }
                } else if (expr.charAt(i) == '(') {
                    brackets++;
                }
                token.append(expr.charAt(i));
            }
            if (i < (expr.length() - 1)) {
                throw new Exception("Expression ends early; explicit dependencies are not supported! " +
                        "Expression was: " + expr);
            }
            
            int firstSpace = token.indexOf(" ");
            if (firstSpace > 0) {
                String function = token.substring(0, firstSpace);
                Node args = evaluateExpression(token.substring(firstSpace + 1));
                return new FunctionNode(function, args);
            } else {
                return new ValueNode(expr);
            }
        } else {
            if (expr.contains(" : ")) {
                throw new Exception("Explicit dependencies are not supported! " +
                        "Expression was: " + expr);
            }
            return new ValueNode(expr);
        }
    }
    
}
