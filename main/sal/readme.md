# code documentation
## typesconfig is Unix only
typesconfig.c is a probe program that runs on Unix to detect platform type sizes and generate typesizes.h.

## workben/ — developer test tools
Clipboard test utilities (testcopy, testpaste, testviewer). These are manual developer tools, not part of the build.

## test/ and qa/ — automated tests
These become cc_test targets (Bazel's test rule), not cc_library. Leave them until sal.dll itself builds correctly. Add them last.

# Technical debt
## tres.c
// tres.c line 164 — should read:
rtl_TestResult* SAL_CALL rtl_tres_create( const sal_Char* meth, sal_uInt32 flags )
### Why it worked before
VS2008 with /Gz compiled this without error — likely because the linker resolved it silently or VS2008 was more lenient about calling convention mismatches in C mode. It's also possible the old build system didn't apply /Gz to .c files, only .cxx files (they had separate CFLAGS and CXXFLAGS paths).
### What to do
For now: removing /Gz is correct and pragmatic. It makes the compiler default match SAL_CALL = __cdecl, hiding the inconsistency safely.

As technical debt: when source code changes become in scope, tres.c (and likely other .c files in sal) should be audited for missing SAL_CALL on function definitions. It's a mechanical fix — grep for exported functions in headers, verify definitions match.

Not blocking the build migration. Move forward.
## uwinapi
### old Windows9x Code:
"systools/win32/uwinapi/SHILCreateFromPathW.cpp",
"systools/win32/uwinapi/MCIWndCreateW.cpp",
not used anymore
"systools/win32/uwinapi/sntprintf.c",
seems outdated too.