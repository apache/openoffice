#!/bin/sh
#
# Apache OpenOffice - Apple Silicon (arm64) configure wrapper.
#
# See ../.agent/mac-silicon-port.md for the full porting notes.
#
# Run from main/:   ./mac-silicon-configure.sh
# It runs autoconf first (configure.ac has a hard "configure is out of date" gate)
# then ./configure with a production-oriented flag set for macOS/arm64.
#
# Deliberately does NOT export CFLAGS / CXXFLAGS / SDKROOT / -std=c++11:
# configure.ac detects the macOS SDK and generates its own clang++ C++ wrapper.

set -e

SRCDIR=$(cd "$(dirname "$0")" && pwd)
cd "$SRCDIR"

# --- toolchain selection -----------------------------------------------------

# JDK. AOO's build (bundled apache-commons, saxon, lucene, hsqldb, ...) uses
# javac source/target levels that JDK 11+ rejects ("Source option 1.5 is no
# longer supported"), so this port builds with JDK 8 - and it must be a native
# arm64 JDK 8 (no Rosetta). Amazon Corretto 8 ships macOS aarch64;
# `brew install --cask corretto@8`. Override by exporting JAVA_HOME.
: "${JAVA_HOME:=/Library/Java/JavaVirtualMachines/amazon-corretto-8.jdk/Contents/Home}"
export JAVA_HOME
if [ ! -x "$JAVA_HOME/bin/javac" ]; then
  echo "mac-silicon-configure: JAVA_HOME has no bin/javac: $JAVA_HOME" >&2
  exit 1
fi

# Apple's system perl has every module configure requires (Archive::Zip,
# LWP::UserAgent, LWP::Protocol::https, XML::Parser, Digest::*). Homebrew perl
# does not, and it is first on PATH - so pin the interpreter here.
export PERL=/usr/bin/perl

ANT_HOME=/opt/homebrew/opt/ant/libexec

# --- configure -------------------------------------------------------------

autoconf

./configure \
  --with-jdk-home="$JAVA_HOME" \
  --with-ant-home="$ANT_HOME" \
  --with-dmake-path="$SRCDIR/../../dmake/dmake" \
  --with-epm="$SRCDIR/../../epm/epm" \
  --with-package-format=dmg \
  --with-macosx-target=11.0 \
  --without-system-python \
  --without-system-curl \
  --without-system-libxml \
  --without-system-libxslt \
  --with-gnu-patch=/opt/homebrew/opt/gpatch/bin/gpatch \
  --without-junit \
  --without-stlport \
  --with-alloc=internal \
  --enable-category-b \
  --enable-bundled-dictionaries \
  --enable-wiki-publisher \
  --disable-systray \
  --with-lang="en-US de fr es it nl pt ru" \
  "$@"
