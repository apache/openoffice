#!/bin/bash
#
# Code-sign a built Apache OpenOffice .app (or .dmg) on macOS.
#
#   ./solenv/bin/macosx-codesign.sh [options] <OpenOffice.app|installer.dmg> ...
#
# Options:
#   -i, --identity ID   codesign identity; "-" = ad-hoc (default, or
#                       $MACOSX_CODESIGNING_IDENTITY when set)
#   -e, --entitlements  entitlements plist (default: macosx-codesign-entitlements.plist)
#   -k, --keychain PATH keychain holding the identity (default: the search list)
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
ENTITLEMENTS="$SRCDIR/macosx-codesign-entitlements.plist"
KEYCHAIN="${MACOSX_CODESIGNING_KEYCHAIN:-}"
HARDENED=no
VERIFY_ONLY=no
TARGETS=()

while [ $# -gt 0 ]; do
	case "$1" in
		-i|--identity)
			[ $# -ge 2 ] || { echo "$1 requires an argument" >&2; exit 2; }
			IDENTITY="$2"; shift 2 ;;
		-e|--entitlements)
			[ $# -ge 2 ] || { echo "$1 requires an argument" >&2; exit 2; }
			ENTITLEMENTS="$2"; shift 2 ;;
		-k|--keychain)
			[ $# -ge 2 ] || { echo "$1 requires an argument" >&2; exit 2; }
			KEYCHAIN="$2"; shift 2 ;;
		--hardened)        HARDENED=yes; shift ;;
		--verify)          VERIFY_ONLY=yes; shift ;;
		-h|--help)         sed -n '2,25p' "$0"; exit 0 ;;
		-*)                echo "unknown option: $1" >&2; exit 2 ;;
		*)                 TARGETS+=("$1"); shift ;;
	esac
done

[ ${#TARGETS[@]} -gt 0 ] || { echo "usage: $(basename "$0") [options] <app-or-dmg> ..." >&2; exit 2; }
[ "$IDENTITY" = "-" ] || HARDENED=yes

sign_one() {
	local path="$1"; shift
	local args=(--force --sign "$IDENTITY" --timestamp=none)
	if [ "$IDENTITY" != "-" ]; then
		args=(--force --sign "$IDENTITY" --timestamp)
	fi
	if [ -n "$KEYCHAIN" ]; then
		args+=(--keychain "$KEYCHAIN")
	fi
	codesign "${args[@]}" "$@" "$path"
}

sign_executable() {
	local path="$1"; shift
	if [ "$HARDENED" = yes ]; then
		sign_one "$path" --options runtime --entitlements "$ENTITLEMENTS" "$@"
	else
		sign_one "$path" "$@"
	fi
}

sign_disk_image() {
	local path="$1"
	local args=(--force --sign "$IDENTITY" --timestamp)
	if [ -n "$KEYCHAIN" ]; then
		args+=(--keychain "$KEYCHAIN")
	fi
	codesign "${args[@]}" "$path"
}

report() {
	local target="$1"
	echo "--- $target"
	codesign -dv --verbose=2 "$target" 2>&1 | grep -E 'Identifier|Format|CodeDirectory|Authority|TeamIdentifier|Sealed' || true
	if ! codesign --verify --deep --strict --verbose=2 "$target"; then
		echo "verify: FAILED" >&2
		return 1
	fi
	echo "verify: OK"
	case "$target" in
		*.dmg) spctl --assess --type open --context context:primary-signature --verbose=4 "$target" || true ;;
		*)     spctl --assess --type exec --verbose=4 "$target" || true ;;
	esac
}

