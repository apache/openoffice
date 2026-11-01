#!/bin/sh
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

# extract-linker-member.sh <archive.lib> <out.exp>
#
# Writes the FIRST LINKER MEMBER of a COFF archive to <out.exp>.  That member
# is the archive's symbol table, and ldump reads it to find out what a static
# library exports -- which is how every module using DEFLIB<n>NAME gets its
# .def file built.
#
# This used to be
#
#     lib.exe -EXTRACT:/ /OUT:<out.exp> <archive.lib>
#
# because the member is literally named "/".  Modern lib.exe does not extract
# it any more -- it matches -EXTRACT against the member paths that -LIST
# prints, and the linker members are not among them -- so it reports
#
#     LINK : warning LNK4014: member object "/" not found
#
# writes nothing, and still exits 0.  The failure only surfaces afterwards,
# when ldump is handed a file that does not exist.
#
# The layout being read here is fixed by the COFF archive format and has
# nothing to do with which compiler produced the file, so this does the same
# thing for every toolchain rather than branching on one:
#
#     offset  0   8 bytes    "!<arch>\n"
#     offset  8  60 bytes    member header, whose size field is 10 ASCII
#                            digits at offset 48 within it
#     offset 68  size bytes  the first linker member itself

set -e

archive="$1"
out="$2"

if [ -z "$archive" ] || [ -z "$out" ]; then
    echo "usage: $0 <archive.lib> <out.exp>" >&2
    exit 1
fi

if [ ! -f "$archive" ]; then
    echo "$0: no such archive: $archive" >&2
    exit 1
fi

# Guard the signature rather than trusting the extension: a non-archive would
# otherwise be silently truncated into a plausible-looking .exp.
signature=`dd if="$archive" bs=1 count=8 2>/dev/null`
if [ "$signature" != "!<arch>" ]; then
    echo "$0: $archive is not a COFF archive" >&2
    exit 1
fi

# Size of the first member, as 10 ASCII digits at absolute offset 56.
size=`dd if="$archive" bs=1 skip=56 count=10 2>/dev/null | tr -d ' '`
if [ -z "$size" ]; then
    echo "$0: cannot read the first member header of $archive" >&2
    exit 1
fi

# tail/head rather than dd, which would copy a multi-megabyte archive a byte
# at a time.
tail -c +69 "$archive" | head -c "$size" > "$out"
