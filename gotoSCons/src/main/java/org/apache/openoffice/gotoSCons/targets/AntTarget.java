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
import org.apache.openoffice.gotoSCons.GBuildParser;
import org.apache.openoffice.gotoSCons.raw.ListNode;
import org.apache.openoffice.gotoSCons.raw.Node;
import org.apache.openoffice.gotoSCons.raw.ValueNode;

/**
 *
 * @author dj
 */
public class AntTarget extends BaseTarget {
    private File filename;
    private String name;
    private String pathToBuildXml;
    private String componentPath;
    private String layer;

    public AntTarget(File filename) throws Exception {
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

            if (function.equals("gb_Ant_Ant")) {
                parseAntAnt(args);
            } else if (function.equals("gb_Ant_set_componentfile")) {
                parseAntSetComponentfile(args);
            } else {
                throw new Exception("UNHANDLED FUNCTION " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }
    
    private void parseAntAnt(String[] args) throws Exception {
        if (args.length != 2) {
            throw new Exception("Expected 2 args, got " + Arrays.toString(args));
        }
        this.name = args[0];
        this.pathToBuildXml = args[1];
    }

    private void parseAntSetComponentfile(String[] args) throws Exception {
        if (args.length != 3) {
            throw new Exception("Expected 3 args, got " + Arrays.toString(args));
        }
        if (!args[0].equals(name)) {
            throw new Exception("Target name isn't " + name);
        }
        this.componentPath = args[1];
        this.layer = args[2];
    }

    public String getName() {
        return name;
    }

    public String getPathToBuildXml() {
        return pathToBuildXml;
    }

    public String getComponentPath() {
        return componentPath;
    }

    public String getLayer() {
        return layer;
    }
}
