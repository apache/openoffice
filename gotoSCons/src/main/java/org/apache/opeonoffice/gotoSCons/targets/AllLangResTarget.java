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

package org.apache.opeonoffice.gotoSCons.targets;

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
 * gbuild's main/solenv/gbuild/AllLangResTarget.mk contains these 5 classes:
 * 1. SrsPartMergeTarget (used in 2 modules)
 * 2. SrsPartTarget (used in 2 modules)
 * 3. SrsTarget (used in many modules)
 * 4. ResTarget (unused in modules)
 * 5. AllLangResTarget (used in many modules)
 * 
 * 
 * ====================
 * 5. AllLangResTarget
 * ====================
 * This is just there to create a number of ResTarget objects,
 * one per language, and forward various calls to each one
 * of those (eg. add_srs() is called on every lang's ResTarget).
 * 
 * (extensions/pre_SharedLangList.mk)
 * if WITH_LANG == 'All':
 *     gb_WITH_LANG = $(completelangiso) from $(SOLARENV)/inc/langlist.mk
 * else:
 *     gb_WITH_LANG = $(WITH_LANG)
 * gb_AllLangResTarget_LANGS = unique union of (en-US, gb_WITH_LANG)
 * 
 * # UNUSED
 * #def gb_AllLangResTarget_set_langs(langs):
 * #    gb_AllLangResTarget_LANGS = langs
 * 
 * def gb_AllLangResTarget_get_target(name):
 *     return $(WORKDIR)/AllLangRes/$(name)
 * 
 * def gb_AllLangResTarget_AllLangResTarget(name):
 *     for lang in gb_AllLangResTarget_LANGS:
 *         gb_ResTarget_ResTarget($name$lang, name, lang)
 *     register the target gb_AllLangResTarget_get_target(name)
 * 
 * def gb_AllLangResTarget_add_srs(name, srs):
 *     for s in srs:
 *         gb_AllLangResTarget_add_one_srs(name, s)
 * 
 * def gb_AllLangResTarget_add_one_srs(name, srs):
 *     for lang in gb_AllLangResTarget_LANGS:
 *         gb_ResTarget_add_one_srs($(name)$(lang), srs)
 * 
 * def gb_AllLangResTarget_set_reslocation(name, location):
 *     for lang in gb_AllLangResTarget_LANGS:
 *         gb_ResTarget_set_reslocation($(name)$(lang), location)
 * 
 * def gb_AllLangResTarget_set_imagelocations(name, location):
 *     for lang in gb_AllLangResTarget_LANGS:
 *         gb_ResTarget_set_imagelocations($(name)$(lang), location)
 *
 * ============================
 * 4. ResTarget
 * ============================
 * Generates $(WORKDIR)/ResTarget/$(resName).res
 * from a number of .srs
 * 
 * def gb_ResTarget_get_target(resName):
 *     return $(WORKDIR)/ResTarget/$(resName).res
 * 
 * def gb_ResTarget_ResTarget(resName, allLangResName, lang):
 *     on gb_ResTarget_get_target(resName), set:
 *         RESLOCATION = allLangResName
 *         LIBRARY = allLangResName
 *         LANGUAGE = lang
 *     add dependencies and deliverables
 * 
 * def gb_ResTarget_add_file(resName, filename):
 *     gb_ResTarget_get_target(resName) depends on filename
 * 
 * def gb_ResTarget_add_one_srs(resName, srsName):
 *     gb_ResTarget_add_file(resName, gb_SrsTarget_get_target(srsName))
 * 
 * def gb_ResTarget_set_reslocation(resName, location):
 *     on gb_ResTarget_get_target(resName), set RESLOCATION=location
 * 
 * def gb_ResTarget_set_imagelocation(resName, location):
 *     on gb_ResTarget_get_target(resName), set IMAGELOCATIONS=location
 * 
 * A SUPER LONG COMPLEX RULE TO GENERATE ANY gb_ResTarget_get_target()
 * FROM ITS DEPENDENCIES USING rsc
 * 
 * =============================
 * 3. SrsTarget
 * =============================
 *  
 * 
 * def gb_SrsTarget_get_target(srsName):
 *     return $(WORKDIR)/SrsTarget/$(srsName).srs
 * 
 * gb_SrsTarget_DEFAULTDEFS = gb_GLOBALDEFS
 * 
 * def gb_SrsTarget_SrsTarget(srsName):
 *     on gb_SrsTarget_get_target(srsName), set:
 *         DEFS = $(gb_SrsTarget_DEFAULTDEFS)
 *         INCLUDE = $(SOLARINC)
 *         PARTS =
 *     some dependencies
 * 
 * def gb_SrsTarget_set_include(srsName, includes):
 *     on gb_SrsTarget_get_target(srsName) set INCLUDE = includes
 * 
 * def gb_SrsTarget_add_file(srsName, file):
 *     gb_SrsPartTarget_SrsPartTarget(file)
 *     on gb_SrsPartTarget_get_target(file), add file to PARTS, make us depend on it
 *     dependencies
 * 
 * # UNUSED
 * # def gb_SrsTarget_set_defs
 * 
 * RULE to generate gb_SrsTarget_get_target():
 *     cat together dependencies (file contents) into target file
 * 
 * ====================================
 * 2. SrsPartTarget
 * ====================================
 * 
 * def gb_SrsPartTarget_get_target(srsPartName):
 *     return $(WORKDIR)/SrsPartTarget/$(srsPartName)
 * 
 * def gb_SrsPartTarget_SrsPartTarget(srsPartName):
 *     if $(gb_WITH_LANG) is empty:
 *         on gb_SrsPartTarget_get_target(srsPartName), set MERGEDFILE=
 *     else:
 *         on gb_SrsPartTarget_get_target(srsPartName), set:
 *             MERGEDFILE = gb_SrsPartMergeTarget_get_target(srsPartName)
 *             depend on gb_SrsPartMergeTarget_get_target(srsPartName)
 *             on gb_SrsPartMergeTarget_get_target(srsPartName):
 *                 set SDF = $(realpath $(gb_SrsPartMergeTarget_SDFLOCATION)/$(dir $(1))localize.sdf)
 *                 make gb_SrsPartMergeTarget_get_target(srsPartName) depend on that location
 * 
 * RULE to generate gb_SrsPartTarget_get_target() from dependencies or MERGEFILE
 * which doesn't make sense as dependencies aren't added when MERGEFILE is unset
 * Perhaps gb_Helper_MISCDUMMY comes into play.
 * Unless $(eval $(call gb_SrsPartTarget__rules,$(repo))
 * $$(call gb_SrsPartTarget_get_target,%) : $(1)/% $$(gb_Helper_MISCDUMMY) | $$(gb_SrsPartTarget_RSCTARGET)
 * So it depends on everything in repo?
 * 
 * =====================
 * 1. SrsPartMergeTarget
 * =====================
 *
 * gb_SrsPartMergeTarget_get_target = $(WORKDIR)/SrsPartMergeTarget/$(1)
 * gb_SrsPartMergeTarget_SDFLOCATION := $(LOCDIR)/l10n/$(INPATH)/misc/sdf
 * 
 * runs rules on each repo
 * 
 * def gb_SrsPartMergeTarget__rules():
 *     if SDF is set:
 *         gb_SrsPartMergeTarget__command()
 *     else:
 *         copy sources to target
 * 
 * def gb_SrsPartMergeTarget__command():
 *     calls gb_SrsPartMergeTarget_TRANSEXCOMMAND on various args
 * 
 * 
 * 
 * 
 * 
 * NOTE: SCons' CPPSUFFIXES could scan .src files for dependencies
 */
public class AllLangResTarget extends BaseTarget {
    private File filename;
    private String name;

    public AllLangResTarget(File filename) throws Exception {
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
    protected void parseCall(Node argsNode) throws Exception {
        if (argsNode instanceof ValueNode) {
            String value = ((ValueNode)argsNode).value;
            String[] tokens = value.split(",");
            
            String function = tokens[0].trim();
            String[] args = Arrays.copyOfRange(tokens, 1, tokens.length);
            
            if (function.equals("gb_AllLangResTarget_AllLangResTarget")) {
                parseAllLangResTargetAllLangResTarget(args);
            } else {
                throw new Exception("UNHANDLED FUNCTION " + function);
            }
        } else {
            throw new Exception("Call args not a value");
        }
    }
    
    private void parseAllLangResTargetAllLangResTarget(String[] args) throws Exception {
        if (args.length != 1) {
            throw new Exception("Expected 2 arg, got " + Arrays.toString(args));
        }
        this.name = args[0];
    }
}
