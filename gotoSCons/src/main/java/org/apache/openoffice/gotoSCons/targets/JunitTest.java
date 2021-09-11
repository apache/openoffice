/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package org.apache.openoffice.gotoSCons.targets;

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
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

/**
 *
 * @author dj
 */
public class JunitTest extends BaseTarget {
    private File filename;
    private String name;
    private Set<String> defs = new TreeSet<>();
    private Set<String> jars = new TreeSet<>();
    private Set<String> sourceFiles = new TreeSet<>();
    private Set<String> classes = new TreeSet<>();
    
    public JunitTest(File filename) throws Exception {
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

            if (function.equals("gb_JunitTest_JunitTest")) {
                parseJunitTestJunitTest(args);
            } else if (function.equals("gb_JunitTest_set_defs")) {
                parseJunitTestSetDefs(args);
            } else if (function.equals("gb_JunitTest_add_jars")) {
                parseJunitAddJars(args);
            } else if (function.equals("gb_JunitTest_add_sourcefiles")) {
                parseJunitAddSourceFiles(args);
            } else if (function.equals("gb_JunitTest_add_classes")) {
                parseJunitAddClasses(args);
            } else {
                throw new Exception("UNHANDLED FUNCTION " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }
    
    private void parseJunitTestJunitTest(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 1 arg, got " + Arrays.toString(args));
        }
        this.name = args[0];
        if (!args[1].equals("SRCDIR")) {
            throw new Exception("arg 2 isn't SRCDIR");
        }
    }
    
    private void parseJunitTestSetDefs(String[] args) throws Exception {
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

    private void parseJunitAddJars(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!jars.add(arg)) {
                throw new Exception("Duplicate JAR " + arg);
            }
        }
    }
    
    private void parseJunitAddSourceFiles(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!sourceFiles.add(arg)) {
                throw new Exception("Duplicate source file " + arg);
            }
        }
    }
    
    private void parseJunitAddClasses(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        
        for (String arg : Utils.spaceSeparatedTokens(args[1])) {
            if (!classes.add(arg)) {
                throw new Exception("Duplicate class " + arg);
            }
        }
    }

    public String getName() {
        return name;
    }

    public Set<String> getClasses() {
        return classes;
    }
    
    public Set<String> getDefs() {
        return defs;
    }

    public Set<String> getJars() {
        return jars;
    }

    public Set<String> getSourceFiles() {
        return sourceFiles;
    }
}

