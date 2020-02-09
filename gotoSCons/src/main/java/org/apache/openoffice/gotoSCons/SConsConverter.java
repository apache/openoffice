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
import java.io.PrintStream;
import java.util.Map;
import org.apache.opeonoffice.gotoSCons.targets.BaseBinary;
import org.apache.opeonoffice.gotoSCons.targets.Executable;
import org.apache.opeonoffice.gotoSCons.targets.Library;
import org.apache.opeonoffice.gotoSCons.targets.Module;
import org.apache.opeonoffice.gotoSCons.targets.Pkg;
import org.apache.opeonoffice.gotoSCons.targets.Repository;
import org.apache.opeonoffice.gotoSCons.targets.StaticLibrary;

public class SConsConverter {
    private Repository repo;
    private PrintStream out;
    
    public SConsConverter(Repository repo, PrintStream out) {
        this.repo = repo;
        this.out = out;
    }
    
    public void convertToSCons(Module module) throws Exception {
        for (Library library : module.getLibraries().values()) {
            convertLibrary(library);
        }

        for (StaticLibrary library : module.getStaticLibraries().values()) {
            convertStaticLibrary(library);
        }
        
        for (Executable exe : module.getExecutables().values()) {
            convertExecutable(exe);
        }
        
        for (Pkg pkg : module.getPackages().values()) {
            for (Map.Entry<String, String> entry : pkg.getFilesToFrom().entrySet()) {
                String toPath = entry.getKey();
                String fromPath = pkg.getBaseDir() + '/' + entry.getValue();
                
                File moduleDir = module.getFilename().getParentFile().getCanonicalFile();
                String expandedFromPath = fromPath.replace("$(SRCDIR)", moduleDir.getParentFile().getCanonicalPath());
                String moduleRelativeFromPath;
                if (expandedFromPath.startsWith(moduleDir.getCanonicalPath())) {
                    moduleRelativeFromPath = expandedFromPath.substring(moduleDir.getCanonicalPath().length() + 1);
                } else {
                    throw new Exception("Cannot find module relative path to " + fromPath);
                }
                
                out.println(String.format("Install('${OUTDIR}/%s', '%s')", toPath, moduleRelativeFromPath));
            }
            out.println();
        }
    }
    
