<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# Localization / language builds (planned — not yet wired)

Current staging is en-US only.  This is the deliberate demo baseline; AOO must
start first.  When adding languages, keep THREE independent axes distinct
(conflating them is the trap) — dmake drives them from separate variables:

1. Installed locales (which UI languages are SELECTABLE) — dmake `alllangiso`/
   `WITH_LANG`, a LIST.  Each lang gets a Langpack-<lang>.xcd registering it in
   Setup/Office/InstalledLocales (+ fcfg_langpack_<lang>.xcd, registry_<lang>.xcd).
   Bazel today: only langpack_en_us_xcd.  Future: a macro over OOO_LANGS emitting
   one pack_registry per lang, each added to all_xcd via
   select({"//build:build_<code>": [...], "//conditions:default": []}) so
   --//build:lang_de=True auto-includes Langpack-de.xcd.  Mirrors the
   {$(alllangiso)} brace-expansion in postprocess/packregistry/makefile.mk.
2. Default UI language (the ONE start language) — dmake `PRODUCTLANGUAGE`, a
   SCALAR (installer/languages.pm $officestartlanguage; NOT the list).  This is
   the forcedefault.xcd ${PRODUCTLANGUAGE} substitution above.  Future: replace
   the hardcoded 'en-US' with a make-var, e.g. --define=office_start_lang=de
   (default en-US) read in the forcedefault_linguistic_xcu genrule cmd.
3. Localized content (the big lift) — per-lang .src/.res (rsc pipeline),
   localized .xcu (oc_xcu alllang spool), help, autotext, wordbook.  Same
   select()-per-lang pattern, but needs the TRANSLATION DATA wired in (see SDF).

Existing scaffolding (unconsumed): build/langs.bzl (OOO_LANGS, lang_id),
build/BUILD.bazel emits //build:lang_<code> bool_flags (en-US default True) and
//build:build_<code> config_settings.  user.bazelrc can flip lang_de/lang_fr;
nothing reads them yet.

## SDF files and the missing Pootle→SDF rule

Translation data flows as SDF (a.k.a. GSI) — the build's merge-database format,
one TAB-delimited line per translatable string
(project\path\file\type\gid\lid\helpid\platform\width\langid\text\...).

- Consumption (merge): l10ntools `transex`/export.cxx and friends read a source
  file + an SDF via `-m <sdf> -l <lang>` and emit the localized resource
  (the merge step before rsc compiles .src→.res, and for helpex/cfgex/xrmex).
  l10ntools localize.cxx does the reverse — EXTRACTS source strings into one
  merged .sdf (POT-equivalent) for translators.
- The gap: Pootle stores translations as .po (per lang/module).  AOO's build
  consumes .sdf, NOT .po.  The .po↔.sdf bridge is translate-toolkit's
  oo2po (sdf→po, to seed/update Pootle) and po2oo (po→sdf, to feed the build).
  Neither translate-toolkit nor any po/sdf conversion exists in this tree
  (grep: no oo2po/po2oo/po2sdf).  So axis 3 needs a NEW Bazel rule that runs
  po2oo over the Pootle .po export to (re)generate the per-lang .sdf the merge
  tools expect — this rule does not exist yet and is a prerequisite for any
  real (translated) language build.  Until then, language builds can only do
  axes 1+2 (German/French DEFAULT UI, but strings still English).
