#!/usr/bin/env bash
# Wrapper: invokes OpenSSL Configure + nmake for VC-WIN32 static build.
# PERL env var is set to Strawberry Perl's perl.exe by rules_foreign_cc.
set -euo pipefail

# Unset the double-quoted compiler paths BEFORE Configure so OpenSSL's
# VC-WIN32 target uses its own defaults (CC=cl, CFLAGS=/MT ...) rather
# than baking the rules_foreign_cc-injected quoted absolute paths into
# the generated makefile.  e.g. CC='"C:/Program Files.../cl.exe"' baked
# into the makefile produces ""C:/path/cl.exe"" which fails at compile.
# INCLUDE and LIB (needed by cl.exe) remain set from the Bazel toolchain.
unset CC CXX AR LD LDFLAGS CFLAGS CXXFLAGS || true

# Run OpenSSL's perl Configure (receives --prefix=... VC-WIN32 no-asm no-shared)
"$PERL" Configure "$@"

# Build static libs and install headers + libs to the configured prefix.
# Done here (not via targets = []) so all nmake calls — including recursive
# $(MAKE) /e sub-makes — see the clean environment.
nmake.exe build_libs
nmake.exe install_dev
