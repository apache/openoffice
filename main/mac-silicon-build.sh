#!/bin/sh
#
# Apache OpenOffice - Apple Silicon (arm64) full-build wrapper.
#
# See ../.agent/mac-silicon-port.md for the full porting notes.
#
# Run from main/ after ./mac-silicon-configure.sh && ./mac-silicon-bootstrap.sh:
#   ./mac-silicon-build.sh                 # build.pl --all -P4 --stoponerror
#   ./mac-silicon-build.sh --from hyphen   # pass through any build.pl args
#
# Everything here must run WITHOUT Rosetta - native arm64 only.

set -e

SRCDIR=$(cd "$(dirname "$0")" && pwd)
cd "$SRCDIR"

# GNU awk: the bundled hyphen-2.8.8 Makefile calls `awk` and uses gensub(),
# a gawk-only function. Homebrew gawk's gnubin dir exposes it as `awk`.
GAWK_GNUBIN=/opt/homebrew/opt/gawk/libexec/gnubin
if [ -x "$GAWK_GNUBIN/awk" ]; then
  PATH="$GAWK_GNUBIN:$PATH"
  export PATH
fi

# CA bundle for any perl LWP fetches that still happen during the build
# (Apple perl has no Mozilla::CA).
export SSL_CERT_FILE=/etc/ssl/cert.pem
export PERL_LWP_SSL_CA_FILE=/etc/ssl/cert.pem
export CURL_CA_BUNDLE=/etc/ssl/cert.pem

. ./MacOSXAARCH64Env.Set.sh

# re-assert the gawk gnubin dir in case the env script reordered PATH
case ":$PATH:" in
  *":$GAWK_GNUBIN:"*) : ;;
  *) [ -x "$GAWK_GNUBIN/awk" ] && PATH="$GAWK_GNUBIN:$PATH" && export PATH ;;
esac

cd instsetoo_native
if [ "$#" -eq 0 ]; then
  set -- --all -P4 --stoponerror
fi
exec ../solenv/bin/build.pl "$@"
