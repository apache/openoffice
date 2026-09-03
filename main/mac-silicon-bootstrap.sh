#!/bin/sh
#
# Apache OpenOffice - Apple Silicon (arm64) bootstrap wrapper.
#
# See ../.agent/mac-silicon-port.md for the full porting notes.
#
# Run from main/ after ./mac-silicon-configure.sh has completed:
#   ./mac-silicon-bootstrap.sh
#
# Apple's system perl (which we pin as $PERL) has no Mozilla::CA, so
# download_external_dependencies.pl fails every HTTPS fetch with
# "500 Can't verify SSL peers ...". Point LWP/IO::Socket::SSL at a real
# CA bundle. /etc/ssl/cert.pem is the system bundle on macOS.

set -e

SRCDIR=$(cd "$(dirname "$0")" && pwd)
cd "$SRCDIR"

CA_BUNDLE=/etc/ssl/cert.pem
export SSL_CERT_FILE="$CA_BUNDLE"
export PERL_LWP_SSL_CA_FILE="$CA_BUNDLE"
export CURL_CA_BUNDLE="$CA_BUNDLE"

exec ./bootstrap "$@"
