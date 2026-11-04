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

# usage: python -m aootest.checkers.validate <outdir> <val.json>   (needs AOO_JING and AOO_SCHEMA_DIR; tests call check())
# Validates every package in <outdir> against ODF 1.2 as written, against ODF 1.3 and 1.4 with an office:version or
# manifest:version of 1.2 or 1.3 rewritten to the target ("13", "14"; any other version stays and is reported), and against
# ODF 1.2, 1.3 and 1.4 after removing foreign elements and attributes ("12x", "13x", "14x"; Part 1 2.2.2 B.1; manifest left
# as written apart from the version).
import sys, os, re, zipfile, subprocess, json

from aootest.checkers import foreign

SCHEMA_FILES = {"12": ("OpenDocument-v1.2-os-schema.rng", "OpenDocument-v1.2-os-manifest-schema.rng"),
                "13": ("OpenDocument-v1.3-schema.rng", "OpenDocument-v1.3-manifest-schema.rng"),
                "14": ("OpenDocument-v1.4-schema.rng", "OpenDocument-v1.4-manifest-schema.rng")}
VERSIONS = ("12", "14", "12x", "13", "13x", "14x")
STREAMS = {"content.xml", "styles.xml", "meta.xml", "settings.xml"}
# ODF 1.2 Part 1 2.2.1 B.4: a content.xml rooted at MathML is validated against MathML, not ODF
ERR = re.compile(r"^(.*?):(\d+):(\d+): (error|fatal|warning): (.*)$")


def root_is_mathml(text):
    body = re.sub(r'<\?xml[^>]*\?>|<!DOCTYPE[^>]*>|<!--.*?-->', '', text[:4000], flags=re.S).lstrip()
    return bool(re.match(r'<(?:[\w.-]+:)?math[\s>/]', body))


def schemas(schema_dir):
    sch = {v: tuple(os.path.join(schema_dir, f) for f in fs) for v, fs in SCHEMA_FILES.items()}
    sch["12x"], sch["13x"], sch["14x"] = sch["12"], sch["13"], sch["14"]
    return sch


def check(outdir, workdir, jing, schema_dir):
    """Returns the result that main() writes as <val.json>. `jing` is the command prefix (java, -jar, jar, -i);
    `workdir` must be empty or absent: the checker writes its working copies there and never deletes anything."""
    work = os.path.abspath(workdir)
    if os.path.isdir(work) and os.listdir(work):
        raise ValueError("validate: workdir is not empty: " + work)
    SCH = schemas(schema_dir)
    files = {v: ([], []) for v in VERSIONS}
    mixed = {v: foreign.mixed_elements(SCH[v][0]) for v in ("12x", "13x", "14x")}
    skipped, bad_zip = [], []
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith("."))
    for pkg in pkgs:
        try:
            z = zipfile.ZipFile(os.path.join(outdir, pkg))
        except Exception:
            bad_zip.append(pkg); continue
        for name in z.namelist():
            is_mani = name == "META-INF/manifest.xml"
            if name.rsplit("/", 1)[-1] not in STREAMS and not is_mani:
                continue
            text = z.read(name).decode("utf-8")
            if not is_mani and name.endswith("content.xml") and root_is_mathml(text):
                skipped.append(pkg + "/" + name); continue
            for v in VERSIONS:
                t = text
                if v in mixed and not is_mani:
                    t = foreign.strip_foreign(t, mixed[v])
                if v in ("13", "13x", "14", "14x"):
                    attr = "manifest:version" if is_mani else "office:version"
                    t = re.sub(attr + '="1\\.[23]"', attr + '="1.%s"' % v[1], t, count=1)
                p = os.path.join(work, v, pkg, name)
                os.makedirs(os.path.dirname(p), exist_ok=True)
                open(p, "w", encoding="utf-8").write(t)
                files[v][1 if is_mani else 0].append(p)
    errors = {}
    for v in VERSIONS:
        errs = []
        for schema, fl in ((SCH[v][0], files[v][0]), (SCH[v][1], files[v][1])):
            while fl:
                batch = fl[:400]
                r = subprocess.run(jing + [schema] + batch, capture_output=True, text=True)
                fl = fl[400:]
                for line in (r.stdout + r.stderr).splitlines():
                    m = ERR.match(line)
                    if m:
                        errs.append([os.path.relpath(m.group(1), os.path.join(work, v)), int(m.group(2)), int(m.group(3)), m.group(5)])
                        # a fatal error stops jing: resume with the files after the one that caused it
                        if m.group(4) == "fatal" and m.group(1) in batch:
                            fl = batch[batch.index(m.group(1)) + 1:] + fl
                    elif line.strip():
                        errs.append(["?", 0, 0, line.strip()])
        errors[v] = errs
    return {"packages": pkgs, "bad_zip": bad_zip, "mathml_skipped": skipped,
            "nfiles": len(files["12"][0]) + len(files["12"][1]), "errors": errors}


def main(argv=None):
    outdir, valjson = sys.argv[1:3] if argv is None else argv
    res = check(outdir, valjson[:-5] + ".xml", ["java", "-jar", os.environ["AOO_JING"], "-i"], os.environ["AOO_SCHEMA_DIR"])
    json.dump(res, open(valjson, "w"))
    errors, pkgs, skipped, bad_zip = res["errors"], res["packages"], res["mathml_skipped"], res["bad_zip"]
    for v in VERSIONS:
        print(f"ODF {v[0]}.{v[1]}{' extended, foreign content removed' if v.endswith('x') else ''}: {len(errors[v])} errors in {len({e[0].split('/')[0] for e in errors[v]})} of {len(pkgs)} packages")
    print(f"files validated: {res['nfiles']}, MathML skipped: {len(skipped)}, bad zips: {bad_zip}")
    unparsed = [e for v in errors for e in errors[v] if e[0] == "?"]
    if unparsed:
        print("UNPARSED jing output:", *[e[3] for e in unparsed[:5]], sep="\n  ")


if __name__ == "__main__":
    main()
