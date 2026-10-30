#!/usr/bin/env python3
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

# Black-box corpus builder: converts the same inputs with another office suite's headless binary once per output
# ODF version (1.2, 1.2 extended, 1.3, 1.3 extended), so an import smoke test can load each 1.3 file next to its
# 1.2 twin. Only observes output files; never reads that suite's source or installed configuration.
#
#   python3 -I make-foreign-corpus.py <soffice binary> [--inputs LIST] [--root DIR] [--modes v12,v13,...]
#   python3 -I make-foreign-corpus.py <soffice binary> --probe WORKDIR [--probe-range 0-20]   (value table only)
import argparse, os, re, signal, subprocess, sys, time, zipfile
from urllib.parse import quote

HERE = os.path.dirname(os.path.abspath(__file__))
TEST = os.path.dirname(HERE)  # test/odf-validate
JOBS = os.path.join(TEST, "runs", "gate-t16f", "jobs.tsv")
DEFAULT_ROOT = os.path.join(TEST, "runs", "phase3", "foreign")

# Observed black-box (--probe, one small .odt converted per value; office:version and manifest:version read back
# from the result, extension namespaces = loext/calcext/officeooo/drawooo/tableooo/field declared on the root):
#   1 -> 1.0 (no version attribute)   2 -> 1.1   4 -> 1.2 plain   8 -> 1.2 + extensions   9 -> 1.2 + extensions
#   10 -> 1.3 plain   11 -> 1.3 + extensions   12 -> 1.4 plain   13 -> 1.4 + extensions
#   0, 3, 5, 6, 7 and 14.. all wrote 1.4 + extensions (the default).
DEFAULT_VERSION_V12 = 4
DEFAULT_VERSION_V12EXT = 8
DEFAULT_VERSION_V13 = 10
DEFAULT_VERSION_V13EXT = 11

# mode -> (DefaultVersion value, expected office:version, whether extension namespaces are expected)
MODES = {
    "v12": (DEFAULT_VERSION_V12, "1.2", False),
    "v12ext": (DEFAULT_VERSION_V12EXT, "1.2", True),
    "v13": (DEFAULT_VERSION_V13, "1.3", False),
    "v13ext": (DEFAULT_VERSION_V13EXT, "1.3", True),
}
OUT_EXT = {"odt": "odt", "sxw": "odt", "doc": "odt", "docx": "odt", "rtf": "odt",
           "ods": "ods", "sxc": "ods", "xls": "ods", "xlsx": "ods",
           "odp": "odp", "sxi": "odp", "ppt": "odp", "pptx": "odp", "odg": "odg", "sxd": "odg"}

# Selection of the built-in list: per source extension a quota, filled round-robin across source directories
# (largest file first within a directory, 2 KB..2 MB) so the spread covers many components and not one folder.
# Quotas total 56 (<= 60); 55 are filled on this tree (the two testinput.odp share a name, so one is dropped):
# native ODF odt 14, ods 9, odp 3, odg 3 (every ods/odp/odg source in the list), legacy
# doc 6, xls 6, ppt 2, sxw 5, sxc 5, sxi 2. docx/rtf are left out: the aim is ODF 1.2/1.3 twins of ODF/legacy
# office sources, and OOXML import adds noise unrelated to ODF 1.3 import. The xmlsecurity signed-document
# variants are near duplicates, so the per-directory round-robin lets at most a few of them in.
QUOTA = [("odt", 14), ("ods", 9), ("odp", 4), ("odg", 3), ("doc", 6), ("xls", 6), ("ppt", 2),
         ("sxw", 5), ("sxc", 5), ("sxi", 2)]
MIN_SIZE, MAX_SIZE = 2048, 2 * 1024 * 1024

EXT_URI_RX = re.compile(r"documentfoundation|openoffice\.org/20(?:09|10)/|ooo-ms-interop:xmlns:field")