    private void convertLibrary(Library library) throws Exception {
        String objectsVariable = convertObjects(library);

        String layer = repo.getLibraryLayer(library.getName());
        out.println(String.format("%s = AOOSharedLibrary(", library.getName()));
        out.println(String.format("    '%s',", library.getName()));
        out.println(String.format("    '%s',", layer));
        out.println(String.format("    %s", objectsVariable));
        out.println(String.format(")"));

        if (!library.getLinkedLibs().isEmpty()) {
            out.println(String.format("%s.AddLinkedLibs([", library.getName()));
            boolean first = true;
            for (String linkedLib : library.getLinkedLibs()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + linkedLib + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        String componentFile = library.getComponentFile();
        if (componentFile != null) {
            int firstSlash = componentFile.indexOf('/');
            if (firstSlash < 0) {
                throw new Exception("Invalid filename " + componentFile);
            }

            out.println(String.format("%s.SetComponentFile('%s')",
                    library.getName(), componentFile.substring(firstSlash + 1)));
        }
        
        String versionScript = library.getVersionScript();
        if (versionScript != null) {
            if (versionScript.startsWith("$(SRCDIR)/")) {
                int firstSlash = versionScript.indexOf('/');
                int secondSlash = versionScript.indexOf('/', firstSlash + 1);
                versionScript = versionScript.substring(secondSlash + 1);
            }

            out.println(String.format("%s.SetVersionScript('%s')",
                    library.getName(), versionScript));
        }

        out.println(String.format("%s.InstallTo('${OUTDIR}/lib')", library.getName()));
        out.println();
    }
    
    private void convertStaticLibrary(StaticLibrary library) throws Exception {
        String objectsVariable = convertObjects(library);

        String layer = repo.getLibraryLayer(library.getName());
        out.println(String.format("%s = AOOStaticLibrary(", library.getName()));
        out.println(String.format("    '%s',", library.getName()));
        out.println(String.format("    '%s',", layer));
        out.println(String.format("    %s.objects", objectsVariable));
        out.println(String.format(")"));

        out.println(String.format("%s.InstallTo('${OUTDIR}/lib')", library.getName()));
        out.println();
    }
    
    private void convertExecutable(Executable exe) throws Exception {
        String objectsVariable = convertObjects(exe);

        String layer = repo.getExecutableLayer(exe.getName());
        out.println(String.format("%s = AOOExecutable(", exe.getName()));
        out.println(String.format("    '%s',", exe.getName()));
        out.println(String.format("    '%s',", layer));
        out.println(String.format("    %s.objects", objectsVariable));
        out.println(String.format(")"));
        
        if (exe.isTargetTypeSet()) {
            out.println(String.format("%s.SetTargetTypeGUI(%s)",
                    exe.getName(),
                    exe.isIsTargetTypeGUI() ? "True" : "False"));
        }

        if (!exe.getLinkedLibs().isEmpty()) {
            out.println(String.format("%s.AddLinkedLibs([", exe.getName()));
            boolean first = true;
            for (String linkedLib : exe.getLinkedLibs()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + linkedLib + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        out.println(String.format("%s.InstallTo('${OUTDIR}/bin')", exe.getName()));
        out.println();
    }
    
    private String convertObjects(BaseBinary binary) throws Exception {
        String objectsVariable = binary.getName() + "Objects";
        if (binary.getExceptionObjects().isEmpty() && binary.getNoexceptionObjects().isEmpty()) {
            throw new Exception("How can a binary have no source files?");
        }
        out.println(objectsVariable + " = AOOSharedObjects()");
        
        if (!binary.getApis().isEmpty()) {
            out.println(objectsVariable + ".AddAPI([");
            boolean first = true;
            for (String api : binary.getApis()) {
                if (!first) {
                    out.println(",");
                }
                out.print("    '" + api + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }
            
        if (!binary.getIncludes().isEmpty()) {
            out.println(objectsVariable + ".AddInclude([");
            boolean first = true;
            for (String include : binary.getIncludes()) {
                if (include.equals("$$(INCLUDE)")) {
                    continue;
                }
                if (include.startsWith("-I")) {
                    include = include.substring(2);
                }
                if (include.startsWith("$(SRCDIR)/")) {
                    int firstSlash = include.indexOf('/');
                    int secondSlash = include.indexOf('/', firstSlash + 1);
                    include = include.substring(secondSlash + 1);
                }

                if (!first) {
                    out.println(",");
                }
                out.print("    '" + include + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        if (!binary.getDefs().isEmpty()) {
            out.println(objectsVariable + ".AddDefs([");
            boolean first = true;
            for (String def : binary.getDefs()) {
                if (!first) {
                    out.println(",");
                }
                if (def.startsWith("-D")) {
                    def = def.substring(2);
                }
                out.print("    '" + def + "'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        if (!binary.getExceptionObjects().isEmpty()) {
            out.println(objectsVariable + ".AddCxxExceptionSources([");
            boolean first = true;
            for (String exceptionObject : binary.getExceptionObjects()) {
                if (!first) {
                    out.println(",");
                }
                // in: fileaccess/source/FileAccess
                // out: source/FileAccess.cxx
                int firstSlash = exceptionObject.indexOf('/');
                if (firstSlash < 0) {
                    throw new Exception("Invalid filename " + exceptionObject);
                }
                out.print("    '" + exceptionObject.substring(firstSlash + 1) + ".cxx'");
                first = false;
            }
            out.println();
            out.println("])");
        }

        if (!binary.getNoexceptionObjects().isEmpty()) {
            out.println(objectsVariable + ".AddCxxNoExceptionSources([");
            boolean first = true;
            for (String noExceptionObject : binary.getNoexceptionObjects()) {
                if (!first) {
                    out.println(",");
                }
                // in: fileaccess/source/FileAccess
                // out: source/FileAccess.cxx
                int firstSlash = noExceptionObject.indexOf('/');
                if (firstSlash < 0) {
                    throw new Exception("Invalid filename " + noExceptionObject);
                }
                out.print("    '" + noExceptionObject.substring(firstSlash + 1) + ".cxx'");
                first = false;
            }
            out.println();
            out.println("])");
        }
        
        return objectsVariable;
    }
}
