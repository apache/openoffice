################################################################
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
################################################################

"""Known deviations as data: rows of test_name<TAB>kind<TAB>reason in data/expect/*.tsv, applied as pytest marks.

test_name is the pytest item name including its parameter ids, e.g. test_formula_result[DCOUNT.zero.crit];
a '*' matches any characters. kind is xfail (the item is expected to fail; it is reported XPASS when it starts to pass, and
fails then under --strict-expect), flaky (it fails in some runs; never strict) or skip. The reason must say what is known, not guess."""
import re

from aootest.paths import DATA

EXPECT_DIR = DATA / "expect"
KINDS = ("xfail", "flaky", "skip")


class Row:
    def __init__(self, pattern, kind, reason, origin):
        self.pattern, self.kind, self.reason, self.origin = pattern, kind, reason, origin
        self.regex = re.compile("".join(".*" if c == "*" else re.escape(c) for c in pattern))
        self.used = 0


def load(directory=EXPECT_DIR):
    rows = []
    for path in sorted(directory.glob("*.tsv")):
        for n, line in enumerate(open(path, encoding="utf-8"), 1):
            if not line.strip() or line.startswith("#"):
                continue
            parts = line.rstrip("\n").split("\t")
            if len(parts) != 3 or parts[1] not in KINDS or not parts[2].strip():
                raise ValueError("%s:%d: expected test_name<TAB>xfail|flaky|skip<TAB>reason" % (path.name, n))
            rows.append(Row(parts[0], parts[1], parts[2], "%s:%d" % (path.name, n)))
    return rows


def match(rows, name):
    """The first row that matches the item name, or None."""
    for r in rows:
        if r.regex.fullmatch(name):
            return r
    return None
