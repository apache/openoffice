# macOS/arm64: UNO exception RTTI — post-mortem

**Status: implemented and landed on trunk, 2026-08-03.** This was written as a fix plan;
it is retained as the design rationale, because the in-tree comments in
`solenv/src/component.map` and `solenv/bin/addsym-macosx.sh` deliberately defer to the
analysis in §2 and §3 rather than repeating it. The one item still outstanding is in §7a.

| Plan section | Landed as |
|---|---|
| §4.1–4.4 — synthesis code and corrected comments | `9ff3e22531` *Synthesise UNO exception RTTI on macOS/arm64* |
| §6.1–6.2 — ABI and cross-image harnesses | `4ca7e75a8a` *bridges: add arm64 RTTI synthesis QA harnesses* |
| §7a — x86-64 bridge cleanup | **still open**, not a blocker |

Both commits are on `origin/trunk`. The §8 commit split was executed as proposed.

**Original date:** 2026-08-03
**Blocked at the time:** full `unxmaccr.pro` build (`instsetoo_native` → `register_extensions` → `unopkg sync`)
**Files at the centre of it:** `main/bridges/source/cpp_uno/s5abi_macosx_aarch64/{except.cxx,share.hxx}`

---

## 1. What is actually broken

The build dies at the last packaging step:

```
.../OpenOffice.app/Contents/program/unopkg sync --verbose ... failed!
in function: register_extensions
```

`unopkg` aborts with
`com.sun.star.uno.RuntimeException: no rtti for type com.sun.star.ucb.InteractiveAugmentedIOException`.

That message comes from `RTTI::getRTTI()` in the arm64 bridge: it resolves an exception's
`type_info` with `dlsym(RTLD_DEFAULT, "_ZTIN...E")`, the lookup fails, and `raiseException()`
substitutes a `RuntimeException` for the exception the caller was supposed to receive.

**The previously recorded root cause was wrong.** The note in memory (and the comments added in
`adce10bba5` to `solenv/src/component.map` and `solenv/bin/addsym-macosx.sh`) says the typeinfo is
never *emitted*, because the type is never literally thrown or caught in hand-written C++. That is
not what is happening, and the difference changes the fix completely.

---

## 2. Evidence

All commands below were run against the existing build tree; results are reproducible.

### 2.1 The typeinfo *is* emitted — it just is not exported

```console
$ P=main/instsetoo_native/unxmaccr.pro/Apache_OpenOffice/dmg/install/en-US_witherror/\
Apache_OpenOffice_4.5.0_MacOS_aarch64_install_en-US/OpenOffice.app/Contents/program
$ nm -m $P/libucpfile1.dylib | grep InteractiveAugmentedIOException
0000000000042350 (__DATA_CONST,__const) non-external (was a private external) \
    __ZTIN3com3sun4star3ucb31InteractiveAugmentedIOExceptionE
```

Present in `libsfx.dylib`, `libucpdav1.dylib` and `libucpfile1.dylib` — always
`non-external (was a private external)`, i.e. hidden. `dlsym()` only finds exported symbols, so it
legitimately returns null.

### 2.2 This is not one exception — it is all of them

Over every shipped dylib:

```console
$ for f in $P/*.dylib; do nm -m "$f"; done | grep "__ZTIN3com3sun4star" > /tmp/all.txt
$ grep -c " external "  /tmp/all.txt      # 130
$ grep -c "non-external" /tmp/all.txt     # 5559
```

Restricted to distinct UNO **exception** typeinfos (`...ExceptionE`):

| | count |
|---|---|
| distinct exception typeinfos present in the shipped binaries | **136** |
| of those, exported (`external`) anywhere | **0** |
| UNO exception types declared in `udkapi`/`offapi` IDL | 299 |

The 130 exported `_ZTIN3com...` symbols are all *polymorphic implementation* classes (67 distinct,
almost all `com::sun::star::i18n::*`), not exceptions.

Even the two typeinfos explicitly listed in `bridges/source/bridge_exports.map` —
`_ZTIN3com3sun4star3uno9ExceptionE` and `_ZTIN3com3sun4star3uno16RuntimeExceptionE` — are
`non-external` in every dylib that contains them (`libs5abi_uno`, `libuno_cppuhelper`, `libcomphelp`,
`libucbhelper`, `libjvmaccess`, `libi18nutil`, …). `ucb/source/ucp/file/exports2.dxp` likewise lists
`_ZTIN3com3sun4star3ucb31InteractiveAugmentedIOExceptionE` and it is still hidden in the built
`libucpfile1.dylib`. **Export lists cannot fix this.**