def builtin_selection(jobs=JOBS):
    seen, byext = set(), {}
    with open(jobs, encoding="utf8") as fh:
        for line in fh:
            p = line.split("\t", 1)[0].strip()
            if not p or p in seen or not os.path.isfile(p):
                continue
            seen.add(p)
            ext = p.rsplit(".", 1)[-1].lower()
            sz = os.path.getsize(p)
            if ext in OUT_EXT and MIN_SIZE <= sz <= MAX_SIZE:
                byext.setdefault(ext, []).append((p, sz))
    chosen, used = [], set()
    for ext, quota in QUOTA:
        dirs = {}
        for p, sz in byext.get(ext, []):
            dirs.setdefault(os.path.dirname(p), []).append((-sz, p))
        queues = [[p for _, p in sorted(v)] for _, v in sorted(dirs.items())]
        got = 0
        while got < quota and any(queues):
            for q in queues:
                if not q or got >= quota:
                    continue
                p = q.pop(0)
                key = (os.path.splitext(os.path.basename(p))[0], OUT_EXT[ext])
                if key in used:
                    continue
                used.add(key)
                chosen.append(p)
                got += 1
    return chosen


def read_list(path):
    out = []
    with open(path, encoding="utf8") as fh:
        for line in fh:
            p = line.split("\t", 1)[0].strip()
            if p and not p.startswith("#"):
                out.append(os.path.abspath(p))
    return out


def profile_url(d):
    return "file://" + quote(d)


def write_profile(d, value):
    os.makedirs(os.path.join(d, "user"), exist_ok=True)
    with open(os.path.join(d, "user", "registrymodifications.xcu"), "w", encoding="utf8") as fh:
        fh.write('<?xml version="1.0" encoding="UTF-8"?>\n'
                 '<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema"'
                 ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">\n'
                 '<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse">'
                 '<value>%d</value></prop></item>\n</oor:items>\n' % value)


def own_pids(profile_dirs):
    # Processes whose command line carries exactly one of our profile URLs (as a whole argument).
    toks = ["-env:UserInstallation=" + profile_url(d) for d in profile_dirs]
    r = subprocess.run(["ps", "-axww", "-o", "pid=,command="], capture_output=True, text=True)
    pids = []
    for line in r.stdout.splitlines():
        line = line.strip()
        if not line:
            continue
        pid, _, cmd = line.partition(" ")
        pid = int(pid)
        if pid == os.getpid():
            continue
        for t in toks:
            if re.search(re.escape(t) + r"(?:\s|$)", cmd):
                pids.append(pid)
                break
    return pids


def kill_own(profile_dirs):
    pids = own_pids(profile_dirs)
    for sig in (signal.SIGTERM, signal.SIGKILL):
        for pid in pids:
            try:
                os.kill(pid, sig)
            except ProcessLookupError:
                pass
        deadline = time.time() + 8
        while time.time() < deadline and own_pids(profile_dirs):
            time.sleep(0.5)
        pids = own_pids(profile_dirs)
        if not pids:
            return


def run_convert(soffice, prof, outext, outdir, files, timeout, log):
    cmd = [soffice, "-env:UserInstallation=" + profile_url(prof), "--headless", "--norestore",
           "--convert-to", outext, "--outdir", outdir] + files
    with open(log, "ab") as lf:
        lf.write(("\n$ " + " ".join(cmd) + "\n").encode())
        lf.flush()
        p = subprocess.Popen(cmd, stdout=lf, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL)
        try:
            p.wait(timeout=timeout)
            timed_out = False
        except subprocess.TimeoutExpired:
            timed_out = True
    # soffice.bin may outlive the launcher briefly; only our own profile is touched
    t0 = time.time()
    while own_pids([prof]) and time.time() - t0 < 20 and not timed_out:
        time.sleep(0.5)
    if timed_out or own_pids([prof]):
        kill_own([prof])
    return timed_out


