#!/usr/bin/env bash
# Wraps the NSS 3.39 + NSPR 4.20 Windows-MSVC build for rules_foreign_cc.
# Requires: bash + GNU make (Cygwin/MSYS2) in PATH; MSVC cl.exe via toolchain.
set -euo pipefail

# Parse --prefix= from args
INSTALL_PREFIX=""
for arg in "$@"; do
    case "$arg" in
        --prefix=*) INSTALL_PREFIX="${arg#--prefix=}" ;;
    esac
done
if [ -z "$INSTALL_PREFIX" ]; then
    echo "Error: no --prefix argument provided" >&2
    exit 1
fi

# Unset Bazel-injected compiler vars; NSS coreconf sets its own via OS_TARGET
unset CC CXX AR LD LDFLAGS CFLAGS CXXFLAGS || true

# Windows MSVC x86 build configuration (mirrors AOO makefile.mk WNT+MSVC path)
export MOZ_MSVCVERSION=9
export OS_TARGET=WINNT
export BUILD_OPT=1
export NSS_DISABLE_GTESTS=1

# NSPR's configure uses config.guess from 2014, which doesn't recognise
# MSYS_NT-* (MSYS2 reports "MSYS_NT-10.0-..."). Setting these autoconf cache
# variables bypasses config.guess entirely. i686-pc-cygwin is the correct
# 32-bit Windows triplet that NSPR's configure.in accepts when CC=cl is set.
export ac_cv_build=i686-pc-cygwin
export ac_cv_host=i686-pc-cygwin

# Build inside the nss/ subdirectory; nspr/ in the sibling directory is picked
# up automatically by NSS's coreconf makefile infrastructure.
cd nss
make nss_build_all

# Locate the platform OBJ output directory (e.g. WINNT5.0_OPT.OBJ)
OBJ_DIR=$(ls -d ../dist/*.OBJ 2>/dev/null | head -1)
if [ -z "$OBJ_DIR" ]; then
    echo "Error: NSS OBJ output directory not found under dist/" >&2
    exit 1
fi

# Install all public headers into a flat include/ so consumers can use
# #include <nspr.h> and #include <nss.h> with a single -I path.
mkdir -p "${INSTALL_PREFIX}/include"
cp ../dist/public/nss/*.h  "${INSTALL_PREFIX}/include/" 2>/dev/null || true
cp "${OBJ_DIR}/include/"*.h "${INSTALL_PREFIX}/include/" 2>/dev/null || true

# Install import libs and DLLs
mkdir -p "${INSTALL_PREFIX}/lib"
cp "${OBJ_DIR}/lib/"*.lib  "${INSTALL_PREFIX}/lib/" 2>/dev/null || true
cp "${OBJ_DIR}/lib/"*.dll  "${INSTALL_PREFIX}/lib/" 2>/dev/null || true