### 2.3 Root cause: clang emits keyless-class RTTI differently on arm64 Darwin

```console
$ cat vis3.cxx
struct A { int x; };                                    // keyless (like every UNO exception)
struct __attribute__((visibility("default"))) B { int x; };
struct C { int x; virtual void key(); };  void C::key() {}   // has a key function
#include <typeinfo>
const void* refs[] = { &typeid(A), &typeid(B), &typeid(C) };

$ clang++ -arch arm64  -c vis3.cxx && nm -m vis3.o | grep __ZTI
(__DATA,__const) external                             __ZTI1C
(__DATA,__const) weak private external                __ZTI1A
(__DATA,__const) weak private external                __ZTI1B

$ clang++ -arch x86_64 -c vis3.cxx && nm -m vis3.o | grep __ZTI
(__DATA,__const) external                             __ZTI1C
(__DATA,__const) weak external                        __ZTI1A     <-- exported
(__DATA,__const) weak external                        __ZTI1B     <-- exported
```

* **x86_64 Darwin:** typeinfo for a class with no key function is `weak external` → exported →
  `dlsym()` finds it → the `s5abi_macosx_x86-64` bridge almost never takes its synthesis path.
  That is why macOS/Intel works.
* **arm64 Darwin:** the same typeinfo is `weak private external` (hidden) → `dlsym()` can never
  find it.

This is clang's *NonUniqueARMRTTIBit* scheme. On arm64 Apple, libc++ selects
`_LIBCPP_TYPEINFO_COMPARISON_IMPLEMENTATION 3` (see `$(xcrun --show-sdk-path)/usr/include/c++/v1/typeinfo`,
`__non_unique_arm_rtti_bit_impl`): hidden RTTI is per-image, and the **top bit of
`type_info::__type_name` is set** to mark it non-unique; `operator==` then falls back to `strcmp` of
the mangled name. So hidden RTTI still compares correctly across dylibs — but it is invisible to
`dlsym`.

Neither `-fvisibility=...`, `__attribute__((visibility("default")))` on the class, a version script,
an `exports2.dxp`, nor any clang flag changes this (`clang++ --help-hidden | grep -i 'unique\|rtti'`
offers nothing relevant). UNO exceptions are plain structs with no virtual functions, so they can
never have a key function, and adding one would be an ABI break.

**Conclusion:** on arm64 Darwin the bridge's `dlsym()` lookup for UNO exception typeinfo can never
succeed. Every UNO exception crossing the UNO→C++ bridge currently degrades to `RuntimeException`.
`unopkg` is simply the first place the build notices.

### 2.4 Naive synthesis (what the x86-64 bridge does) does **not** work on libc++abi

`s5abi_macosx_x86-64/share.hxx` declares its own `__class_type_info` / `__si_class_type_info`
stand-ins and `new`s them. Those objects get **their own vtable**, not libc++abi's.
`__class_type_info::can_catch()` in libc++abi does a
`dynamic_cast<const __class_type_info*>(thrown_type)`, which fails for such an object, so no typed
handler ever matches.

Measured (arm64, Xcode 16 toolchain — see harness in §6.1):

```
3: real compiler-emitted typeinfo (control)    : CAUGHT as UnoException&  (WORKS)
1: naive hand-built (AOO x86-64 approach)      : caught only by catch(...)  (BROKEN)
2a: borrowed vtable, plain name                : CAUGHT as UnoException&  (WORKS)
2b: borrowed vtable, non-unique bit set        : CAUGHT as UnoException&  (WORKS)
```

The arm64 bridge author's decision not to copy the x86-64 approach was correct.

### 2.5 What *does* work: borrowed vtable + non-unique bit

Cross-dylib harness (§6.2): a dylib synthesises the **entire** chain — a fake
`InteractiveAugmentedIOException` whose `__base_type` is itself a synthesised
`com::sun::star::uno::Exception` — and throws it; a separate executable catches using its own real,
hidden typeinfo.

```
RESULT: caught as EXACT derived type  (WORKS)
```

Negative control, identical but with the non-unique bit cleared:

```
RESULT: caught only by catch(...)  (BROKEN)
```

Both ingredients are required:

1. **The vtable must be a genuine libc++abi one.** Take it from a compiler-emitted `type_info` of a
   local helper type — never hardcode `_ZTVN10__cxxabiv1...`. This automatically matches whatever
   libc++abi the process actually loaded.
