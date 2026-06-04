# Bug report — Use-after-free in sfx2 Sidebar on document-frame startup

**Component:** `sfx2` (Sidebar) / `vcl` (Window)
**Crash site:** `vcl!Window::ImplInsertWindow` — [main/vcl/source/window/window.cxx:1048](main/vcl/source/window/window.cxx#L1048)
**Type:** Use-after-free (dangling VCL frame window), timing-dependent
**Severity:** High in debug builds (all document editing dead); latent/masked in release builds
**Status:** Root-caused; workaround available; fix is out of scope for the Bazel migration (no source changes)

---

## 1. Summary

When a document view frame (Writer/Calc/Impress/…) is created at startup, the sfx2
Sidebar's `SidebarController` posts an **asynchronous** `UpdateConfigurations` call
(via `Application::PostUserEvent`). That call later runs `TabBar::SetDecks`, which
constructs the deck tab buttons (`ImageRadioButton`s). During construction,
`Window::ImplInsertWindow` dereferences the parent TabBar's **frame window**
(`mpWindowImpl->mpFrameWindow`) — but by the time the async call fires, that
**document frame window has already been destroyed**. The result is a read from freed
heap memory.

- In a **debug** build the freed memory is poison-filled, so it faults immediately
  (clean AV under a debugger) or corrupts the window tree and **deadlocks** when run
  normally (symptom: "can't click anything, must hard-kill").
- In a **release** build the same latent bug is normally masked by timing (the
  reference installed OpenOffice 4 does not exhibit it).

This is a **latent lifetime race in AOO**, *exposed* by the debug build's timing and
heap poisoning — not introduced by the Bazel build, and not a missing/misconfigured
staged file.

---

## 2. Environment

| | |
|---|---|
| Product | Apache OpenOffice (Bazel-migration branch `bazel-migration`) |
| OS | Windows 11 (10.0.26200), x86 / 32-bit process |
| Toolchain | MSVC VS2008, **debug CRT** (`MSVCR90D.dll`, `MSVCP90D.dll`) |
| Entry binary | `soffice.exe` (7,680-byte loader) → `sofficeapp.dll` (in-process) |
| Reference | Installed **OpenOffice 4** (release build) — works correctly |
| Debugger | cdb / WinDbg 10.0.26100.7705; TTD (tttracer) |

---

## 3. Steps to reproduce

```
soffice.exe -env:UserInstallation=file:///C:/temp/ooo_nosync4 -norestore -writer
```

(fresh user profile, no extensions). The Start Center appears fine; opening any
document frame triggers the failure.

### Observed
- Document window paints (chrome/toolbars visible) but **does not respond to mouse or
  keyboard**; process must be hard-killed.
- **Start Center is unaffected** — it has no sfx2 Sidebar, so it never runs this path.

### Expected
- Document frame is fully interactive.

---

## 4. Symptom matrix (why it looks different under different tools)

| How run | Result | Why |
|---|---|---|
| Debug build, normal launch | **Hang** → hard-kill | Dangling read returns stale data → corrupt window tree → deadlock |
| Debug build under cdb | **Access violation** in `ImplInsertWindow` | Debug heap poisons freed memory → immediate fault |
| Debug build under cdb + full page heap | AV at freed page (decommitted) | Page heap confirms the block is freed |
| Debug build under **TTD** | **No crash** — `exit code 0` after 151 s | TTD perturbs timing → the race is not lost |
| Release build (reference install) | **Works** | Timing masks the latent race |

The nondeterminism across runs (see §6) is the defining evidence that this is a
**timing race**, not a deterministic build/data defect.

---

## 5. Root cause

### 5.1 The faulting code
[main/vcl/source/window/window.cxx:1039-1051](main/vcl/source/window/window.cxx#L1039-L1051):

```cpp
void Window::ImplInsertWindow( Window* pParent )
{
    mpWindowImpl->mpParent     = pParent;
    mpWindowImpl->mpRealParent = pParent;
    if ( pParent && !mpWindowImpl->mbFrame )
    {
        Window* pFrameParent = pParent->mpWindowImpl->mpFrameWindow;        // 1047
        mpWindowImpl->mpFrameData = pFrameParent->mpWindowImpl->mpFrameData; // 1048  <-- AV
        ...
```

- `pParent` is the **TabBar** — alive and valid (`mbInDtor == 0`).
- `pFrameParent` = `pParent->mpWindowImpl->mpFrameWindow` = the document's **top-level
  frame window** — **freed**. `pFrameParent->mpWindowImpl` reads back as the heap
  poison value, and the next dereference faults.

### 5.2 The path that gets there (cdb `kb`, abridged)

```
vcl!Window::ImplInsertWindow                         <-- deref freed frame window
vcl!Window::ImplInit
vcl!RadioButton::ImplInit
vcl!RadioButton::RadioButton
vcl!ImageRadioButton::ImageRadioButton
sfx!sfx2::sidebar::TabItem::TabItem
sfx!sfx2::sidebar::ControlFactory::CreateTabItem
sfx!sfx2::sidebar::TabBar::CreateTabItem
sfx!sfx2::sidebar::TabBar::SetDecks
sfx!sfx2::sidebar::SidebarController::UpdateConfigurations
sfx!...boost bind...
sfx!sfx2::sidebar::AsynchronousCall::HandleUserCall   <-- *** posted user event ***
tl!Link::Call
vcl!ImplHandleUserEvent
vcl!ImplWindowFrameProc
...
vcl!Application::Execute
sofficeapp!desktop::Desktop::Main
```

### 5.3 The lifetime bug

The async plumbing itself is **correct**:
- `AsynchronousCall::~AsynchronousCall()` → `CancelRequest()` → `RemoveUserEvent()`
  ([main/sfx2/source/sidebar/AsynchronousCall.cxx:49-86](main/sfx2/source/sidebar/AsynchronousCall.cxx#L49-L86)).
- The controller owns the call as a member
  ([main/sfx2/source/sidebar/SidebarController.cxx:125](main/sfx2/source/sidebar/SidebarController.cxx#L125)).

So a pending call *is* cancelled when the controller is disposed. The defect is **one
level up**: the ref-counted `SidebarController` and its `TabBar` (a VCL window)
**outlive the document frame window that hosts them**. The TabBar survives with a
dangling `mpFrameWindow`, the controller is *not* disposed when the frame is
destroyed, and so its already-posted `UpdateConfigurations` fires against the dead
frame.

This is consistent with the Start-Center → first-document frame transition during
startup, where an interim frame window is torn down while the sidebar controller it
spawned (and its posted call) survive.

---

## 6. Evidence this is a latent race, not a build/staging defect

1. **Nondeterministic poison value** — the freed frame window read back as
   `0xFEEEFEEE` (OS-heap free fill) in one run and `0xDDDDDDDD` (debug-CRT `delete`
   fill) in another. A deterministic defect (e.g. a missing component) fails
   identically every run.
2. **No repro under TTD** — recording with tttracer changed timing enough that the
   process ran to a clean `exit code 0`. The bug is suppressed by instrumentation
   (Heisenbug).
3. **Reference release build works** — same source, release timing wins the race.
4. **Config default is correct** (see §7) — the sidebar is *supposed* to be visible.

---

## 7. Ruled out: sidebar-visibility config mismatch

Checked whether the staging wrongly enables the sidebar. It does not — the default is
the standard upstream value in
[main/officecfg/registry/data/org/openoffice/Office/Views.xcu:33-38](main/officecfg/registry/data/org/openoffice/Office/Views.xcu#L33-L38):

```xml
<!-- show Sidebar child window by default - oor:name == SID_SIDEBAR -->
<node oor:name="10336" oor:op="replace">
  <prop oor:name="Visible" oor:type="xs:boolean"><value>true</value></prop>
</node>
```

`SID_SIDEBAR` (= 10336) is `Visible=true` by default in both the migration build and
the reference release. Not a staging gap.

---

## 8. Workaround (no rebuild)

Force the sidebar window hidden at startup so the `SidebarController` (and its async
call) is never created. Add to the user profile's
`<UserInstallation>/user/registrymodifications.xcu`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 <item oor:path="/org.openoffice.Office.Views/Windows/org.openoffice.Office.Views:WindowType['10336']">
  <prop oor:name="Visible" oor:op="fuse">
   <value>false</value>
  </prop>
 </item>
</oor:items>
```

(Path syntax verified against a `registrymodifications.xcu` written by the app itself.)

---

## 9. Suggested next steps

1. **Build/run a release (`-c opt`) staging** — the decisive, apples-to-apples test
   against the working reference install. Expected: the document frame opens cleanly,
   confirming this is a debug-only timing artifact.
2. If a real fix is ever pursued **upstream** (out of scope for the migration): ensure
   the `SidebarController` is disposed — and thus its pending `AsynchronousCall`
   cancelled — when its host frame window is destroyed, so a posted
   `UpdateConfigurations` cannot run against a freed frame.
3. Track as a **known debug-build limitation**, analogous to the documented
   `_HAS_ITERATOR_DEBUGGING` debug-only ABI issue.

---

## Appendix A — raw debugger output

### A.1 First-chance AV (no page heap)
```
vcl!Window::ImplInsertWindow+0x6f:
8b5208  mov edx,dword ptr [edx+8]   ds:002b:feeefef6=????????
edx=feeefeee
```

### A.2 Full page heap — block confirmed freed
```
vcl!Window::ImplInsertWindow+0x60:
mov edx,dword ptr [ecx+10Ch]   ds:002b:604c0fc4=????????
ecx=604c0eb8

!address 0x604c0eb8 ->
  Usage:   PageHeap
  State:   MEM_RESERVE
  Protect: PAGE_NOACCESS      (i.e. freed/decommitted)
```

### A.3 Second no-page-heap run — different poison
```
vcl!Window::ImplInsertWindow+0x6f:
8b5208  mov edx,dword ptr [edx+8]   ds:002b:dddddde5=????????
edx=dddddddd
```

### A.4 TTD recording
```
soffice.exe(x86) (PID:21652): Process exited with exit code 0 after 151375ms
  Full trace dumped to C:\temp\ooo.run
```

### A.5 Parent (TabBar) is alive
```
pParent = 0x08890a20
  mpWindowImpl = 0x08890bd8   (contiguous, freshly allocated)
  mbInDtor = 0  (not in destructor)
```
