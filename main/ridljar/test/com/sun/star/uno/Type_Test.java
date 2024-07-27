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



package com.sun.star.uno;

import org.junit.Test;
import static org.junit.Assert.*;

public final class Type_Test {
    @Test
    public void testZClass() {
        assertTrue("VOID", new Type("void").getZClass() == void.class);
        assertTrue("BOOLEAN", new Type("boolean").getZClass() == boolean.class);
        assertTrue("BYTE", new Type("byte").getZClass() == byte.class);
        assertTrue("SHORT", new Type("short").getZClass() == short.class);
        assertTrue("UNSIGNED SHORT",
               new Type("unsigned short").getZClass() == short.class);
        assertTrue("LONG", new Type("long").getZClass() == int.class);
        assertTrue("UNSIGNED LONG",
               new Type("unsigned long").getZClass() == int.class);
        assertTrue("HYPER", new Type("hyper").getZClass() == long.class);
        assertTrue("UNSIGNED HYPER",
               new Type("unsigned hyper").getZClass() == long.class);
        assertTrue("FLOAT", new Type("float").getZClass() == float.class);
        assertTrue("DOUBLE", new Type("double").getZClass() == double.class);
        assertTrue("CHAR", new Type("char").getZClass() == char.class);
        assertTrue("STRING", new Type("string").getZClass() == String.class);
        assertTrue("TYPE", new Type("type").getZClass() == Type.class);
        assertTrue("ANY", new Type("any").getZClass() == Object.class);
        assertTrue("sequence of BOOLEAN",
               new Type("[]boolean", TypeClass.SEQUENCE).getZClass()
               == boolean[].class);
        assertTrue("sequence of sequence of XComponentContext",
               new Type("[][]com.sun.star.uno.XComponentContext",
                        TypeClass.SEQUENCE).getZClass()
               == XComponentContext[][].class);
        assertTrue("enum TypeClass",
               new Type("com.sun.star.uno.TypeClass",
                        TypeClass.ENUM).getZClass() == TypeClass.class);
        assertTrue("struct Uik",
               new Type("com.sun.star.uno.Uik", TypeClass.STRUCT).getZClass()
               == Uik.class);
        assertTrue("exception Exception",
               new Type("com.sun.star.uno.Exception",
                        TypeClass.EXCEPTION).getZClass()
               == com.sun.star.uno.Exception.class);
        assertTrue("exception RuntimeException",
               new Type("com.sun.star.uno.RuntimeException",
                        TypeClass.EXCEPTION).getZClass()
               == com.sun.star.uno.RuntimeException.class);
        assertTrue("exception DeploymentException",
               new Type("com.sun.star.uno.DeploymentException",
                        TypeClass.EXCEPTION).getZClass()
               == DeploymentException.class);
        assertTrue("interface XInterface",
               new Type("com.sun.star.uno.XInterface",
                        TypeClass.INTERFACE).getZClass() == XInterface.class);
        assertTrue("interface XComponentContext",
               new Type("com.sun.star.uno.XComponentContext",
                        TypeClass.INTERFACE).getZClass()
               == XComponentContext.class);

        assertTrue(new Type(boolean.class).getZClass() == boolean.class);
        assertTrue(new Type(Boolean.class).getZClass() == boolean.class);
        assertTrue(new Type(boolean[].class).getZClass() == boolean[].class);
        assertTrue(new Type(Boolean[].class).getZClass() == boolean[].class);
    }

    @Test
    public void testIsSupertypeOf() {
        Type ifc = new Type(com.sun.star.uno.XInterface.class);
        Type ctx = new Type(com.sun.star.uno.XComponentContext.class);
        Type exc = new Type(com.sun.star.uno.RuntimeException.class);
        assertTrue("LONG :> LONG", Type.LONG.isSupertypeOf(Type.LONG));
        assertTrue("not ANY :> XInterface", !Type.ANY.isSupertypeOf(ifc));
        assertTrue("ANY :> ANY", Type.ANY.isSupertypeOf(Type.ANY));
        assertTrue("not ANY :> LONG", !Type.ANY.isSupertypeOf(Type.LONG));
        assertTrue("not XInterface :> ANY", !ifc.isSupertypeOf(Type.ANY));
        assertTrue("XInterface :> XInterface", ifc.isSupertypeOf(ifc));
        assertTrue("XInterface :> XComponentContext", ifc.isSupertypeOf(ctx));
        assertTrue("not XComponentContext :> XInterface", !ctx.isSupertypeOf(ifc));
        assertTrue("XComponentContext :> XComponentContext",
               ctx.isSupertypeOf(ctx));
        assertTrue("not XInterface :> RuntimeException", !ifc.isSupertypeOf(exc));
    }
}
