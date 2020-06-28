/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package org.apache.openoffice.gotoSCons.targets;

import java.util.Set;
import java.util.TreeSet;

/**
 *
 * @author dj
 */
public class SrsTarget {
    private String name;
    private Set<String> includes = new TreeSet<>();
    private Set<String> files = new TreeSet<>();

    public SrsTarget(String name) throws Exception {
        this.name = name;
    }
    
    public void setIncludes(Set<String> includes) {
        this.includes = includes;
    }
    
    public void addFiles(Set<String> files) throws Exception {
        for (String file : files) {
            if (!this.files.add(file)) {
                throw new Exception("Duplicate file object " + file);
            }
        }
    }
}
