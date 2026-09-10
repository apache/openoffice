#!/bin/bash
#
# Code-sign a built Apache OpenOffice .app (or .dmg) on macOS.
#
#   ./solenv/bin/mac-silicon-sign.sh [options] <OpenOffice.app|installer.dmg> ...
#
# Options:
#   -i, --identity ID   codesign identity; "-" = ad-hoc (default, or
#                       $MACOSX_CODESIGNING_IDENTITY when set)
#   -e, --entitlements  entitlements plist (default: mac-silicon-entitlements.plist)
#       --hardened      force hardened runtime even for an ad-hoc signature
#                       (implied by a real identity)
#       --verify        only report the current signing state, change nothing
#
# The linker already ad-hoc-signs each Mach-O it produces, which is why the
# binaries load at all on arm64. What it does not do is seal the *bundles*:
# without a _CodeSignature/CodeResources the .app has no identity, Gatekeeper
# rejects it once it carries a quarantine flag, and it can never be notarized.
#
# This signs inside-out -- every Mach-O, then the nested bundles, then the app.
# It relies on the installation being laid out so that Contents/MacOS holds
# only the soffice launcher (see scp2/source/ooo/common_brand.scp); codesign
# refuses to seal a bundle with anything else in there.

set -euo pipefail

SRCDIR=$(cd "$(dirname "$0")" && pwd)
IDENTITY="${MACOSX_CODESIGNING_IDENTITY:--}"
ENTITLEMENTS="$SRCDIR/mac-silicon-entitlements.plist"
HARDENED=no
VERIFY_ONLY=no
TARGETS=()

while [ $# -gt 0 ]; do
	case "$1" in
		-i|--identity)     IDENTITY="$2"; shift 2 ;;
		-e|--entitlements) ENTITLEMENTS="$2"; shift 2 ;;
		--hardened)        HARDENED=yes; shift ;;
		--verify)          VERIFY_ONLY=yes; shift ;;
		-h|--help)         sed -n '2,25p' "$0"; exit 0 ;;
		-*)                echo "unknown option: $1" >&2; exit 2 ;;
		*)                 TARGETS+=("$1"); shift ;;
	esac
done

[ ${#TARGETS[@]} -gt 0 ] || { echo "usage: $(basename "$0") [options] <app-or-dmg> ..." >&2; exit 2; }
[ "$IDENTITY" = "-" ] || HARDENED=yes

is_macho() {
	[ -f "$1" ] && [ ! -L "$1" ] && file -b "$1" 2>/dev/null | grep -q '^Mach-O'
}

sign_one() {
	local path="$1"; shift
	local args=(--force --sign "$IDENTITY" --timestamp=none)
	if [ "$IDENTITY" != "-" ]; then
		args=(--force --sign "$IDENTITY" --timestamp)
	fi
	if [ "$HARDENED" = yes ]; then
		args+=(--options runtime --entitlements "$ENTITLEMENTS")
	fi
	codesign "${args[@]}" "$@" "$path"
}

report() {
	local app="$1"
	echo "--- $app"
	codesign -dv --verbose=2 "$app" 2>&1 | grep -E 'Identifier|Format|CodeDirectory|Authority|TeamIdentifier|Sealed' || true
	if codesign --verify --deep --strict "$app" 2>/dev/null; then
		echo "verify: OK"
	else
		echo "verify: FAILED"
		codesign --verify --deep --strict --verbose=2 "$app" 2>&1 | tail -5
	fi
	spctl --assess --type exec --verbose=4 "$app" 2>&1 | tail -2 || true
}

sign_app() {
	local app="$1"
	echo "==> signing $app  (identity: $IDENTITY, hardened: $HARDENED)"

	# The installer stages every file read-only, and codesign has to rewrite them.
	chmod -R u+w "$app"
	# Quarantine and other xattrs make codesign fail or produce an unstable seal.
	xattr -cr "$app" 2>/dev/null || true

	# A bundle's main executable is signed as part of its bundle, not on its own:
	# codesign silently redirects such a path to the enclosing bundle.
	local bundles=() main_execs=() b exe
	while IFS= read -r b; do bundles+=("$b"); done < <(find "$app" \
		\( -name '*.app' -o -name '*.framework' -o -name '*.bundle' \
		-o -name '*.mdimporter' -o -name '*.plugin' -o -name '*.qlgenerator' \) \
		| awk '{ print gsub(/\//,"/") "\t" $0 }' | sort -rn | cut -f2-)
	for b in "${bundles[@]}"; do
		exe=$(/usr/libexec/PlistBuddy -c 'Print CFBundleExecutable' "$b/Contents/Info.plist" 2>/dev/null) || continue
		main_execs+=("$b/Contents/MacOS/$exe")
	done

	# 1. every Mach-O object, deepest path first
	local count=0
	while IFS= read -r f; do
		is_macho "$f" || continue
		case " ${main_execs[*]} " in *" $f "*) continue ;; esac
		sign_one "$f"
		count=$((count + 1))
	done < <(find "$app" -type f | awk '{ print gsub(/\//,"/") "\t" $0 }' | sort -rn | cut -f2-)
	echo "    signed $count Mach-O objects"

	# 2. nested bundles, deepest first, so each seal covers already-signed contents
	for b in "${bundles[@]}"; do
		[ "$b" = "$app" ] && continue
		sign_one "$b"
		echo "    sealed nested bundle: ${b#"$app"/}"
	done

	# 3. the app bundle itself
	sign_one "$app"
	echo "    sealed $app"
	report "$app"
}

for target in "${TARGETS[@]}"; do
	[ -e "$target" ] || { echo "no such path: $target" >&2; exit 1; }
	case "$target" in
		*.dmg)
			if [ "$VERIFY_ONLY" = yes ]; then report "$target"; continue; fi
			# A .dmg is signed as a whole; the .app inside must already be signed.
			if [ "$IDENTITY" = "-" ]; then
				echo "refusing to ad-hoc sign a .dmg (pointless); pass -i <Developer ID>" >&2
				exit 1
			fi
			codesign --force --sign "$IDENTITY" --timestamp "$target"
			report "$target"
			;;
		*)
			if [ "$VERIFY_ONLY" = yes ]; then report "$target"; continue; fi
			sign_app "$target"
			;;
	esac
done