# codesign rewrites every Mach-O it signs and writes _CodeSignature/ into
# every bundle it seals; the installer stages files read-only. Open just those
# for writing rather than the whole tree, so the staged UnixRights survive.
open_for_signing() {
	local b="$1" d
	for d in "$b" "$b/Contents" "$b"/Versions/*/; do
		if [ -d "$d" ]; then chmod u+w "$d"; fi
	done
	find "$b" -maxdepth 3 -name _CodeSignature -exec chmod -R u+w {} + 2>/dev/null || true
}

sign_app() {
	local app="$1"
	[ -d "$app/Contents" ] && [ -f "$app/Contents/Info.plist" ] || {
		echo "not an application bundle: $app" >&2
		return 1
	}
	echo "==> signing $app  (identity: $IDENTITY, hardened: $HARDENED)"

	# Quarantine and other xattrs make codesign fail or produce an unstable seal.
	xattr -cr "$app" 2>/dev/null || true

	# A bundle's main executable is signed as part of its bundle, not on its own:
	# codesign silently redirects such a path to the enclosing bundle.
	local bundles=() main_execs=() b exe
	while IFS= read -r b; do bundles+=("$b"); done < <(find "$app" \
		\( -name '*.app' -o -name '*.framework' -o -name '*.bundle' \
		-o -name '*.mdimporter' -o -name '*.plugin' -o -name '*.qlgenerator' \) \
		| awk '{ print gsub(/\//,"/") "\t" $0 }' | sort -rn | cut -f2-)
	for b in ${bundles[@]+"${bundles[@]}"}; do
		exe=$(/usr/libexec/PlistBuddy -c 'Print CFBundleExecutable' "$b/Contents/Info.plist" 2>/dev/null) || continue
		main_execs+=("$b/Contents/MacOS/$exe")
	done

	# Find every Mach-O with one batched file(1) run instead of a process per
	# file (an installation holds ~10k). --print0 emits "path\0: type\n".
	local machos=() executable_machos=() f type
	while IFS= read -r -d '' f && IFS= read -r type; do
		case "$type" in
			": Mach-O"*)
				machos+=("$f")
				case "$type" in *" executable"*) executable_machos+=("$f") ;; esac
				;;
		esac
	done < <(find "$app" -type f -print0 | xargs -0 file --no-pad --print0 -- 2>/dev/null)
	"$SRCDIR/macosx-check-load-commands.sh" "$app"
	# codesign rewrites a Mach-O through a temporary file beside it, so the
	# containing directory has to be writable as well.
	if [ ${#machos[@]} -gt 0 ]; then
		local dirs=()
		for f in "${machos[@]}"; do dirs+=("${f%/*}"); done
		chmod u+w "${machos[@]}"
		printf '%s\n' "${dirs[@]}" | sort -u | tr '\n' '\0' | xargs -0 chmod u+w
	fi
	for b in ${bundles[@]+"${bundles[@]}"}; do open_for_signing "$b"; done

	# 1. every Mach-O object, deepest path first. Process entitlements belong
	# on executables; dylibs and plug-ins inherit the host process's policy.
	local count=0
	if [ ${#machos[@]} -gt 0 ]; then
		while IFS= read -r f; do
			case " ${main_execs[*]-} " in *" $f "*) continue ;; esac
			case " ${executable_machos[*]-} " in
				*" $f "*) sign_executable "$f" ;;
				*)         sign_one "$f" ;;
			esac
			count=$((count + 1))
		done < <(printf '%s\n' "${machos[@]}" | awk '{ print gsub(/\//,"/") "\t" $0 }' | sort -rn | cut -f2-)
	fi
	echo "    signed $count Mach-O objects"

	# 2. nested bundles, deepest first, so each seal covers already-signed contents
	for b in ${bundles[@]+"${bundles[@]}"}; do
		[ "$b" = "$app" ] && continue
		case "$b" in
			*.app) sign_executable "$b" ;;
			*)     sign_one "$b" ;;
		esac
		echo "    sealed nested bundle: ${b#"$app"/}"
	done

	# 3. the app bundle itself
	sign_executable "$app"
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
			sign_disk_image "$target"
			report "$target"
			;;
		*)
			if [ "$VERIFY_ONLY" = yes ]; then report "$target"; continue; fi
			sign_app "$target"
			;;
	esac
done
