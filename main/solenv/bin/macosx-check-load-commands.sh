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
# Fail when a Mach-O under the given paths still loads from or searches a
# build-time location: the CPython staging prefix or an unresolved @_______
# placeholder that macosx-change-install-names.pl should have rewritten.
#
#   ./solenv/bin/macosx-check-load-commands.sh <bundle-or-file> ...

set -euo pipefail

[ $# -gt 0 ] || { echo "usage: $(basename "$0") <bundle-or-file> ..." >&2; exit 2; }

machos=()
while IFS= read -r -d '' f && IFS= read -r type; do
	case "$type" in ": Mach-O"*) machos+=("$f") ;; esac
done < <(find "$@" -type f -print0 | xargs -0 file --no-pad --print0 -- 2>/dev/null)
# Every caller passes a real app bundle or binary that is expected to contain
# Mach-O content; zero matches means a wrong path or missing build output, not
# "nothing to check" - fail loudly instead of silently passing.
if [ ${#machos[@]} -eq 0 ]; then
	echo "no Mach-O files found under: $*" >&2
	exit 1
fi

# A dylib's own ID (LC_ID_DYLIB) is never used to locate it at runtime.
bad=$(printf '%s\0' "${machos[@]}" | xargs -0 otool -l | awk '
	/^[^[:space:]].*:$/ { file = $0; next }
	$1 == "cmd" { cmd = $2; next }
	cmd != "LC_ID_DYLIB" && ($1 == "name" || $1 == "path") && /python-inst|@_______/ {
		print file " " cmd " " $2
	}')
if [ -n "$bad" ]; then
	echo "unrelocated Mach-O load commands:" >&2
	echo "$bad" >&2
	exit 1
fi
