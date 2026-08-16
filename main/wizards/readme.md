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

# wizards — Bazel migration

`main/wizards` is not one deliverable but three, sharing only a directory.
`Module_wizards.mk` builds ten Java jars, fifteen BASIC library zips and seven
resource bundles, and nothing in the module links or includes anything from
another part of it.  The Bazel file is organised the same way.

This is the first module whose output is reached almost entirely through the
**Java UNO loader** — everything here is data or bytecode, and there is not one
line of C++.  It depends on the whole Java bucket landing first (`stoc`
javaloader/javavm, `jvmfwk` + `sunjavaplugin`, the five runtime jars); with that
in place the module itself is small.

## What it delivers

`File > Wizards` — Letter, Fax, Agenda, Web Page — plus the Base wizards (Form,
Query, Report, Table) reached from a database document, plus the BASIC macro
libraries under `Tools > Macros` (Euro Converter, Import Wizard, Depot,
Gimmicks, Tutorials, …).

The menu entries were already there and already dead: `officecfg`'s
`Common.xcu` has dispatched `service:com.sun.star.wizards.letter.CallWizard?start`
since the demo baseline, at a service nothing implemented.  The eight
registrations added to `//main/postprocess` are what those URLs finally resolve
to.

## 1. Java jars

Ten `uno_jar`s, via the `wizard_jar` macro in `wizards.bzl` (the nine component
recipes are the same shape nine times; only the package, the registration class
and the class path differ).

`commonwizards.jar` is the shared half — the UNO dialog framework (`ui/`),
database metadata (`db/`), document manipulation (`document/`, `text/`) and
`common/`.  The other nine compile against it.

**The globs are exact.**  Every `Jar_*.mk` lists its sources explicitly; each
glob was checked against that list rather than trusted.  `commonwizards` is 87
files across five packages, and the per-wizard counts (agenda 10, fax 8, form
10, letter 11, query 4, report 14, table 9, web 54, reportbuilder 14) match the
makefiles one for one.  Nothing here is over- or under-collected, so a future
source addition lands automatically and a mismatch would be a real signal.

### The manifest is the whole interface

A wizard jar is inert without three manifest attributes, and one of them is a
trap:

- `RegistrationClassName` — javaloader has no other way to find the class to
  register.  This is the reason these need `uno_jar` and not a plain
  `java_library`.
- `Class-Path: commonwizards.jar` — a RELATIVE entry, resolved by
  `URLClassLoader` against the jar's own URL.  That is why all nine must land in
  the same `program/classes/` directory, not merely somewhere on a classpath.
- `UNO-Type-Path:` — **must be present and must be empty.**  Absent is not the
  same as empty: `UnoClassLoader.getClassLoader()` (ridljar unoloader)
  substitutes `"<>"` when the header is missing, `"<>"` resolves to the jar's
  own URL, and it then `addURL()`s that into the **shared** UnoClassLoader —
  permanently hoisting a wizard jar into the global UNO type path.  An empty
  value walks a zero-length path and adds nothing, which is correct here: these
  jars carry implementations, not UNO types.  (`dp_component.cxx` reads the same
  header for the same meaning on the extension path, recording the jar as a
  java type library in `unorc` when it is absent.)

  Mechanically this needs a trailing space in the Starlark string —
  `"UNO-Type-Path: "` — because singlejar splits the line on `": "`.  Verified
  in the built jars: the emitted manifests are byte-equivalent to upstream's
  checked-in `MANIFEST.MF` files.

`java_uno.jar` is deliberately **not** a compile dep although the dmake
`JARFILES` lists it — it is a runtime dependency of the JNI bridge, not a
compile dependency of this source (same call as `//main/cppuhelper`'s
propertysetmixin supplier).

### reportbuilderwizard.jar

The one jar that is not a UNO component: `report.jar`'s `ReportWizard`
instantiates its layouters reflectively, so it has no `.component`, no
`RegistrationClassName` and nothing in `services.rdb`.  That reflective edge is
also why `report.jar` does not compile-depend on it — the dependency reads both
ways but there is no cycle.

It is built but **not staged**: it ships inside the `reportbuilder` `.oxt`,
which is still ⬜ (JFreeReport is not on Maven).  Building it now is what keeps
it from rotting until that bucket lands.

`commonwizards`'s `Class-Path: saxon9.jar` is kept verbatim for the same reason
— saxon is still ⬜, and a Class-Path entry naming a jar that is not there is
silently ignored, so it costs nothing now and needs no second edit later.

## 2. BASIC libraries

Upstream ships each library as a `.zip` that the MSI unpacks into
`share/basic/<Name>/`.  There is no MSI here, so **the directory is the
deliverable and the zip step has nothing to port** — `tree_install` stages the
files directly.

The installed `<Name>` is scp2's `DosName`, not the source directory name, and
`configshare/script.xlc` spells the hrefs out
(`$(INST)/share/basic/FormWizard/script.xlb/`).  So `formwizard`→`FormWizard`
and `importwizard`→`ImportWizard` are corrections, not cosmetics.

The per-directory globs are restricted to `*.xba`/`*.xdl`/`*.xlb`.  That filter
is load-bearing: five of these directories also hold the `.src` file that feeds
section 3, and a bare glob would ship `dbwizres.src` into `share/basic/`.  Each
glob was checked against its `Zip_*.mk` list and matches exactly.

