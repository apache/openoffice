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

# usage: crosscheck.py <outdir> <val.json> <cross.json>
# Validates every package in <outdir> with the Apache ODF Toolkit odfvalidator (fetch-odfvalidator.sh; MSV engine,
# the OASIS schemas given explicitly) in conformance mode (-c, compared with validate.py's "12") and extended
# conformance mode (-e, compared with "12x"), and compares per stream with the jing results in <val.json>.
# Error counts are not comparable between engines (different error recovery); the comparison is whether a stream
# has any error and where its first error is.
import sys, os, re, json, subprocess, collections
from xml.sax.saxutils import escape

D = os.path.dirname(os.path.abspath(__file__))
JAR = os.environ.get("ODFV_ODFVALIDATOR", D + "/tools/odfvalidator-1.1.8-incubating-jar-with-dependencies.jar")
JAVA = os.environ.get("ODFV_JAVA", "java")
SCH = D + "/schema/"
STREAMS = {"content.xml", "styles.xml", "meta.xml", "settings.xml", "manifest.xml"}
LINE = re.compile(r"^(?P<path>/.*?)(?:\[(?P<l>\d+),(?P<c>\d+)\])?:\s+(?P<lvl>Error|Fatal|Warning|Info):\s*(?P<msg>.*)$")
# odfvalidator needs a MathML 2 schema for embedded formula objects and its jar has none; this accepts anything,
# which is what validate.py does too (it skips MathML).
ANY = ('<grammar xmlns="http://relaxng.org/ns/structure/1.0"><start><ref name="e"/></start>'
       '<define name="e"><element><anyName/><interleave><zeroOrMore><attribute><anyName/></attribute></zeroOrMore>'
       '<zeroOrMore><choice><ref name="e"/><text/></choice></zeroOrMore></interleave></element></define></grammar>')


def run(flag, outdir, pkgs, work, tag):
    """Returns (events, crashed packages); events are (package, entry, line, col, level, message).
    Exits if odfvalidator stops without a completion line for every package: its own failures are silent otherwise."""
    open(work + "/any.rng", "w").write(ANY)
    events, crashed, todo, round_ = [], [], list(pkgs), 0
    while todo:
        round_ += 1
        cfg = work + "/%s.%d.xml" % (tag, round_)
        entries = {"schema": SCH + "OpenDocument-v1.2-os-schema.rng", "manifest-schema": SCH + "OpenDocument-v1.2-os-manifest-schema.rng",
                   "dsig-schema": SCH + "OpenDocument-v1.2-os-dsig-schema.rng", "mathml2-schema": work + "/any.rng"}
        for i, p in enumerate(todo):
            entries["path%06d" % i] = os.path.join(outdir, p)
        open(cfg, "w").write('<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE properties SYSTEM "http://java.sun.com/dtd/properties.dtd">\n<properties>\n'
                             + "".join('<entry key="%s">%s</entry>\n' % (k, escape(v)) for k, v in entries.items()) + "</properties>\n")
        r = subprocess.run([JAVA, "-jar", JAR, "-v", flag, "-C", cfg], capture_output=True, text=True, timeout=3600)
        text = r.stdout + r.stderr
        done = set()
        prefix = os.path.join(outdir, "")
        for line in text.splitlines():
            m = LINE.match(line)
            if not m or not m.group("path").startswith(prefix):
                continue
            rest = m.group("path")[len(prefix):]
            pkg, _, entry = rest.partition("/")
            if m.group("lvl") == "Info" and entry == "" and re.match(r"no errors|\d+ errors", m.group("msg")):
                done.add(pkg)
            if m.group("lvl") != "Info":
                events.append((pkg, entry, int(m.group("l") or 0), int(m.group("c") or 0), m.group("lvl"), m.group("msg")))
        pending = [p for p in todo if p not in done]
        if "Exception in thread" in text and pending:
            crashed.append(pending[0])
            todo = todo[todo.index(pending[0]) + 1:]
        elif pending:
            sys.exit("crosscheck.py: odfvalidator %s did not finish %d of %d packages (first: %s); output tail:\n%s"
                     % (flag, len(pending), len(todo), pending[0], "\n".join(text.splitlines()[-4:])))
        else:
            todo = []
    return events, crashed


def compare(events, jing_errors, pkgs):
    odf = collections.defaultdict(list)
    for pkg, entry, l, c, lvl, msg in events:
        if lvl in ("Error", "Fatal") and entry.rsplit("/", 1)[-1] in STREAMS:
            odf[pkg + "/" + entry].append((l, c, msg))
    jing = collections.defaultdict(list)
    for f, l, c, msg in jing_errors:
        if f != "?":
            jing[f].append((l, c, msg))
    both, same_pos, only_jing, only_odf = 0, 0, [], []
    diffpos = []
    for k in sorted(set(odf) | set(jing)):
        if k in odf and k in jing:
            both += 1
            if min(odf[k])[:2] == min(jing[k])[:2]:
                same_pos += 1
            else:
                diffpos.append([k, min(jing[k])[:2], min(odf[k])[:2], min(jing[k])[2][:100], min(odf[k])[2][:100]])
        elif k in jing:
            only_jing.append([k, min(jing[k])[:2], min(jing[k])[2][:140]])
        else:
            only_odf.append([k, min(odf[k])[:2], min(odf[k])[2][:140]])
    pkgmsgs = collections.Counter()
    for pkg, entry, l, c, lvl, msg in events:
        if entry.rsplit("/", 1)[-1] not in STREAMS and entry.rsplit("/", 1)[-1] != "":
            pkgmsgs["%s %s: %s" % (lvl, entry.rsplit("/", 1)[-1], re.sub(r"\d+", "N", msg)[:120])] += 1
        elif entry == "":
            pkgmsgs["%s (package): %s" % (lvl, re.sub(r"\d+", "N", msg)[:120])] += 1
    return {"odfvalidator_error_streams": sorted(odf), "streams_both_with_errors": both, "first_error_same_position": same_pos, "only_jing": only_jing, "only_odfvalidator": only_odf,
            "different_first_error": diffpos, "package_level": dict(pkgmsgs)}


def main():
    outdir, valjson, crossjson = os.path.abspath(sys.argv[1]), sys.argv[2], sys.argv[3]
    if not os.path.abspath(crossjson).startswith(os.path.join(D, "runs") + os.sep):
        sys.exit("crosscheck.py: <cross.json> must live under " + os.path.join(D, "runs"))
    if not os.path.isfile(JAR):
        sys.exit("crosscheck.py: " + JAR + " not found; run fetch-odfvalidator.sh or set ODFV_ODFVALIDATOR")
    work = os.path.abspath(crossjson[:-5] + ".d")
    os.makedirs(work, exist_ok=True)
    val = json.load(open(valjson))
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith(".") and p not in val.get("bad_zip", []))
    res = {}
    for flag, jv, name in (("-c", "12", "conformance"), ("-e", "12x", "extended")):
        events, crashed = run(flag, outdir, pkgs, work, name)
        res[name] = compare(events, val["errors"][jv], pkgs)
        res[name]["odfvalidator_crashed_on"] = crashed
        r = res[name]
        print(f"{name} (odfvalidator {flag} vs jing {jv}): both {r['streams_both_with_errors']} (same first position {r['first_error_same_position']}), "
              f"only jing {len(r['only_jing'])}, only odfvalidator {len(r['only_odfvalidator'])}, odfvalidator crashed on {len(crashed)} packages")
        for k, v in sorted(r["package_level"].items()):
            print(f"    {v:5d} {k}")
    json.dump(res, open(crossjson, "w"), indent=1)


main()
