#!/bin/bash
#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************
#
# Fail when a Mach-O under the given paths still loads from or searches the
# CPython staging prefix, which does not exist outside the build tree.
#
#   ./solenv/bin/macosx-check-load-commands.sh <bundle-or-file> ...
#
# Unresolved @_______ placeholders are not rejected: gbuild executables such as
# uno.bin and regcomp.bin still ship them and load only via DYLD_LIBRARY_PATH.

set -euo pipefail

[ $# -gt 0 ] || { echo "usage: $(basename "$0") <bundle-or-file> ..." >&2; exit 2; }

machos=()
while IFS= read -r -d '' f && IFS= read -r type; do
	case "$type" in ": Mach-O"*) machos+=("$f") ;; esac
done < <(find "$@" -type f -print0 | xargs -0 file --no-pad --print0 -- 2>/dev/null)
[ ${#machos[@]} -gt 0 ] || exit 0

# A dylib's own ID (LC_ID_DYLIB) is never used to locate it at runtime.
bad=$(printf '%s\0' "${machos[@]}" | xargs -0 otool -l | awk '
	/^[^[:space:]].*:$/ { file = $0; next }
	$1 == "cmd" { cmd = $2; next }
	cmd != "LC_ID_DYLIB" && ($1 == "name" || $1 == "path") && /python-inst/ {
		print file " " cmd " " $2
	}')
if [ -n "$bad" ]; then
	echo "unrelocated Mach-O load commands:" >&2
	echo "$bad" >&2
	exit 1
fi