2. **The mangled-name pointer must carry the non-unique bit** (bit 63), because the handler's real
   typeinfo is hidden and therefore also non-unique; `__eq` returns false if *either* side claims
   uniqueness.

---

## 3. Options rejected, with reasons

| Option | Verdict |
|---|---|
| Export `_ZTI*` via version maps / `component.map` / `exports2.dxp` | **Impossible.** The symbols are private-extern in the `.o` before any linker export list is consulted (§2.1, §2.2). Also note `solenv/inc/unxmacc.mk:224` sets `LINKVERSIONMAPFLAG=-Wl,-map -Wl,` — the map file is used as ld's *output link map*, not as an export list, so the macOS clang build has no export filtering at all. |
| Force per-exception emission by adding an explicit `throw`/`catch`/`typeid()` in C++ (e.g. in `ucbhelper`) | **Does not help.** Emission was never the problem; the symbol is emitted and hidden regardless. Verified: `libucpfile1` catches these types and still ships them hidden. |
| Copy the `s5abi_macosx_x86-64` synthesis verbatim | **Broken.** §2.4 — produces exceptions catchable only by `catch(...)`, i.e. `std::terminate` at most call sites. Worse than today's behaviour. |
| Compiler/linker flag to restore unique RTTI on arm64 | **No such flag exists.** §2.3. |
| Generate an exported "RTTI registry" library covering all 299 IDL exception types | Workable but heavy: a generated TU per API change, a new dylib in the dependency graph, and it still relies on the same non-unique-bit `strcmp` matching that §2.5 already gives us for free. Keep as fallback only. |

---

## 4. The fix

*Implemented as described, in `9ff3e22531`. The code below is the design as written; read
the bridge sources for what actually shipped. `RttiDonorBase`/`RttiDonorDerived` are in
`share.hxx`; `NON_UNIQUE_RTTI_BIT`, `rttiDonorsUsable()`, `rttiIsNonUnique()`,
`m_generatedRttis` and `synthesiseRTTI()` are in `except.cxx`.*

Confine the change to `bridges/source/cpp_uno/s5abi_macosx_aarch64/`. Keep the `dlsym` lookup first
(it is correct whenever a real symbol does exist), keep the existing `observedRttis()`
short-circuit ahead of it (a real typeinfo observed via `fillUnoException` is always preferable),
and add a synthesis fallback that is ABI-correct.

### 4.1 `share.hxx` — layouts and donors

Add to `namespace CPPU_CURRENT_NAMESPACE` (the arm64 `share.hxx`, which currently has no
`__class_type_info` declarations — do **not** add the x86-64 stand-in classes):

```cpp
// Donor types for RTTI synthesis.  Their type_info objects are emitted by the
// compiler, so they carry the real libc++abi vtables and the platform's own
// uniqueness convention.  They must stay ordinary namespace-scope classes with
// no virtual functions and a single public non-virtual base -- exactly the
// shape of a generated UNO exception -- so that typeid(RttiDonorDerived) is a
// __si_class_type_info and typeid(RttiDonorBase) a __class_type_info.
// Do not move them into an anonymous namespace.
struct RttiDonorBase { sal_Int32 dummy; };
struct RttiDonorDerived : public RttiDonorBase { sal_Int32 dummy2; };

// Itanium ABI object layouts (http://itanium-cxx-abi.github.io/cxx-abi/abi.html#rtti).
// libc++abi does not publish __cxxabiv1::__class_type_info, and declaring a
// look-alike class is not an option: it would get its own vtable, and
// __class_type_info::can_catch() dynamic_casts the thrown type to the real
// libc++abi class, so no typed handler would ever match.  We therefore build
// raw storage in the ABI layout and install a borrowed, genuine vtable.
struct RttiClassLayout   { void const * pVtable; sal_uIntPtr nName; };
struct RttiSiClassLayout { void const * pVtable; sal_uIntPtr nName; void const * pBase; };
```

### 4.2 `except.cxx` — helpers

Add near the top of the existing anonymous namespace (needs `<stdlib.h>` for `calloc`/`free`;
`<string.h>` for `strdup` is already included):

