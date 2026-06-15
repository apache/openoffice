# Bug report — Writer document fails to survive load at startup; office self-terminates, and queued async events then crash (use-after-free)

**Component:** `ext_libraries/icu` data packaging (true root cause); `i18npool` break iterator (throw site); `desktop`/`sfx2`/`vcl` (downstream self-terminate + crash sites)
**Root cause (CONFIRMED):** the staged `icudata.dll` is ICU's **stub** (empty) and the real `icudt49l.dat` is never staged/loaded, so ICU has **no break-iterator data**. The first text layout during the Writer load calls `BreakIterator_Unicode::loadICUBreakIterator`, ICU's `createLineInstance` fails for lack of data, and it `throw RuntimeException()` (empty message). That escapes the document load → the frame is closed → `DispatchWatcher` sees no open frame → `Desktop::terminate()` → `SfxApplication` is deleted → already-posted async user-events race the teardown and AV.
**Type:** Missing ICU runtime data → load failure → startup self-termination → use-after-free (async timing decides *which* AV you see)
**Severity:** High — Writer never opens; office exits or AV-crashes on every launch.
**Status:** **Root-caused and confirmed** (cdb throw-stack walk + ICU build inspection). Fix is **staging-only, ICU unchanged**: stage the prebuilt `icudt49l.dat` and point ICU at it via `ICU_DATA` (§9). The sidebar-hidden workaround (§8) only suppresses one downstream AV signature and is now secondary.

---

## 1. Summary

**The full chain, innermost cause first:**

