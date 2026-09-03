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

# JDK. Temurin 11 (arm64) is the intended production JDK, but installing the
# Homebrew cask needs an admin password. OpenJDK 25 (arm64, already installed)
# passes configure's checks and is fine for the configure/bootstrap milestone.
# Override by exporting JAVA_HOME before running this script.
: "${JAVA_HOME:=/opt/homebrew/opt/openjdk/libexec/openjdk.jdk/Contents/Home}"
export JAVA_HOME

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
