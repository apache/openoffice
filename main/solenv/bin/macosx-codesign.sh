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
#       --notarize PROFILE
#                       after signing, submit to the Apple notary service with
#                       the "xcrun notarytool store-credentials" keychain
#                       profile PROFILE and staple the ticket (.dmg, or a .app
#                       zipped for submission); needs a real identity
#       --release       fail if the Gatekeeper (spctl) assessment rejects the
#                       result, instead of only reporting it; ignored for an
#                       ad-hoc identity, which spctl always rejects regardless
#                       of notarization
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
NOTARY_PROFILE=""
HARDENED=no
RELEASE=no
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
		--notarize)
			[ $# -ge 2 ] || { echo "$1 requires an argument" >&2; exit 2; }
			NOTARY_PROFILE="$2"; shift 2 ;;
		--release)         RELEASE=yes; shift ;;
		--verify)          VERIFY_ONLY=yes; shift ;;
		-h|--help)         sed -n '2,33p' "$0"; exit 0 ;;
		-*)                echo "unknown option: $1" >&2; exit 2 ;;
		*)                 TARGETS+=("$1"); shift ;;
	esac
done

[ ${#TARGETS[@]} -gt 0 ] || { echo "usage: $(basename "$0") [options] <app-or-dmg> ..." >&2; exit 2; }
[ "$IDENTITY" = "-" ] || HARDENED=yes
if [ -n "$NOTARY_PROFILE" ] && [ "$IDENTITY" = "-" ]; then
	echo "--notarize needs a Developer ID identity, not an ad-hoc signature" >&2
	exit 2
fi

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

# notarytool exits 0 even when the notary service rejects the submission, so
# a successful staple is the only proof the ticket exists. A .app cannot be
# submitted as-is; it goes up zipped and the ticket is stapled to the bundle.
notarize() {
	local target="$1" upload="$1" zip=""
	case "$target" in
		*.dmg) ;;
		*)
			zip=$(mktemp -d)/$(basename "$target").zip
			ditto -c -k --keepParent "$target" "$zip"
			upload="$zip" ;;
	esac
	echo "==> notarizing $target  (profile: $NOTARY_PROFILE)"
	if ! xcrun notarytool submit --wait --keychain-profile "$NOTARY_PROFILE" "$upload"; then
		echo "notarization submission failed" >&2
		[ -z "$zip" ] || rm -rf "$(dirname "$zip")"
		return 1
	fi
	[ -z "$zip" ] || rm -rf "$(dirname "$zip")"
	if ! xcrun stapler staple "$target"; then
		echo "stapling failed: the submission was probably rejected; run 'xcrun notarytool log' for details" >&2
		return 1
	fi
	echo "    stapled notarization ticket to $target"
	if ! xcrun stapler validate "$target"; then
		echo "stapler validate: FAILED (staple reported success but does not validate)" >&2
		return 1
	fi
	echo "    stapled ticket validates"
}

# hdiutil create/codesign only ever touch the staged tree before it goes into
# the image; nothing checks the *finished* image's own structure, or that the
# application actually mounts and verifies from inside it. See the "older
# hdiutil makehybrid" note above sign_app() for why that distinction matters.
verify_dmg_contents() {
	local dmg="$1" mnt rc=0 app
	if ! hdiutil verify "$dmg" >/dev/null; then
		echo "hdiutil verify: FAILED" >&2
		return 1
	fi
	echo "hdiutil verify: OK"

	mnt=$(hdiutil attach -readonly -nobrowse "$dmg" | tail -1 | awk -F'\t' '{print $NF}')
	if [ -z "$mnt" ] || [ ! -d "$mnt" ]; then
		echo "could not mount $dmg to verify its contents" >&2
		return 1
	fi
	for app in "$mnt"/*.app; do
		[ -d "$app" ] || continue
		if ! codesign --verify --deep --strict --verbose=2 "$app"; then
			echo "enclosed application verify: FAILED ($app)" >&2
			rc=1
		fi
	done
	hdiutil detach "$mnt" -quiet
	if [ "$rc" -eq 0 ]; then
		echo "    enclosed application verifies from the mounted image"
	fi
	return "$rc"
}

report() {
	local target="$1" spctl_rc=0 dv
	echo "--- $target"
	dv=$(codesign -dv --verbose=2 "$target" 2>&1) || true
	printf '%s\n' "$dv" | grep -E 'Identifier|Format|CodeDirectory|Authority|TeamIdentifier|Sealed' || true
	if ! codesign --verify --deep --strict --verbose=2 "$target"; then
		echo "verify: FAILED" >&2
		return 1
	fi
	echo "verify: OK"
	case "$target" in
		*.dmg)
			verify_dmg_contents "$target" || return 1
			spctl --assess --type open --context context:primary-signature --verbose=4 "$target" || spctl_rc=$? ;;
		*)     spctl --assess --type exec --verbose=4 "$target" || spctl_rc=$? ;;
	esac
	# spctl rejects every ad-hoc signature outright, notarized or not, so only
	# a real signature's rejection is meaningful enough to fail the build on.
	# Detect that from the target's own signature (an "Authority=" line means a
	# real CA-chained identity; ad-hoc has none) rather than the -i/env
	# default, so this is also correct when --verify checks an
	# already-signed artifact without re-passing -i.
	if [ "$spctl_rc" -ne 0 ] && [ "$RELEASE" = yes ] && printf '%s\n' "$dv" | grep -q 'Authority='; then
		echo "spctl: FAILED (fatal under --release)" >&2
		return 1
	fi
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
			[ -z "$NOTARY_PROFILE" ] || notarize "$target"
			report "$target"
			;;
		*)
			if [ "$VERIFY_ONLY" = yes ]; then report "$target"; continue; fi
			sign_app "$target"
			[ -z "$NOTARY_PROFILE" ] || notarize "$target"
			# spctl rejects a Developer ID app that isn't notarized yet, so this
			# must run after notarize, not inside sign_app() - see the .dmg
			# branch above, which already had this order right.
			report "$target"
			;;
	esac
done