1. **ICU has no runtime data.** The staged `icudata.dll` is built from `stubdata.c`
   ([ext_libraries icu overlay/BUILD.bazel:277-278](ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel#L278))
   — an empty placeholder — and the real `icudt49l.dat` is neither baked into it nor staged, and nothing
   calls `udata_setCommonData`/`u_setDataDirectory`. So ICU's `brkitr` (break-iterator) data is unavailable.
2. **The Writer load needs a line break iterator.** During the load, the first real text layout calls
   `BreakIterator_Unicode::loadICUBreakIterator`
   ([breakiterator_unicode.cxx:95](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L95)).
   The custom `OpenOffice_dat` package has no `line` rule (intentionally omitted — see §5.1), so it falls
   back to ICU's own `icu::BreakIterator::createLineInstance` ([line 178](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L178)),
   which **also** fails for lack of data → `throw RuntimeException()` (empty message)
   ([line 183](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L183)).
3. **The load aborts and closes the document.** That exception escapes `SfxFrameLoader_Impl::load`'s
   `catch` ([frmload.cxx:632](main/sfx2/source/view/frmload.cxx#L632)); with `bLoadSuccess == false` the
   loader closes the model ([frmload.cxx:645](main/sfx2/source/view/frmload.cxx#L645)) — **no frame survives.**
4. **The office self-terminates,** and posted async events race the teardown and AV (the originally-reported
   crashes). Mechanism below.

Why the office *starts* fine but Writer dies: AOO uses its own `localedata_*.dll` for locale formatting
(not ICU data), and break iterators load **on demand**
([line 94](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L94)) — Start Center never needs
word/line/sentence breaking, so the empty ICU data is first hit only when a document lays out text.

### The self-terminate + async-AV mechanism (downstream of the load failure)

`soffice -writer` dispatches `private:factory/swriter` **synchronously**
([dispatchwatcher.cxx:333](main/desktop/source/app/dispatchwatcher.cxx#L333)). When the dispatch
returns (with the doc already closed by step 3),
[DispatchWatcher::executeDispatchRequests](main/desktop/source/app/dispatchwatcher.cxx#L432-L453)
asks the Desktop for its open frames — **the list is empty** — and therefore shuts the office down:

```cpp
bool bEmpty = (m_nRequestCount == 0);
if ( bEmpty && !bNoTerminate )
{
    Reference< XFramesSupplier > xTasksSupplier( xDesktop, UNO_QUERY );
    Reference< XElementAccess > xList( xTasksSupplier->getFrames(), UNO_QUERY );
    if ( !xList->hasElements() )                 // ← NO open frames
    {
        Reference< XDesktop > xDesktop2( xTasksSupplier, UNO_QUERY );
        if ( xDesktop2.is() )
            return xDesktop2->terminate();        // ← the teardown
    }
}
```

So **the Writer document does not survive its own load** — no frame is left for the office to show.

`Desktop::terminate()` runs the SFX terminate listener, whose `notifyTermination` does
`delete pApp` ([appinit.cxx:148](main/sfx2/source/appl/appinit.cxx#L148)) → `~SfxApplication`
([app.cxx:379-383](main/sfx2/source/appl/app.cxx#L379)) → `Deinitialize()`
([appquit.cxx:90](main/sfx2/source/appl/appquit.cxx#L90)) → `delete pSlotPool`, whose destructor
`delete`s every `SfxInterface` ([msgpool.cxx:91-92](main/sfx2/source/control/msgpool.cxx#L91)). The
process-static `Class::pInterface` pointers behind the `SFX_IMPL_INTERFACE` macro are **left dangling**
(never reset to 0).

The office is mid-`Application::Yield`, and one or more **already-posted async user-events** still sit
in the VCL event queue. They fire in the same loop and **race the teardown** — that is where the visible
crash comes from. Which crash you get depends on what was queued:

| Build | Queued event that fires after terminate | Crash signature |
|---|---|---|
| sidebar **on** | `SidebarController::UpdateConfigurations` (posted) | **Sig. B** — `vcl!Window::ImplInsertWindow` AV: sidebar TabBar parents a tab button into the **freed frame window** |
| sidebar **off** | `SfxDispatcher::DispatcherUpdate_Impl` (posted via `svtools::AsynchronLink`) | **Sig. A** — `sfx!SfxInterface::Register` AV: `SfxGetpApp()->GetOrCreate()` **re-creates** `SfxApplication`; `GetStaticInterface()` returns the **dangling** static interface |

**Both AVs are the same defect**, separated only by which posted event wins the race after the office
has decided to terminate. Hiding the sidebar (§8) removed event B but exposed event A; it did **not**
make Writer open. This supersedes the earlier framing of this file (which treated the sidebar UAF as
*the* bug — it is a symptom).

---

## 2. Environment

| | |
|---|---|
| Product | Apache OpenOffice (Bazel-migration branch `bazel-migration`) |
| OS | Windows 11 (10.0.26200), x86 / 32-bit process |
| Toolchain | MSVC VS2008, **debug CRT** (`MSVCR90D.dll`, `MSVCP90D.dll`) |
| Entry binary | `soffice.exe` (loader) → `sofficeapp.dll` (in-process) |
| Reference | Installed **OpenOffice 4** (release build) — opens Writer correctly |
| Debugger | cdb / WinDbg 10.0.26100.7705 |

---

## 3. Steps to reproduce

```
soffice.exe -env:UserInstallation=file:///C:/temp/ooo_nosyncN -norestore -writer
```

Fresh user profile, no extensions. Start Center is unaffected (it never dispatches a document factory and
has no sfx2 Sidebar). The failure is specific to opening a document frame.

### Observed
- Office **self-terminates** during startup (no Writer window), or AVs in cdb (sig. A or B).

### Expected
- A blank, interactive Writer document window that stays open.

---

## 4. Symptom matrix (one defect, several faces)

| How run | Result | Why |
|---|---|---|
| sidebar on, normal launch | Hang → hard-kill | Posted `UpdateConfigurations` reads freed frame → corrupt window tree → deadlock |
| sidebar on, under cdb | AV in `vcl!Window::ImplInsertWindow` (sig. B) | Debug heap poisons freed frame → immediate fault |
| sidebar off, under cdb | AV in `sfx!SfxInterface::Register` (sig. A) | Posted `DispatcherUpdate` re-creates `SfxApplication`; dangling static `SfxInterface` |
| sidebar off, normal launch | Office exits with no window | `getFrames()` empty → `Desktop::terminate()`; the async race may or may not crash first |
| under **TTD** | exit code 0 after ~150 s | TTD perturbs timing so a posted event runs while state is still alive; masks the race, not the root |
| installed **OO4** reference | Works | Different binary; its `swriter` load produces a surviving frame |

The poison value seen at the AV varies by run (`0xFEEEFEEE` OS-heap free-fill vs `0xDDDDDDDD` debug-CRT
`delete`-fill) — consistent with reading **freed** memory whose exact filler depends on the allocator path.

---

## 5. Root cause

### 5.1 The disease: ICU has no break-iterator data → the line break iterator throws

ICU's data is missing at runtime:

- `icudata.dll` is built from `source/stubdata/stubdata.c`
  ([icu overlay/BUILD.bazel:277-294](ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel#L276)) — ICU's
  **stub** (zero resources). The real `icudt49l.dat` exists in the tree
  (`source/data/in/icudt49l.dat`, filegroup `icudt49l_dat`) but is used only as a **build-tool input**
  (`-d` for `genbrk`), never baked into the DLL or staged.
- Nothing makes the data available at runtime: a grep of `main/staging` finds no `icudt49l.dat`, no
  `ICU_DATA`, no `udata_setCommonData`; AOO never calls `u_setDataDirectory` (it normally relies on the
  *real* data DLL's entry point, which here is the stub).

So when the Writer load triggers the first text layout, `BreakIterator_Unicode::loadICUBreakIterator`
([breakiterator_unicode.cxx:95-194](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L95))
runs for `LOAD_LINE_BREAKITERATOR`:

- The custom `OpenOffice_dat` package (compiled into `i18npool` from the `*.txt` rules) has **no `line`
  rule** — `line.txt`/`sent.txt` were deliberately omitted in the migration
  ([i18npool/pool/BUILD.bazel:86-90](main/i18npool/pool/BUILD.bazel#L86)) on the assumption that ICU's own
  line break would serve as a fallback.
- The code *does* have that fallback — `icu::BreakIterator::createLineInstance`
  ([line 178](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L178)) — **but it also fails**
  (`!U_SUCCESS(status)`) because ICU has no `brkitr` data, so the function does
  `throw RuntimeException()` ([line 183](main/i18npool/source/breakiterator/breakiterator_unicode.cxx#L183)).
  `#define ERROR ::com::sun::star::uno::RuntimeException()` — an **empty-message** RuntimeException,
  exactly the type/message we decoded at the loader's catch.

So the omission of `line.txt` is harmless *on its own*; the killer is that the ICU fallback it relies on
has no data either. Either source of data (custom OOo `line` rule **or** ICU `brkitr` data) would prevent
the throw; the migration removed the first and never wired up the second.

**Confirmed**, not inferred: the captured fatal throw stack is
`i18npool!BreakIterator_Unicode::loadICUBreakIterator+0x87e` ← `getLineBreak` ← `OutputDevice::ImplGetTextLines`
← `MessBox` layout ← `ErrorHandler::HandleError` ← `sb!SfxLibraryContainer::init` (Appendix A.4), and it is
the **last first-chance C++ exception before `SfxFrameLoader_Impl::load`'s catch** (`getCaughtException`
at `load+0xa7e`). Every other throw in the trace (encryption check, basic-library probing, ucb folder
probing) is caught internally and benign.

### 5.1a The load aborts and closes the document

`SfxFrameLoader_Impl::load` ([frmload.cxx:577-653](main/sfx2/source/view/frmload.cxx#L577)) wraps view
creation + `connectController` in a `try`; the RuntimeException escapes `impl_createDocumentView` (which
has no internal `catch`), so `bLoadSuccess` stays false, the `catch` runs
`impl_handleCaughtError_nothrow`, and then `xCloseable->close(sal_True)` closes the model
([frmload.cxx:640-645](main/sfx2/source/view/frmload.cxx#L645)). `SwXTextDocument::close` →
`SfxBaseModel::close` tears the frame down → `getFrames()` is now empty.

`sw.dll`, `svx.dll`, `swd.dll` all load and the SwView is built before this — the frame is **created then
disposed**, which is why the earlier traces looked like "frame torn down during construction."

### 5.2 The teardown path (terminate → freed SFX statics)

```
sfx!SfxTerminateListener_Impl::notifyTermination          ; delete pApp  (appinit.cxx:148)
fwk!framework::Desktop::terminate
sofficeapp!desktop::DispatchWatcher::executeDispatchRequests   ; getFrames() empty → terminate
sofficeapp!desktop::OfficeIPCThread::ExecuteCmdLineRequests
sofficeapp!desktop::Desktop::OpenDefault
sofficeapp!desktop::Desktop::OpenClients
... vcl!Application::Yield ...                             ; first message-loop yield
```

`delete pApp` → `~SfxApplication` → `Deinitialize()` → `delete pSlotPool` →
`SfxSlotPool::~SfxSlotPool` runs `for (pIF = FirstInterface(); pIF; ...) delete pIF;`
([msgpool.cxx:91-92](main/sfx2/source/control/msgpool.cxx#L91)). Each `~SfxInterface`
([objface.cxx:304-317](main/sfx2/source/control/objface.cxx#L304)) does `delete pImpData` (debug-CRT
poison-fills it `0xDD`). `~SfxApplication` sets `pApp = 0`. **But the per-class process-static
`Class::pInterface` is never reset** — `~SfxInterface` has no access to it; the static lives in the
`SFX_IMPL_INTERFACE` macro ([shell.hxx:341-358](main/sfx2/inc/sfx2/shell.hxx#L341)).

### 5.3 Signature A — `SfxInterface::Register` AV (sidebar off)

A `DispatcherUpdate_Impl` posted earlier via `svtools::AsynchronLink` fires in the same `Yield`,
**after** `pApp` was deleted:

```
sfx!SfxInterface::Register+0xd          ; mov byte ptr [ecx+3Ch],1 ; ecx = pImpData = 0xDDDDDDDD
sfx!SfxApplication::RegisterInterface
sfx!SfxApplication::Registrations_Impl
sfx!SfxApplication::Initialize_Impl
sfx!SfxApplication::GetOrCreate         ; pApp == 0 → builds a NEW SfxApplication
sfx!SfxGetpApp
sfx!SfxDispatcher::Update_Impl
sfx!DispatcherUpdate_Impl               ; posted async user-event
svt!svtools::AsynchronLink::Call_Impl
vcl!ImplHandleUserEvent ... vcl!Application::Execute
```

`GetOrCreate()` ([app.cxx:287](main/sfx2/source/appl/app.cxx#L287)) sees `pApp == 0` and builds a fresh
`SfxApplication` → `Initialize_Impl` → `Registrations_Impl` → `SfxApplication::RegisterInterface()` →
`GetStaticInterface()`. Because `SfxApplication::pInterface` is **non-NULL but dangling**,
`GetStaticInterface()` returns it *without* re-allocating
([shell.hxx:345-358](main/sfx2/inc/sfx2/shell.hxx#L345)); `Register()` then writes
`pImpData->bRegistered = sal_True` ([objface.cxx:147-150](main/sfx2/source/control/objface.cxx#L147))
through the freed impl → AV, `ecx = 0xDDDDDDDD`.

> The `SFX_IMPL_INTERFACE` static-interface design assumes `SfxApplication` is a true **process singleton
> that is never destroyed and recreated**. A first-ever clean `GetOrCreate` allocates fresh impls and does
> not crash; the AV is therefore positive proof that a **destroy-then-recreate** of `SfxApplication`
> happened — i.e. that §5.1's terminate ran.

### 5.4 Signature B — `vcl!Window::ImplInsertWindow` AV (sidebar on)

With the sidebar visible, the queued event is `SidebarController::UpdateConfigurations`, which builds deck
tab buttons (`ImageRadioButton`s). `Window::ImplInsertWindow`
([window.cxx:1039-1051](main/vcl/source/window/window.cxx#L1039)) dereferences the parent TabBar's
**frame window** (`mpWindowImpl->mpFrameWindow`) — already freed by the same terminate → AV.

```
vcl!Window::ImplInsertWindow            ; deref freed frame window
vcl!RadioButton::ImplInit
sfx!sfx2::sidebar::TabItem::TabItem
sfx!sfx2::sidebar::ControlFactory::CreateTabItem
sfx!sfx2::sidebar::TabBar::SetDecks
sfx!sfx2::sidebar::SidebarController::UpdateConfigurations
sfx!sfx2::sidebar::AsynchronousCall::HandleUserCall    ; posted async user-event
```

---

## 6. Evidence

**The root is a staging gap (ICU data):**
1. `icudata.dll` links `stubdata.c` ([§5.1](#51-the-disease-icu-has-no-break-iterator-data--the-line-break-iterator-throws)); no `icudt49l.dat`/`ICU_DATA`/`udata_setCommonData` anywhere in staging or source.
2. The captured fatal throw is `loadICUBreakIterator` for the **line** break (Appendix A.4) — the on-demand
   data consumer — which is why **Start Center works but Writer dies** (break iterators load lazily;
   Start Center needs none).
3. The caught exception is a `com.sun.star.uno.RuntimeException` with an **empty message** — exactly
   `#define ERROR RuntimeException()` at the throw site, decoded from the `Any` at the loader's catch.

**The AV crashes themselves are a latent race downstream of the load failure** (so they vary run-to-run):
4. **The terminate is deliberate and reached cleanly** — captured `kb` shows `Desktop::terminate` from
   `DispatchWatcher::executeDispatchRequests` after a `getFrames()` check, no exception unwinding.
5. **Sig. A proves a destroy+recreate of `SfxApplication`** (§5.3) — impossible on a clean first init;
   only the terminate path frees those statics.
6. **Nondeterministic poison value** (`0xFEEEFEEE` vs `0xDDDDDDDD`) and **no repro under TTD** — classic
   freed-memory race (Heisenbug); the *root* (missing data) is deterministic, the *AV* is the race.

---

## 7. Ruled out: sidebar-visibility config mismatch

The staging does not wrongly enable the sidebar — the default is the standard upstream value in
[Views.xcu:33-38](main/officecfg/registry/data/org/openoffice/Office/Views.xcu#L33-L38)
(`SID_SIDEBAR` = 10336, `Visible=true`), identical to the reference release. Not a staging gap. (The
sidebar is merely *one* async event that races the teardown — see §8.)

---

## 8. Workaround status — sidebar-hidden is NOT sufficient

### 8.1 What the sidebar-hidden patch does (and doesn't)

The build-layer patch flips `SID_SIDEBAR` (10336) `Visible` true→false at stage time so the
`SidebarController` (and its async `UpdateConfigurations`) is never constructed:

- [main/postprocess/BUILD.bazel](main/postprocess/BUILD.bazel): genrule `sidebar_off_views_xcu` rewrites
  `Office/Views.xcu`; `main_xcd` packs the patched copy. Mirrors `forcedefault_linguistic_xcu`.

This removes **signature B** only. Because the underlying defect (§5.1, ICU has no break data → the Writer
load throws and closes the doc) is unchanged, the office **still self-terminates**, and the queued
`DispatcherUpdate` now produces **signature A** instead. **Net: hiding the sidebar does not yield an
interactive Writer — only the ICU-data fix (§9) does.** Keep the patch (it removes one crash and is
harmless), but do not treat it as the fix.

### 8.2 Use a fresh `UserInstallation`

`main.xcd` defaults only seed a *new* profile, and a fresh path drops stale `.lock` /
`registrymodifications.xcu` from prior hung runs. Do not hand-edit
`registrymodifications.xcu` (PowerShell writes UTF-16 LE BOM, which configmgr rejects as a profile-access
error).

---

## 9. Fix — build `icudata.dll` with the REAL data (no stub) — IMPLEMENTED

Chosen approach: stop shipping a stub `icudata.dll`; bake the prebuilt `icudt49l.dat` into it so ICU loads
data from the DLL's entry point exactly as it does in a normal ICU build. No `ICU_DATA` env var, no
loose data file, no source change. Implemented entirely in the ICU overlay BUILD —
[ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel](ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel):

1. **`genrule :icudt49_dat_obj`** runs the in-tree `:genccode` tool to convert `icudt49l.dat` → a COFF
   object `icudt49_dat.obj`:
   ```
   genccode -o -e icudt49 -f icudt49_dat -d $(RULEDIR) source/data/in/icudt49l.dat
   ```
   - `-o` emits a COFF object directly (`pkg_genc.h` defines `CAN_GENERATE_OBJECTS`); with no `-m`
     match-arch file, `getArchitecture()` defaults to `IMAGE_FILE_MACHINE_I386` — correct for the x86 build.
   - `-e icudt49` → symbol `icudt49_dat` = `U_DEF2_ICUDATA_ENTRY_POINT(49,…)` = `U_ICUDATA_ENTRY_POINT`
     (the same symbol the stub exported).
   - `-f icudt49_dat` → predictable output name for the genrule `outs`.
2. **`cc_binary :icudata`** now links that object instead of `stubdata.c`, as a pure data DLL:
   `linkshared` + `/NOENTRY` (no DllMain/CRT, like ICU's own `icudt49.dll`) +
   `/EXPORT:icudt49_dat` (a hand-built COFF object has no `__declspec(dllexport)`, so the export is explicit).
   `icuuc.dll` keeps importing `icudt49_dat` via `:icudata_implib` — unchanged, just real content now.

The build **tools** (`genbrk`/`gencmn`/`genccode`) keep using the stub `:icudata_static` — they only need
enough ICU to run, and `icudt49_dat.obj` depends on `:genccode`, so the runtime data DLL must not feed back
into the tools (no cycle). Staging is unchanged: it already stages `:icudata`; that DLL now carries data.

> **CRITICAL — local-registry overlay hash.** `icu` is a `bazel_dep` from the local registry under
> `ext_libraries/`. Its `49.1.2/source.json` pins the overlay BUILD by SHA-256
> (`"overlay": { "BUILD.bazel": "sha256-…" }`). **Editing `overlay/BUILD.bazel` has NO effect until that
> hash is updated** — Bazel keeps the cached repo and silently builds the *old* rules (here: the stub,
> linking `stubdata.obj` → a 7680-byte `icudata.dll`). After any overlay edit:
> `openssl dgst -sha256 -binary overlay/BUILD.bazel | openssl base64`, prefix `sha256-`, write it into
> `source.json`, then `bazel build --lockfile_mode=refresh …`. **Verify the staged `icudata.dll` is ~18 MB,
> not 7680 bytes.** This gotcha hid the ICU fix for days; the symptom kept drifting ("BASIC error" →
> "main menu fails") but it was always the same `loadICUBreakIterator` throw reached via different
> first-start paths.

**Build & verify:**
```
bazel build //main/staging:install
bazel-bin\main\staging\program\soffice.exe -env:UserInstallation=file:///C:/temp/ooo_icu -norestore -writer
```
Expected: `loadICUBreakIterator` no longer throws → Writer window opens.

**If the link complains** (e.g. about a missing entry point because the toolchain still expects one): drop
`/NOENTRY` and instead add a tiny anchor TU (a one-line `DllMain` `.c`) to `:icudata` `srcs` alongside the
object — the CRT then supplies the entry point and the object supplies the export. Keep `/EXPORT:icudt49_dat`
either way.

### 9.3 Note the misleading source comment
[i18npool/pool/BUILD.bazel:86-90](main/i18npool/pool/BUILD.bazel#L86) states `line.txt`/`sent.txt` were
omitted because `loadICUBreakIterator` "falls back to ICU's own" — true only when ICU *has* data. With §9's
real-data DLL the comment is accurate again; until then it was the trail that hid this bug.

### 9.4 Downstream items (now secondary, not required for Writer to open)
Once ICU has data the load succeeds, no frame is closed, no terminate, no AV. The latent lifetime issues
remain *real* but are no longer triggered: (a) `SFX_IMPL_INTERFACE` statics aren't reset on
`SfxApplication` teardown (§5.3); (b) posted async events aren't cancelled on terminate (§5.4/§5.5). These
are source-level hardening, out of migration scope, and only bite if the office ever destroys+recreates
`SfxApplication` again.

---

## 10. Follow-on after the ICU fix — BASIC user-library seed (non-fatal)

Once ICU has data, the Writer load proceeds far enough to reveal a *separate*, pre-existing first-start
gap that the broken break iterator had been masking. The application BASIC library container reads
`$(user)/basic/script.xlc` ([namecont.cxx:763-776](main/basic/source/uno/namecont.cxx#L763)); on a fresh
profile the file is absent, `openFileRead` throws, and `nPass==0` raises `ERRCODE_IO_GENERAL` /
`ERRCTX_SFX_LOADBASIC` → dialog **"Error loading BASIC of document …/user/basic/script.xlc/ : General
input/output error."** It is **non-fatal** — the code continues past the dialog with an empty container
(`xInput` stays empty → parse skipped). This is the *same* `SfxLibraryContainer::init` path that appears in
Appendix A.4; the ICU break-iterator throw used to abort it before the dialog could even render.

**Root cause:** scp2 seeds `user/basic/` from `basicusr.zip` (`gid_File_Basic_User`, the `Standard`-only
`script.xlc`/`dialog.xlc`) and `share/basic/` from `basicshare.zip` — both produced by the **deferred
`wizards` module**, so neither is staged.

**Fix (staging):** the two `Standard`-only template files
([wizards/source/config/script.xlc + dialog.xlc](main/wizards/source/config/script.xlc)) are now staged
into `presets/basic/` via `//main/wizards:basic_user_config` + `tree_install :_install_presets_basic`
([staging/BUILD.bazel](main/staging/BUILD.bazel)). `UserInstall::finalize()` copies `presets/` →
`$UserInstallation/user/` on first launch, so `user/basic/script.xlc` exists and the read succeeds.
The `configshare` `share/basic/script.xlc` is deliberately **not** staged: it lists wizard libraries
(`Tools`, `Gimmicks`, …) that the deferred module doesn't build, and the share read (`nPass==1`) fails
silently anyway.

**Separate cosmetic item (not addressed here):** the error box title shows literal
`${PRODUCTNAME} ${PRODUCTVERSION}` — product branding substitution (`org.openoffice.Setup/Product/ooName`
etc., normally filled from the bootstrap/version config) isn't applied. Tracked separately from the crash.

---

## 11. Follow-on — unbuilt UNO reflection/introspection layer (the activation crash)

With ICU + BASIC + Standard-library all fixed, the Writer view builds fully (toolbars, layout, frame
**Show**) and then throws at the very last step — **view activation**:
```
type:  com.sun.star.loader.CannotActivateFactoryException
msg:   bootstrap.uno.dll: cannot get factory of demanded implementation:
       com.sun.star.comp.stoc.Introspection
sb!SbUnoObject::doIntrospection ← BasicManager::SetGlobalUNOConstant ("ThisComponent")
  ← SfxObjectShell::SetCurrentComponent ← SfxViewShell::Activate ← SwView::Activate
```
**Root cause:** the entire stoc UNO **reflection/introspection/invocation** layer was never compiled.
`stoc/BUILD.bazel` built only `bootstrap.uno` (bootstrap/registry/loader/tdmanager) and `stocservices.uno`
(typeconv/uri). Missing: `corereflection` (`CoreReflection`), `inspect` (`Introspection`), `invocation`
(`Invocation`), `invocation_adapterfactory` (`InvocationAdapterFactory`), `namingservice` (`NamingService`),
`proxy_factory` (`ProxyFactory`). The postprocess `_SERVICES_COMPONENTS` map *registered* all of them in
services.rdb pointing at `bootstrap.uno.dll` ("All bundled into bootstrap.uno.dll"), but that DLL never
contained their factories → `CannotActivateFactoryException`. They cannot be merged into `bootstrap.uno`
(each has its own `component_getFactory` → symbol collision).

**Fix (build-only, no source change):** build each as its own component DLL and point services.rdb at it.
- [stoc/BUILD.bazel](main/stoc/BUILD.bazel): six `cc_binary` targets (`reflection.uno`, `introspection.uno`,
  `invocation.uno`, `invocadapt.uno`, `namingservice.uno`, `proxyfac.uno`) over a `_REFL_COMPONENTS` map,
  sharing [util/component.def](main/stoc/util/component.def) (the 3 standard component exports).
- [postprocess/BUILD.bazel](main/postprocess/BUILD.bazel): each `.component` URI now points at its own DLL
  instead of `bootstrap.uno.dll` (java* stay on bootstrap — deferred, only loaded if Java is used).
- [staging/BUILD.bazel](main/staging/BUILD.bazel): stage the six new DLLs.

---

## Appendix A — raw debugger output

### A.1 Sig. A — `SfxInterface::Register` AV (sidebar off), with `kp`
```
sfx!SfxInterface::Register+0xd            ; mov byte ptr [ecx+3Ch],1   ds:002b:ddddde19=??  ; ecx=0xDDDDDDDD
sfx!SfxApplication::RegisterInterface(SfxModule* pMod = 0)+0x13
sfx!SfxApplication::Registrations_Impl+0xe
sfx!SfxApplication::Initialize_Impl+0x82d
sfx!SfxApplication::GetOrCreate+0xf3
sfx!SfxGetpApp+0x8
sfx!SfxDispatcher::Update_Impl(bForce=1)+0x5e
sfx!DispatcherUpdate_Impl+0xd
svt!svtools::AsynchronLink::Call_Impl+0x2a
vcl!ImplHandleUserEvent ... vcl!Application::Execute+0x2b
sofficeapp!desktop::Desktop::Main+0x2225
```

### A.2 The culprit — `Desktop::terminate` from DispatchWatcher (`kb` at `notifyTermination`)
```
sfx!SfxTerminateListener_Impl::notifyTermination
fwk!framework::Desktop::terminate+0x4c2
sofficeapp!desktop::DispatchWatcher::executeDispatchRequests+0x1d08    ; getFrames() empty → terminate
sofficeapp!desktop::OfficeIPCThread::ExecuteCmdLineRequests+0x260
sofficeapp!desktop::Desktop::OpenDefault+0x50b
sofficeapp!desktop::Desktop::OpenClients+0x158c
sofficeapp!desktop::Desktop::OpenClients_Impl+0x4b
... vcl!Application::Yield ... vcl!Application::Execute+0x2b
sofficeapp!desktop::Desktop::Main+0x2225
```
(`queryTermination` fires first, then `notifyTermination` → `delete pApp`.)

### A.3 Sig. B — `Window::ImplInsertWindow` AV (sidebar on)
```
vcl!Window::ImplInsertWindow+0x6f         ; mov edx,dword ptr [edx+8]  ; edx=0xDDDDDDDD (or 0xFEEEFEEE)
vcl!RadioButton::ImplInit
sfx!sfx2::sidebar::TabItem::TabItem+0x39
sfx!sfx2::sidebar::ControlFactory::CreateTabItem
sfx!sfx2::sidebar::TabBar::SetDecks
sfx!sfx2::sidebar::SidebarController::UpdateConfigurations
sfx!sfx2::sidebar::AsynchronousCall::HandleUserCall
```

### A.4 The TRUE root — fatal throw: ICU line break iterator has no data
The last first-chance C++ exception before `SfxFrameLoader_Impl::load`'s catch (`getCaughtException` at
`load+0xa7e`). `throw RuntimeException()` = empty-message — matches the `Any` decoded at the handler.
```
i18npool!BreakIterator_Unicode::loadICUBreakIterator+0x87e   ; throw ERROR (line 183): ICU createLineInstance failed — no data
i18npool!BreakIterator_Unicode::getLineBreak+0xa7
i18npool!BreakIteratorImpl::getLineBreak+0x8c
vcl!OutputDevice::ImplGetTextLines+0x3cb
vcl!OutputDevice::GetTextRect+0x114
vcl!MessBox::ImplPosControls+0x669            ; laying out an error message box...
vcl!Dialog::Execute+0x30
svt!aWndFunc+0x4d8
tl!ErrorHandler::HandleError+0x48             ; ...shown by Basic library container init
sb!basic::SfxLibraryContainer::init_Impl+0x116f
sb!basic::SfxLibraryContainer::init+0x28
  ... unwinds through SfxObjectShell::CheckSecurityOnLoading_Impl → SfxBaseModel::connectController
  → sw!SwXTextDocument::connectController → sfx!SfxFrameLoader_Impl::impl_createDocumentView+0x132
  → sfx!SfxFrameLoader_Impl::load (catch) → close(sal_True) → terminate → §5.3/§5.5 AV
```
ICU build proof: `icudata` `cc_binary` `srcs = ["source/stubdata/stubdata.c"]`
([icu overlay/BUILD.bazel:277](ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel#L277)); no
`icudt49l.dat` / `ICU_DATA` / `udata_setCommonData` staged or set.

---

# Follow-on findings — after Writer opens (2026-06-15)

Once the ICU fix let Writer open and accept input, three further issues surfaced while
testing **save** and a second app (Calc). Two are real migration/staging gaps (fixed,
build-only); one is a debug-CRT-only latent upstream defect (out of scope). All three were
root-caused with cdb against the **debug** build (`MSVCR90D`).

## 12. Save-As ODF dialog loops forever — empty filter UINames (FIXED)

**Component:** `filter/source/config` fragment merge (build); surfaces in `sfx2` save flow.
**Root cause (CONFIRMED):** every filter's `UIName` is **empty** in the staged
`share/registry/*.xcd`, because the Bazel `fcfg_merge` listed only the base
`filters/*.xcu` fragments and **dropped the separate `filters/<name>_ui.xcu` UIName
fragments**. (Type fragments embed `<prop oor:name="UIName">` inline, so types were fine;
filters keep the localized UIName in a sibling `_ui.xcu`.)
**Symptom:** in Writer, *File ▸ Save As ▸ ODF Text Document ▸ Save* does **not** write the
file — the picker just reappears, indefinitely. (Not a crash, not a hang.)

### 12.1 The loop mechanism (sfx2)

`SfxStoringHelper::Execute`'s inner `while(!bExit)`
([guisaveas.cxx:1497-1523](main/sfx2/source/doc/guisaveas.cxx#L1497)) only sets `bExit`
when `ModelData_Impl::CheckFilter` returns `STATUS_SAVE` (1). With an **empty** filter name,
`CheckFilter` ([guisaveas.cxx:716-768](main/sfx2/source/doc/guisaveas.cxx#L716)) returns
`STATUS_SAVEAS_STANDARDNAME` (3) → set `bSetStandardName`, re-show → repeat forever
(`STATUS_SAVEAS`=2 would loop too — neither 2 nor 3 is handled as an exit). `STATUS_*`
values: NO_ACTION 0 / SAVE 1 / SAVEAS 2 / SAVEAS_STANDARDNAME 3.

### 12.2 Where the empty name comes from (the file picker side)

`FileDialogHelper_Impl::getRealFilter`
([filedlghelper.cxx:1756-1768](main/sfx2/source/dialog/filedlghelper.cxx#L1756)):
```cpp
_rFilter = getCurrentFilterUIName();          // native Vista picker getCurrentFilter() → ""
if ( !_rFilter.Len() ) _rFilter = maCurFilter; // fallback = the filter's UIName
if ( _rFilter.Len() && mpMatcher ) {
    pFilter = mpMatcher->GetFilter4UIName( _rFilter, m_nMustFlags, m_nDontFlags );
    _rFilter = pFilter ? pFilter->GetFilterName() : _rFilter.Erase();   // NULL → ""
}
```
The native `fps_office` Vista picker returns `""` for `getCurrentFilter()` (it tracks the
filter by *index*; `impl_sta_getCurrentFilter`
[VistaFilePickerImpl.cxx:370](main/fpicker/source/win32/filepicker/VistaFilePickerImpl.cxx#L370)).
The fallback `maCurFilter` holds the filter **UIName** — but because the staged UIName is
empty, the value is just the `" (.ext)"` annotation, and
`SfxFilterMatcher::GetFilter4UIName(" (.odt)")` returns **NULL** → `.Erase()` → empty →
the loop.

### 12.3 cdb evidence
```
bp sfx!ModelData_Impl::CheckFilter
  du poi(poi(@esp+4))+8   → ""        ; aFilterName arg is EMPTY
  pt; r al                → al = 3    ; STATUS_SAVEAS_STANDARDNAME
bp sfx!sfx2::FileDialogHelper_Impl::getCurrentFilterUIName
  pt; du poi(@eax)+8      → ""        ; native picker reports no current filter
bp sfx!SfxFilterMatcher::GetFilter4UIName
  du poi(poi(@esp+4))+8   → " (.odt)" ; looked-up UI name = empty + annotation
  pt; r eax               → 0         ; NULL → .Erase()
```
Save-dialog "File type" dropdown showed `(.odt)` with **no name** — consistent with empty
UIName. Confirmed in `bazel-bin/.../share/registry/writer.xcd`: the `writer8` **filter**
node has Flags/Type/DocumentService but **no `UIName` prop**, while the `writer8` **type**
node does (`Writer 8`).

### 12.4 Fix (build-only, no source change)
- [build/tools/fcfg_merge.pl](build/tools/fcfg_merge.pl): new `ui` manifest section →
  builds an `oor:name → <prop oor:name="UIName">` map from the `_ui.xcu` fragments and
  injects each into the filter node with the matching `oor:name` (before `</node>`).
  Mirrors what upstream FCFGMerge does when folding `_ui` into the filter.
- [main/postprocess/postprocess.bzl](main/postprocess/postprocess.bzl): added a `ui`
  attribute to the `fcfg_merge` rule; threaded into the manifest + action inputs.
- [main/filter/source/config/fragments/BUILD.bazel](main/filter/source/config/fragments/BUILD.bazel):
  `_FILTER_UI = glob(["filters/*_ui.xcu"])` + `ui = _FILTER_UI` on all 18 `fcfg_*_filters`
  targets. Injection is `oor:name`-matched, so passing the full `_ui` set to every target
  is harmless.

**Verified:** after rebuild the dropdown reads "ODF Text Document (.odt)" and the document
saves as `.odt`.

## 13. Startup FatalError "Failed to update/lastsynchronized" (FIXED)

**Component:** brand bootstrap profiles (build); surfaces in `desktop` extension manager.
**Root cause (CONFIRMED):** the extension path macros (`BUNDLED_EXTENSIONS_USER`,
`BUNDLED_EXTENSIONS_PREREG`, `SHARED_EXTENSIONS_USER`, `UNO_*_PACKAGES_CACHE`,
`TMP/BAK_EXTENSIONS`) were defined **only in `uno.ini`**, not in `fundamental.ini`. The UNO
component-context bootstrap reads `uno.ini` (so the PackageManager could still create
`user/extensions/*`), but bare `rtl::Bootstrap::expandMacros()` resolves against the
**`URE_BOOTSTRAP` file = `fundamental.ini`**. So `writeLastModified()`
([dp_extensionmanager.cxx:116](main/desktop/source/deployment/manager/dp_extensionmanager.cxx#L116))
expanded `"$BUNDLED_EXTENSIONS_USER/lastsynchronized"` to the bare **`/lastsynchronized`**
→ invalid `ucbhelper::Content` URL → `DeploymentException("Failed to update" + url)` →
caught at [app.cxx](main/desktop/source/app/app.cxx) outer handler →
`FatalError(MakeStartupErrorMessage(...))`. Only seen when extension sync is **not**
disabled (`-env:DISABLE_EXTENSION_SYNCHRONIZATION=1` had been masking it).

### 13.1 cdb evidence
```
bp sofficeapp!desktop::FatalError
  du poi(poi(@esp+4))+8   → "The application cannot be started. \nFailed to update/lastsynchronized"
```
The URL tail is just `/lastsynchronized` — the leading macro expanded to empty. (Creating
`share/prereg/bundled` + `share/extensions` dirs did **not** help — red herring; the macro
itself was empty.)

### 13.2 Fix (build-only)
[main/staging/fundamental.ini](main/staging/fundamental.ini): added the extension path
macros as cross-refs to `uno.ini`, e.g.
`BUNDLED_EXTENSIONS_USER=${$ORIGIN/uno.ini:BUNDLED_EXTENSIONS_USER}` (+ `_PREREG`,
`BUNDLED_EXTENSIONS`, `SHARED_EXTENSIONS_USER`, `TMP/BAK_EXTENSIONS`, `UNO_*_PACKAGES_CACHE`).
Upstream propagates these `fundamentalrc → fundamentalbasisrc → unorc` (scp2
`common_brand.scp` gid_Brand_Profile_Fundamental_Ini items 999-1061); we have no
fundamentalbasis layer, so reference `uno.ini` directly. `$OOO_BASE_DIR`/`$ORIGIN` are
defined in `fundamental.ini`, so the imported values expand. Rejected alternative
(`DISABLE_EXTENSION_SYNCHRONIZATION=1`) — keeps sync working for future extension modules.
**Verified:** fatal gone; office boots into documents.

## 14. Calc crash-on-open AV — debug-CRT-only latent UAF (NOT a migration bug)

**Component:** `sc` view init — **stock AOO defect**, source byte-identical to upstream.
**Root cause (CONFIRMED):** `ScViewData::ReadUserDataSequence`
([viewdata.cxx:2821](main/sc/source/ui/view/viewdata.cxx#L2821)) does
`delete pTabData[nTab]; pTabData[nTab] = new ScViewDataTable;` per sheet but **never
refreshes `pThisTab`** (which pointed at `pTabData[nTabNo]`). Back in
`ScTabView::SetTabNo`, line 1660 reads `aViewData.GetActivePart()` → `pThisTab->eWhichActive`
**before** line 1663 fixes `pThisTab` → use-after-free → AV
`sc!ScTabView::SetTabNo` `mov ecx,[eax+edx*4+0x664]` with **edx=0xDDDDDDDD**
(`pGridWin[0xdddddddd]`).

**Why only in this build:** debug-CRT artifact, *not* a regression. Release allocator reuses
the just-freed same-size block, so the immediate `new` returns the **same** address and
`pThisTab` stays valid. The debug CRT (`MSVCR90D`) poison-fills freed blocks with `0xDD`
and **delays** their reuse, so `new` returns a *different* block → `pThisTab` dangles.

cdb proof (`.frame 0; dv /t` + `?? &this->aViewData`): `eOldActive = 0xDDDDDDDD`, but
`this`/`pDoc`/`pViewShell` all valid, and crucially
`pThisTab = 0x09a5f118` (old/low region, freed) **≠** `pTabData[nTabNo=0] = 0x13deca08`
(fresh/valid) — the new table object is fine; only the stale pointer dangles. **Contrast
with §1 (ICU):** there, release *also* failed (missing data = real staging gap); here
release opens Calc fine.

**Disposition:** out of migration scope ("source is not changed"). The 1-line upstream fix
would be `pThisTab = pTabData[nTabNo];` at the end of `ReadUserDataSequence`. **Triage rule:**
a debug-build `0xDD`/`0xFEEE` AV is a migration bug only if an upstream *failure* feeds it
(a throw, a missing staged file/data, as in §1); if every object is valid and only one
pointer dangles across a `delete`/`new`, it's a debug-CRT-exposed latent UAF → confirm by
opening the same document in the **release** build. Do feature testing in release.
