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

# usage: validate.py <outdir> <val.json>
# Validates every package in <outdir> against ODF 1.2 as written, against ODF 1.4 with the version rewritten ("14"),
# and against ODF 1.2 after removing foreign elements and attributes ("12x", Part 1 2.2.2 B.1; manifest left as written).
import sys, os, re, zipfile, subprocess, json, shutil, importlib.util

D = os.path.dirname(os.path.abspath(__file__))
JING = ["java", "-jar", os.environ.get("ODFV_JING", "/Applications/XMLauthor/lib/jing.jar"), "-i"]
SCH = {"12": (D + "/schema/OpenDocument-v1.2-os-schema.rng", D + "/schema/OpenDocument-v1.2-os-manifest-schema.rng"),
       "14": (D + "/schema/OpenDocument-v1.4-schema.rng", D + "/schema/OpenDocument-v1.4-manifest-schema.rng")}
SCH["12x"] = SCH["12"]
VERSIONS = ("12", "14", "12x")
_spec = importlib.util.spec_from_file_location("foreign", D + "/foreign.py")
foreign = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(foreign)
STREAMS = {"content.xml", "styles.xml", "meta.xml", "settings.xml"}
# ODF 1.2 Part 1 2.2.1 B.4: a content.xml rooted at MathML is validated against MathML, not ODF
ERR = re.compile(r"^(.*?):(\d+):(\d+): (error|fatal|warning): (.*)$")


def root_is_mathml(text):
    body = re.sub(r'<\?xml[^>]*\?>|<!DOCTYPE[^>]*>|<!--.*?-->', '', text[:4000], flags=re.S).lstrip()
    return bool(re.match(r'<(?:[\w.-]+:)?math[\s>/]', body))


def main():
    outdir, valjson = sys.argv[1], sys.argv[2]
    work = os.path.abspath(valjson[:-5] + ".xml")
    if not work.startswith(os.path.join(D, "runs") + os.sep):
        sys.exit("validate.py: <val.json> must live under " + os.path.join(D, "runs"))
    shutil.rmtree(work, ignore_errors=True)
    files = {v: ([], []) for v in VERSIONS}
    mixed = foreign.mixed_elements(SCH["12"][0])
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
                if v == "14":
                    attr = "manifest:version" if is_mani else "office:version"
                    t = re.sub(attr + '="1.2"', attr + '="1.4"', t, count=1)
                elif v == "12x" and not is_mani:
                    t = foreign.strip_foreign(t, mixed)
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
                r = subprocess.run(JING + [schema] + batch, capture_output=True, text=True)
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
    json.dump({"packages": pkgs, "bad_zip": bad_zip, "mathml_skipped": skipped,
               "nfiles": len(files["12"][0]) + len(files["12"][1]), "errors": errors}, open(valjson, "w"))
    for v in VERSIONS:
        print(f"ODF {v[0]}.{v[1]}{' extended, foreign content removed' if v == '12x' else ''}: {len(errors[v])} errors in {len({e[0].split('/')[0] for e in errors[v]})} of {len(pkgs)} packages")
    print(f"files validated: {len(files['12'][0]) + len(files['12'][1])}, MathML skipped: {len(skipped)}, bad zips: {bad_zip}")
    unparsed = [e for v in errors for e in errors[v] if e[0] == "?"]
    if unparsed:
        print("UNPARSED jing output:", *[e[3] for e in unparsed[:5]], sep="\n  ")


main()
