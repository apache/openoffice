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

import org.apache.openoffice.gotoSCons.raw.FunctionNode;
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

/** The base class for all targets */
public abstract class BaseTarget {
    
    /** Parse the contents of a gbuild file.
     *
     * @param root root node representing the gbuild file.
     *
     * @throws Exception in case of problems.
     */
    protected void parse(ListNode root) throws Exception {
        for (Node child : root.children) {
            if (child instanceof FunctionNode) {
                FunctionNode functionNode = (FunctionNode)child;
                if (functionNode.function.equals("eval")) {
                    parseEval(functionNode);
                } else {
                    throw new Exception("Top-level function isn't \"eval\" but \"" + functionNode.function + "\", full node: " + child.toString());
                }
            } else if (child instanceof ValueNode && ((ValueNode)child).toString().equals("ifneq ($(OOO_JUNIT_JAR),)")) {
            } else if (child instanceof ValueNode && ((ValueNode)child).toString().equals("ifeq ($(ENABLE_UNIT_TESTS),YES)")) {
            } else if (child instanceof ValueNode && ((ValueNode)child).toString().equals("endif")) {
            } else {
                throw new Exception("Top-level declaration isn't a function but " + child.toString());
            }
        }
    }
    
    /** Parse an eval expression.
     *
     * @throws Exception if the expression is not recognized.
     */
    protected void parseEval(FunctionNode functionNode) throws Exception {
        if (functionNode.child instanceof FunctionNode) {
            FunctionNode nestedFunction = (FunctionNode)functionNode.child;
            if (nestedFunction.function.equals("call")) {
                parseEvalCall(nestedFunction.child);
            } else {
                throw new Exception("eval's nested function isn't \"call\" but \"" + functionNode.function + "\"");
            }
        } else {
            throw new Exception("eval child not a function but " + functionNode.child.toString());
        }
    }

    /** Parse a call expression found inside an eval expression.
     *
     * @param child node representing the function to call.
     */
    abstract protected void parseEvalCall(Node child) throws Exception;
}