def inspect(path):
    # -> (office:version of content.xml, manifest:version, declared extension namespace prefixes, problems)
    problems, vers, ext_ns, mv = [], {}, set(), None
    try:
        z = zipfile.ZipFile(path)
        for n in ("content.xml", "styles.xml", "meta.xml", "settings.xml"):
            if n not in z.namelist():
                continue
            head = z.read(n)[:20000].decode("utf8", "replace")
            m = re.search(r'office:version="([^"]*)"', head)
            vers[n] = m.group(1) if m else None
            if n in ("content.xml", "styles.xml"):
                for pre, uri in re.findall(r'xmlns:([\w.-]+)="([^"]*)"', head):
                    if EXT_URI_RX.search(uri):
                        ext_ns.add(pre)
        m = re.search(r'manifest:version="([^"]*)"', z.read("META-INF/manifest.xml").decode("utf8", "replace"))
        mv = m.group(1) if m else None
    except Exception as e:  # unreadable output is a failure, reported by the caller
        return None, None, set(), ["unreadable: %s" % e]
    cv = vers.get("content.xml")
    if len(set(vers.values())) > 1:
        problems.append("office:version differs between parts: %s" % vers)
    return cv, mv, ext_ns, problems


def probe(soffice, work, rng):
    lo, hi = rng
    src = os.path.join(work, "probe-input")
    os.makedirs(src, exist_ok=True)
    doc = os.path.join(src, "probe.odt")
    # minimal ODF 1.2 text document built here, so the probe needs no outside file
    with zipfile.ZipFile(doc, "w") as z:
        z.writestr("mimetype", "application/vnd.oasis.opendocument.text", compress_type=zipfile.ZIP_STORED)
        z.writestr("META-INF/manifest.xml",
                   '<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">'
                   '<manifest:file-entry manifest:full-path="/" manifest:media-type="application/vnd.oasis.opendocument.text"/>'
                   '<manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/></manifest:manifest>')
        z.writestr("content.xml",
                   '<?xml version="1.0" encoding="UTF-8"?>\n<office:document-content xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"'
                   ' xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" office:version="1.2"><office:body><office:text>'
                   '<text:p>probe</text:p></office:text></office:body></office:document-content>')
    dirs = [os.path.join(work, "probe-prof-%d" % n) for n in range(lo, hi + 1)]
    if own_pids(dirs):
        sys.exit("refusing to start: an office using one of the probe profiles is running")
    print("value\toffice:version\tmanifest:version\textension namespaces")
    for n, prof in zip(range(lo, hi + 1), dirs):
        write_profile(prof, n)
        out = os.path.join(work, "probe-out-%d" % n)
        os.makedirs(out, exist_ok=True)
        f = os.path.join(out, "probe.odt")
        if os.path.exists(f):
            os.remove(f)
        run_convert(soffice, prof, "odt", out, [doc], 300, os.path.join(work, "probe.log"))
        if not os.path.exists(f):
            print("%d\tNO OUTPUT" % n)
            continue
        cv, mv, ext, _ = inspect(f)
        print("%d\t%s\t%s\t%s" % (n, cv, mv, ",".join(sorted(ext)) or "-"))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("soffice", help="the other suite's soffice binary")
    ap.add_argument("--inputs", help="file with one input path per line (first tab column used; '#' comments)")
    ap.add_argument("--root", default=DEFAULT_ROOT, help="output root (default %(default)s)")
    ap.add_argument("--modes", default=",".join(MODES), help="comma list of modes (default all)")
    ap.add_argument("--batch", type=int, default=10, help="files per soffice call (default %(default)s)")
    ap.add_argument("--timeout", type=int, default=300, help="seconds per soffice call (default %(default)s)")
    ap.add_argument("--list-selection", action="store_true", help="print the built-in selection and exit")
    ap.add_argument("--probe", metavar="WORKDIR", help="only print the DefaultVersion value table, using WORKDIR")
    ap.add_argument("--probe-range", default="0-20")
    a = ap.parse_args()
    soffice = os.path.abspath(a.soffice)
    if a.list_selection:
        print("\n".join(builtin_selection()))
        return 0
    if not os.access(soffice, os.X_OK):
        sys.exit("not executable: %s" % soffice)
    if a.probe:
        lo, hi = map(int, a.probe_range.split("-"))
        probe(soffice, os.path.abspath(a.probe), (lo, hi))
        return 0

    modes = a.modes.split(",")
    for m in modes:
        if m not in MODES:
            sys.exit("unknown mode %s" % m)
    root = os.path.abspath(a.root)
    profs = {m: os.path.join(root, "foreign-prof-" + m) for m in modes}
    running = own_pids(list(profs.values()))
    if running:
        sys.exit("refusing to start: office process(es) %s already use one of our profile directories" % running)

    inputs = read_list(a.inputs) if a.inputs else builtin_selection()
    inputs = [p for p in inputs if os.path.isfile(p)]
    keys = {}
    for p in inputs:
        ext = p.rsplit(".", 1)[-1].lower()
        if ext not in OUT_EXT:
            sys.exit("unsupported input type: %s" % p)
        k = (os.path.splitext(os.path.basename(p))[0], OUT_EXT[ext])
        if k in keys:
            sys.exit("output name collision %s.%s: %s and %s" % (k[0], k[1], keys[k], p))
        keys[k] = p
    if not inputs:
        sys.exit("no input files")
    os.makedirs(os.path.join(root, "logs"), exist_ok=True)
    print("%d inputs, modes %s, root %s" % (len(inputs), ",".join(modes), root), flush=True)

    rows, short = [], False
    for m in modes:
        value, want, want_ext = MODES[m]
        write_profile(profs[m], value)
        outdir = os.path.join(root, m)
        os.makedirs(outdir, exist_ok=True)
        log = os.path.join(root, "logs", m + ".log")
        todo = {}  # outext -> [(input, expected output)]
        for p in inputs:
            oe = OUT_EXT[p.rsplit(".", 1)[-1].lower()]
            o = os.path.join(outdir, os.path.splitext(os.path.basename(p))[0] + "." + oe)
            if os.path.exists(o):
                os.remove(o)  # a stale file must not pass for a fresh conversion
            todo.setdefault(oe, []).append((p, o))
        for oe, items in todo.items():
            for i in range(0, len(items), a.batch):
                chunk = items[i:i + a.batch]
                to = run_convert(soffice, profs[m], oe, outdir, [p for p, _ in chunk], a.timeout, log)
                missing = [(p, o) for p, o in chunk if not os.path.exists(o)]
                if (to or missing) and len(chunk) > 1:
                    print("  %s: %d of %d missing after batch%s; retrying one by one" %
                          (m, len(missing), len(chunk), " (timeout)" if to else ""), flush=True)
                    for p, o in missing:
                        run_convert(soffice, profs[m], oe, outdir, [p], a.timeout, log)
        n_out = n_ok = 0
        for p in inputs:
            oe = OUT_EXT[p.rsplit(".", 1)[-1].lower()]
            o = os.path.join(outdir, os.path.splitext(os.path.basename(p))[0] + "." + oe)
            if not os.path.exists(o):
                rows.append((p, m, o, "", "", "fail", "no output"))
                continue
            n_out += 1
            cv, mv, ext, probs = inspect(o)
            if cv != want:
                probs.append("office:version %s, expected %s" % (cv, want))
            if mv != want:
                probs.append("manifest:version %s, expected %s" % (mv, want))
            if want_ext and not ext:
                probs.append("no extension namespaces declared")
            if not want_ext and ext:
                probs.append("unexpected extension namespaces %s" % ",".join(sorted(ext)))
            if not probs:
                n_ok += 1
            rows.append((p, m, o, cv or "", mv or "", "fail" if probs else "ok", "; ".join(probs)))
        print("%s (DefaultVersion %d): %d/%d outputs, %d verified as office:version %s" %
              (m, value, n_out, len(inputs), n_ok, want), flush=True)
        if n_out < len(inputs) or n_ok < n_out:
            short = True
    with open(os.path.join(root, "manifest.tsv"), "w", encoding="utf8") as fh:
        fh.write("input\tmode\toutput\toffice:version\tmanifest:version\tstatus\tnote\n")
        for r in rows:
            fh.write("\t".join(r) + "\n")
    bad = [r for r in rows if r[5] != "ok"]
    for r in bad[:20]:
        print("FAIL %s [%s]: %s" % (r[0], r[1], r[6]))
    if own_pids(list(profs.values())):
        print("warning: office processes of ours still running, killing them")
        kill_own(list(profs.values()))
    print("manifest: %s (%d rows, %d not ok)" % (os.path.join(root, "manifest.tsv"), len(rows), len(bad)))
    return 1 if (short or bad) else 0


if __name__ == "__main__":
    sys.exit(main())
