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

import java.io.File;
import java.util.Set;
import java.util.TreeSet;
import org.apache.openoffice.gotoSCons.targets.Library;
import org.apache.openoffice.gotoSCons.targets.Module;
import org.apache.openoffice.gotoSCons.targets.Repository;

public class GBuildConverter {
    /** Print usage information */
    static void usage() {
        System.err.println("Usage:");
        System.err.println("   gotoSCons parsingAnalysis /path/to/main");
        System.err.println("   gotoSCons parseModule /path/to/Module_xxx.mk");
        System.err.println("   gotoSCons parseLibrary /path/to/Library_xxx.mk");
    }


    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            usage();
            System.exit(1);
        }
        if (args[0].equals("parsingAnalysis")) {
            parsingAnalysis(args);
        } else if (args[0].equals("parseModule")) {
            if (args.length < 2) {
                throw new Exception("Needs path to Module_xxx.mk");
            }
            Module module = new Module(new File(args[1]));
            Repository repo = new Repository(new File(
                    module.getFilename().getParentFile().getParentFile(), "Repository.mk"));
            new SConsConverter(repo, System.out).convertToSCons(module);
        } else if (args[0].equals("parseLibrary")) {
            if (args.length < 2) {
                throw new Exception("Needs path to Library_xxx.mk");
            }
            Library library = new Library(new File(args[1]));
            System.out.println(library);
        } else {
            usage();
            System.exit(1);
        }
    }

    public static void parsingAnalysis(String[] args) throws Exception {
        if (args.length < 2) {
            throw new Exception("Needs path to OpenOffice main source directory");
        }
        File dir = new File(args[1]);
        int modules = 0;
        int parsable = 0;
        Set<String> parsed = new TreeSet<>();
        for (File file : dir.listFiles()) {
            if (file.isDirectory()) {
                if (new File(new File(file, "prj"), "makefile.mk").exists()) {
                    ++modules;
                    try {
                        Module module = new Module(new File(file, "Module_" + file.getName() + ".mk"));
                        ++parsable;
                        parsed.add(module.getName());
                    } catch (Exception ex) {
                        System.err.println("============");
                        System.err.print(file.getName() + "#"); // for easy "module#message" separation into CSV
                        ex.printStackTrace();
                    }
                }
            }
        }
        System.out.println("Could parse: " + parsed.toString());
        System.out.println("" + parsable + " out of " + modules + " gbuild modules are parseable");
    }
}