```cpp
// libc++ marks a type_info whose object is not unique across images by setting
// the top bit of type_info::__type_name; comparison then falls back to strcmp
// of the mangled name (see __non_unique_arm_rtti_bit_impl in <typeinfo>).  On
// arm64 Darwin clang emits the typeinfo of every keyless class -- which is every
// UNO exception -- hidden and therefore non-unique, so a synthesised object must
// set the bit too, or std::type_info::operator== degenerates to an address
// comparison and never matches the handler's real typeinfo.
sal_uIntPtr const NON_UNIQUE_RTTI_BIT =
    static_cast< sal_uIntPtr >(1) << (8 * sizeof (sal_uIntPtr) - 1);

RttiSiClassLayout const * siDonor()
{
    return reinterpret_cast< RttiSiClassLayout const * >( &typeid(RttiDonorDerived) );
}
RttiClassLayout const * classDonor()
{
    return reinterpret_cast< RttiClassLayout const * >( &typeid(RttiDonorBase) );
}

// Refuse to synthesise unless the donors really have the layout we assume.
bool rttiDonorsUsable()
{
    return sizeof (void *) == 8
        && siDonor()->pBase == static_cast< void const * >( classDonor() );
}

// Mirror the platform's own convention rather than assuming it.
bool rttiIsNonUnique()
{
    return (siDonor()->nName & NON_UNIQUE_RTTI_BIT) != 0;
}
```

Also factor the existing symbol-name construction out of `getRTTI` into:

```cpp
OString mangledRttiSymbol( OUString const & unoName ) SAL_THROW( () );  // "_ZTIN...E", body unchanged
```

### 4.3 `except.cxx` — `RTTI`

Add `t_rtti_map m_generatedRttis;` and a private
`type_info * synthesiseRTTI( OString const &, typelib_CompoundTypeDescription * ) SAL_THROW( () );`
(called with `m_mutex` held).

```cpp
type_info * RTTI::getRTTI( typelib_CompoundTypeDescription * pTypeDescr ) SAL_THROW( () )
{
    OUString const & unoName = *(OUString const *)&pTypeDescr->aBase.pTypeName;

    // Recursive: synthesiseRTTI() re-enters getRTTI() for the base chain.
    // osl::Mutex is a PTHREAD_MUTEX_RECURSIVE (sal/osl/unx/mutex.c:71), so this
    // is safe.  Lock order against exceptionMapsMutex() is unchanged.
    MutexGuard guard( m_mutex );

    {
        MutexGuard observedGuard( exceptionMapsMutex() );
        ObservedRttiMap::const_iterator observed( observedRttis().find( unoName ) );
        if ( observed != observedRttis().end() )
            return observed->second;          // a real typeinfo always wins
    }

    t_rtti_map::const_iterator iFind( m_rttis.find( unoName ) );
    if (iFind != m_rttis.end())
        return iFind->second;

    OString symName( mangledRttiSymbol( unoName ) );
    type_info * rtti = static_cast< type_info * >( dlsym( RTLD_DEFAULT, symName.getStr() ) );
    if (rtti != 0)
    {
        m_rttis.insert( t_rtti_map::value_type( unoName, rtti ) );
        return rtti;
    }

    t_rtti_map::const_iterator iGen( m_generatedRttis.find( unoName ) );
    if (iGen != m_generatedRttis.end())
        return iGen->second;

    rtti = synthesiseRTTI( symName, pTypeDescr );
    if (rtti != 0)
        m_generatedRttis.insert( t_rtti_map::value_type( unoName, rtti ) );
    return rtti;
}

type_info * RTTI::synthesiseRTTI(
    OString const & rSymbolName,
    typelib_CompoundTypeDescription * pTypeDescr ) SAL_THROW( () )
{
    if (! rttiDonorsUsable())
        return 0;                             // keep the loud RuntimeException fallback

    type_info * pBaseRtti = 0;
    if (pTypeDescr->pBaseTypeDescription != 0)
    {
        // The whole chain must resolve: libc++abi walks __base_type when matching
        // a handler for a base class and would dereference a null link.
        pBaseRtti = getRTTI(
            (typelib_CompoundTypeDescription *) pTypeDescr->pBaseTypeDescription );
        if (pBaseRtti == 0)
            return 0;
    }

    // The mangled type name is the symbol name without its "_ZTI" prefix.
    char * pName = strdup( rSymbolName.getStr() + 4 );
    if (pName == 0)
        return 0;
    sal_uIntPtr nName = reinterpret_cast< sal_uIntPtr >( pName );
    if (rttiIsNonUnique())
        nName |= NON_UNIQUE_RTTI_BIT;

    // Deliberately never freed; these live for the life of the process
    // (the module already builds with -DLEAK_STATIC_DATA).
    if (pBaseRtti != 0)
    {
        RttiSiClassLayout * p = static_cast< RttiSiClassLayout * >(
            calloc( 1, sizeof (RttiSiClassLayout) ) );
        if (p == 0) { free( pName ); return 0; }
        p->pVtable = siDonor()->pVtable;
        p->nName   = nName;
        p->pBase   = pBaseRtti;
        return reinterpret_cast< type_info * >( p );
    }

    RttiClassLayout * p = static_cast< RttiClassLayout * >(
        calloc( 1, sizeof (RttiClassLayout) ) );
    if (p == 0) { free( pName ); return 0; }
    p->pVtable = classDonor()->pVtable;
    p->nName   = nName;
    return reinterpret_cast< type_info * >( p );
}
```

