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

# Document save path — atomic replace (Issue 126990)

Notes on how `SfxMedium` writes a document over an existing file, and why it is
written the way it is.  The short version: **never truncate the user's document
before the replacement exists and is on the disk.**

## The bug this addresses

Users reported opening a previously saved document and finding it full of `#`
characters — hundreds of reports over many years, always after a crash, freeze,
forced shutdown or power loss.  The files were not damaged ODF; they were a run
of zero bytes of roughly the right length.  With no `PK` zip header at offset 0,
type detection falls through to the plain text filter, and `0x00` renders as `#`.

The zeros were never written by us.  Two defects combined to produce them:

1. `TransactedTransferForFS_Impl()` truncated the target to zero (via the `Size`
   property) and then streamed the new contents into it.  Despite the name,
   nothing was transacted — the document was destroyed at the moment of
   truncation, and everything after that point was a window in which a crash
   cost the whole file.
2. Nothing forced the data to the medium.  `osl_closeFile()` flushes osl's own
   buffer and calls `CloseHandle()`; it never calls `FlushFileBuffers()`/`fsync()`.
   So the bytes sat in the OS page cache while the save reported success.

File systems that journal metadata but not data — NTFS, HFS+, XFS, and ext4 with
delayed allocation — commit the new file *size* while the data is still only in
the cache.  Crash in that window and the file reads back as N zeros.  Because the
save had already reported success, users acted on it and lost the document.

## What the code does now

Two changes, and they are **not independent — the flush is a prerequisite for the
rename.** Renaming a replacement whose data is still in the page cache would swap
in a file that the same crash turns into zeros, just under a different name.

- `ucb/source/ucp/file/shell.cxx`, `shell::write()` — syncs before reporting
  success, and reports a sync failure as a write failure rather than a silent
  success.
- `docfile.cxx`, `lcl_ReplaceTargetAtomically()` — streams the new contents into a
  sibling of the target and renames it over the target.  The previous document
  stays intact until one atomic operation swaps in the new one, so an interruption
  now costs at most the most recent save instead of the whole document.

Note that configmgr has always written `registrymodifications.xcu` this way.  The
document save path was the odd one out.

## Constraints — do not "simplify" these away

- **The replacement must be a sibling of the target.**  `osl_moveFile()` passes
  `MOVEFILE_COPY_ALLOWED`, which silently degrades a cross-volume move into a
  non-atomic copy+delete, and POSIX `rename()` cannot cross a mount point at all.
  `SfxMedium::CreateTempFile()` produces temporaries in the *system* temp
  directory — reusing one of those would quietly destroy the atomicity.
- **The in-place path is kept as a fallback, not dead code.**  A rename needs
  delete access on the target; an in-place rewrite does not.  A process holding
  the document open — antivirus, a search indexer — would otherwise turn a save
  that used to work into a hard failure.  That is a worse regression than the rare
  corruption this fixes.
- **Symlinked targets deliberately take the fallback.**  An in-place rewrite
  follows the link and updates what it points at; a rename would replace the link
  itself with a regular file.
- **`bTransactStarted` and `UseBackupToRestore_Impl()` stay live** — they are
  reachable through the fallback.

## Known limitations

- **ACLs, alternate data streams, creation time.**  The renamed file carries the
  sibling's security descriptor, not the original's.  Where ACLs are inherited
  from the containing folder — the common case — the result is identical and
  nothing is lost; it only differs for explicit per-file ACEs and ADS.  Win32
  `ReplaceFile()` is the primitive that preserves all of it, but it is not exposed
  by osl, and adding it would mean a new exported sal API.
- **POSIX rename durability.**  `rename()` is atomic, but the rename itself is not
  durable without an `fsync` of the containing directory, and osl has no
  directory-sync API.  On Windows `MOVEFILE_WRITE_THROUGH` covers this.  The POSIX
  failure mode without it is "last save lost", not "document destroyed".
- **macOS `fsync` is weaker than it looks** — it does not flush the drive write
  cache; `fcntl(F_FULLFSYNC)` would be required.
- **Peak disk space on the target volume roughly doubles.**  The original and the
  replacement now coexist until the rename, where the old code held only the file
  being rewritten.  A large save onto a nearly full volume can therefore hit
  `ENOSPC` where it previously did not; that falls back to the in-place path.
  Conversely the disk-full case is now *safer*: the original is no longer
  destroyed before it is known that the new data fits.
- **The backup is still not durable.**  `DoInternalBackup_Impl()` copies via
  `transferContent`, which routes to the file UCP's `copy` rather than
  `shell::write`, so it does not get the sync.

## Verifying a build

The interesting calls are exported Win32 APIs, so this works on a release build
with no PDBs and no debug-level rebuild.  Under `cdb`:

```
bu KERNELBASE!MoveFileExW ".echo >>> MoveFileExW; du @rcx; du @rdx; gu; r rax; gc"
bu KERNELBASE!FlushFileBuffers ".echo >>> FlushFileBuffers; gc"
```

A healthy overwrite save shows flush(es), then a rename whose source and
destination are **in the same directory**, with `r8=b`
(`REPLACE_EXISTING|COPY_ALLOWED|WRITE_THROUGH`) identifying it as `osl_moveFile`:

```
>>> FlushFileBuffers
>>> MoveFileExW
  C:\Users\...\Documents\mydoc0.odt
  C:\Users\...\Documents\mydoc.odt
  rax=1
```

Things that will waste your time otherwise:

- **Save As to a new file never takes the atomic path**, by design — the helper is
  behind `bOverWrite && IsDocument(aDest)` and there is nothing yet to protect.
  Only an overwrite exercises it.
- **`MoveFileExW` traffic is normal background noise** — configmgr writes its store
  the same way, with the same flags.  Always read the paths before concluding
  anything; `\user\` is config, not the document.
- **The breakpoints fire on entry**, so a *failed* rename looks identical to a
  successful one.  Capture `rax` (as above) or the fallback is invisible.  Without
  it, the fallback's tell is one extra `FlushFileBuffers` immediately after the
  document rename.
- **`SetEndOfFile` is not a fallback signal** — it also fires when `shell::write`
  truncates the sibling that `TempFile` has already created.

To exercise the fallback deliberately, hold the target open against delete from
another process while saving; the rename fails and the save must still succeed:

```powershell
$f = [System.IO.File]::Open($p, 'Open', 'Read', 'ReadWrite')  # ReadWrite withholds Delete
```