**Built but installed by nothing:** `basicsrvweb` and `basicsrvlauncher`.
`file_ooo.scp` has no `File` entry for either, and neither appears in
`script.xlc` — WebWizard even has an scp2 directory (`gid_Dir_Basic_Webwiz`)
with no file to put in it.  They are kept as targets so `bazel build
//main/wizards/...` covers what dmake covers, and deliberately left out of
staging.  The WebWizard *feature* is not lost with them: it is the Java
`web.jar`, a different implementation entirely.

`ScriptBindingLibrary`, the tenth entry in the shared index, is
`//main/scripting`'s (scp2 builds it from `workben/bindings`) and is staged from
there so the index has no dangling entry.  Its file list is exactly upstream's
`ZIP5LIST` — narrower than the directory.  Note `dialog.xlb` declares two
elements while upstream ships only `Highlight.xdl`, so expanding that library in
the BASIC IDE looks for a dialog that was never installed.  That is upstream's,
reproduced rather than quietly repaired.

## 3. Resource bundles

Seven `rsc_res` targets, one per `AllLangResTarget_*.mk`.  None of the `.src`
files `#include`s anything, so there are no `hdrs` and no `includes`.

The bundle **name is the lookup key at runtime**, not a filename convention —
it is the second argument to `InitResources()` in BASIC
(`Tools/Misc.xba`) and the third to `new Resource()` in Java
(`common/Resource.java` → `VclStringResourceLoader`).  So the seven cannot be
merged, and each gets its own `program/resource/<name>en-US.res`:

| bundle | source | consumer |
| --- | --- | --- |
| `cal` | `schedule/schedule.src` | Schedule BASIC library |
| `dbw` | `formwizard/dbwizres.src` | **all** the Java wizards, not just the database ones |
| `eur` | `euro/euro.src` | Euro Converter |
| `imp` | `importwizard/importwi.src` | Import Wizard (BASIC and `common/FileAccess.java`) |
| `tpl` | `template/template.src` | Template/Correspondence |
| `wwz` | `webwizard/webwizar.src` | WebWizard BASIC library |
| `wzi` | `imagelists/imagelists.src` | Images, not strings |

`dbw` is the big one (3,474 lines) and the name undersells it: agenda, fax,
letter and web all resolve `"dbw"` (see `AgendaWizardDialogResources.MODULE_NAME`).

A `com` bundle is also referenced — `InitResources("Tools", "com")` in
`Tools/Misc.xba` — and **does not exist anywhere in the tree**, upstream
included.  It guards an error-message lookup, so the branch is simply dead.  Not
a migration gap; do not go looking for it.

## Known defect found, not fixed: the wizard thumbnails will be blank

`wzi` holds the report/form/web wizards' orientation and layout thumbnails,
requested as `private:resource/wzi/image/<id>`.  They will not load.  The cause
was traced end to end and is **not** the usual `images_root` mistake:

1. `imagelists.src` writes every reference as the one-line form
   `ImageBitmap = Bitmap { File = "portrait_32.png"; };`.
2. rsc2 rewrites a `File=` line into a path-qualified one only when the line
   holds a **single** `'='` — `rsc.cxx PreprocessSrsFile` guards on
   `GetTokenCount('=') == 2`.  Two `'='` here, so the rewrite is skipped, the
   `.res` keeps the bare basename, and nothing is written to the image list
   either.
3. `private:resource/wzi/image/<id>` → GraphicProvider → `Image(ResId)` →
   `ImplImageTree::loadImage` → `hasByName()`, an **exact** key lookup.
4. `images.zip` is keyed by path (`wizards/res/portrait_32.png`) and has **zero**
   top-level entries — here and upstream, whose `images.zip` is built by
   `packimages.pl` from `.ilst` files that step 2 never feeds.

So the bare name never matches.  This is **tree-wide and upstream's**: 1,033
references take that one-line shape across 19 modules (svx 307, sfx2 113, sd
105, cui 91, sw 87, sc 85 …), of which wizards contributes 30.  Modules whose
`.src` puts `File =` on its own line (vcl, framework) are unaffected, which is
why the Start Center fix in `MEMORY.md` worked.

It is left alone deliberately.  Repairing it means either editing 1,033 source
lines or changing what `images.zip` is keyed by for every module — a product
change, not a migration one, and not something to slip into a module port.
`images_root` is still set to the standard `"main/default_images"` so rsc2 finds
the files and the staging layout stays consistent with every other module; it is
simply inert for the stored name here.

## Staging

| artifact | destination | why that exact place |
| --- | --- | --- |
| 9 jars | `program/classes/` | `services.rdb` names `$OOO_BASE_DIR/program/classes/<x>.jar`, and the relative `Class-Path` needs `commonwizards.jar` beside them |
| 7 `.res` | `program/resource/<name>en-US.res` | `ResMgrContainer::init()` scans that dir |
| 9 BASIC libs | `share/basic/<DosName>/` | `script.xlc`'s hrefs |
| 2 `.xlc` | `share/basic/` | the container index |
| `config/`, `standard/` | `presets/basic/` | pre-existing; the user-profile seed |

## Divergences from dmake, in one place

- No zip step for the BASIC libraries (no installer to unpack them).
- `Sealed:` is not reproduced — same reason as the other `uno_jar` consumers.
- `basicsrvweb` / `basicsrvlauncher` built but not staged (upstream installs
  neither).
- `reportbuilderwizard.jar` built but not staged (belongs to the deferred
  `.oxt`).
- The wizard thumbnails are blank, as upstream — see above.