`raiseException()` needs no change: it already handles `rtti == 0` correctly (destructs the payload,
frees the `__cxa` buffer, destructs the incoming any, throws a diagnostic `RuntimeException`). That
path now only fires if the donor sanity check fails.

`deleteException()` needs no change either — the arm64 bridge keys the type description off the
exception pointer in `thrownTypes()`, so it never reads the synthesised `type_info`.
`fillUnoException()` needs no change: `type_info::name()` masks the non-unique bit off before
returning the string, so `toUNOname()` still works.

### 4.4 Correct the now-wrong comments

*Done. `component.map` and `addsym-macosx.sh` now carry the keyless-class explanation and
point back here.*

Three places documented the old (incorrect) theory and would have misled the next reader:

* `main/solenv/src/component.map` — the block claiming the arm64 bridge "resolves exception typeinfo
  with `dlsym()` rather than synthesising it, so an unexported `_ZTI` symbol degrades a UNO exception
  into a RuntimeException". Replace with: exporting `_ZTI*`/`_ZTS*` is right in principle for
  platforms with a real version script, but on macOS/arm64 keyless-class typeinfo is private-extern
  before any export list is applied, so the bridge cannot rely on it and synthesises instead.
* `main/solenv/bin/addsym-macosx.sh` — the `__ZT[IS]` carve-out comment makes the same claim. The
  carve-out itself is harmless; fix the justification.
* `main/bridges/source/cpp_uno/s5abi_macosx_aarch64/except.cxx` — the "Unlike the gcc3_* bridges this
  one does NOT synthesise" comment is replaced by the new code and its comments.

---

## 5. Issue #2 — scope of the gap

The audit answers it: **the gap is total, not partial.** It is not a subset of exceptions that
happen to lack RTTI; on arm64 Darwin *no* UNO exception type can ever be resolved by `dlsym`
(§2.2, §2.3). All 299 IDL-declared exception types are affected, of which 136 currently materialise
in the shipped binaries.

Consequences for the plan:

* There is no per-type patch campaign to run and no version-map audit to do. The single change in
  §4 covers every exception type at once, present and future.
* Non-exception UNO RTTI is **not** affected. `dynamic_cast` between UNO interface pointers across
  dylibs keeps working, because there both sides are real compiler-emitted typeinfos and libc++'s
  non-unique-bit `strcmp` fallback matches them. `getRTTI()` is called only from `raiseException()`
  (verified: it is the sole caller in the bridge).
* Guard against regressions with the check in §6.3 rather than with an export list — someone
  reading `component.map` today would reasonably conclude the fix is "export more symbols", and it
  is not.

---

## 6. Test plan

*The harnesses were committed in `4ca7e75a8a`. The scratch `rtti-harness/` directory that
sat next to this document is gone; the sources now live under
`bridges/source/cpp_uno/s5abi_macosx_aarch64/qa/`.*

### 6.1 ABI harness (unit level)

`qa/rtti_synthesis_test.cxx`, alongside `qa/keyless_rtti_visibility_demo.cxx` for the §2.3
visibility demonstration. They compile standalone with `clang++ -std=c++11 -arch arm64`.
`rtti_synthesis_test.cxx` asserts:

1. control: a real compiler-emitted typeinfo is caught by a base handler;
2. naive hand-built typeinfo is caught **only** by `catch(...)` (documents why we don't do that);
3. borrowed-vtable synthesis is caught by a base handler;
4. borrowed-vtable + non-unique bit is caught by a base handler.

Optionally wire it as a gbuild `GoogleTest` target — `sw/GoogleTest_sw_bigpointerarray.mk` plus its
`sw/Module_sw.mk` entry is the template. `bridges` is a dmake module, so this needs a
`Module_bridges.mk`; treat it as optional, not a blocker.

### 6.2 Cross-image harness (the real scenario)

`qa/rtti_crossdylib_test/` (`thrower.cxx`, `catcher.cxx`, `uno_like.hxx`) — a dylib that
synthesises a *complete* chain (derived + base, neither
resolvable) and throws, and an executable that catches with its own real hidden typeinfo.
Must print `caught as EXACT derived type`. Build:

```sh
clang++ -std=c++11 -arch arm64 -dynamiclib -o libthrower.dylib thrower.cxx \
        -install_name @rpath/libthrower.dylib
clang++ -std=c++11 -arch arm64 -o catcher catcher.cxx -L. -lthrower -Wl,-rpath,@executable_path
./catcher
```

Include the bit-cleared negative control; it must print `caught only by catch(...)`.

### 6.3 In-tree regression checks

* `testtools` bridgetest already round-trips exceptions through the bridge and catches
  `IllegalArgumentException` by exact type (`testtools/source/bridgetest/bridgetest.cxx:590,597`).
  Run it on arm64; it should pass both before and after, but it will catch a synthesis regression
  that makes exceptions uncatchable.
* Symbol-expectation check (documents §2.2 so nobody "fixes" it by exporting):

  ```sh
  nm -m OpenOffice.app/Contents/program/*.dylib | grep "__ZTIN3com3sun4star" \
    | grep -c " external "   # expected: 0 exception typeinfos; only keyed impl classes
  ```

### 6.4 End-to-end

```sh
cd main && build --all:instsetoo_native
```

Success criteria: `unopkg sync` completes under `register_extensions`, and
`openoffice_en-US.dmg` is produced. This is the gate for calling the build fixed.

Additionally smoke-test a real extension install with the built app (`unopkg add`) — that exercises
the same path with a non-trivial exception surface.

---

## 7. Risks and edge cases

* **Mixed uniqueness.** `__eq` returns false if *either* name claims uniqueness. If a type's real
  typeinfo were exported (unique, no bit) but not yet loaded when the bridge synthesised, the two
  would not match. This cannot arise for UNO exceptions (all keyless → always non-unique), but do
  not extend synthesis to keyed classes without revisiting this.
* **Multiple inheritance.** Only `__class_type_info` and `__si_class_type_info` are synthesised.
  UNO exceptions are single-inheritance by construction; if `pBaseTypeDescription` ever indicated
  otherwise the code returns 0 and falls back to today's behaviour rather than building a wrong
  `__vmi_class_type_info`.
* **Toolchain drift.** The design deliberately borrows the vtable from a compiler-emitted `type_info`
  in the same image and mirrors the platform's own non-unique-bit convention, so a future libc++abi
  change to either is followed automatically. `rttiDonorsUsable()` is the tripwire: if the layout
  assumption ever breaks, synthesis is disabled and the loud `RuntimeException` returns instead of a
  crash.
* **Pointer authentication.** libc++ guards `type_info`'s vptr with
  `[[clang::ptrauth_vtable_pointer]]` when the feature is on (arm64e). This build targets arm64, not
  arm64e, so copying the vptr verbatim is correct. If AOO ever targets arm64e, the copy must be
  re-signed and this needs revisiting — add a comment saying so.

---

## 7a. Still open: the x86-64 bridge

The only unfinished item from this work. Not a build blocker, and deliberately deferred.

`s5abi_macosx_x86-64/share.hxx` still declares the naive `__class_type_info` /
`__si_class_type_info` stand-ins (around lines 90–103) and `new`s them. That synthesis path
is latently broken in exactly the way §2.4 measures — such objects get their own vtable, so
`__class_type_info::can_catch()`'s `dynamic_cast` fails and no typed handler matches.

It does no harm today because it is effectively dead code: `dlsym` succeeds on x86_64, since
keyless-class typeinfo is `weak external` there (§2.3), so the synthesis path is essentially
never taken.

If it is ever fixed, do **not** copy the arm64 code across unchanged. x86_64 uses libc++
comparison implementation 1 (unique addresses): the non-unique bit does not apply, and
matching works only because the base link resolves to the real, exported typeinfo.

---

## 8. Commit split — as executed

1. `9ff3e22531` *Synthesise UNO exception RTTI on macOS/arm64* — §4.1–§4.3 plus the
   `except.cxx` comment, and §4.4's `component.map` / `addsym-macosx.sh` corrections.
2. `4ca7e75a8a` *bridges: add arm64 RTTI synthesis QA harnesses* — §6.1, §6.2.

The plan proposed §4.4 as its own commit; in the event it went in with item 1. Item 1
unblocked the build; the harnesses stop the wrong theory from being re-derived later.
