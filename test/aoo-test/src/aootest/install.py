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

"""Finds an installed AOO from a path the user gives, reads its version, and finds a Python that can import uno.

A location may be an .app bundle, an install root, a program directory, or the soffice launcher itself. The platform
is read from the files found (versionrc or version.ini, Contents/MacOS), not from the host, so a layout copied from
another system is understood the same way.
"""
import configparser
import glob
import json
import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

MIN_VERSION = (5, 0)
MIN_WORKER_PYTHON = (3, 11)


class LayoutError(Exception):
    pass


class UnsupportedVersion(LayoutError):
    pass


class NoUnoPython(Exception):
    """No interpreter could import uno. `tried` is [(candidate, source, reason)]; `linked` is what libpyuno links, if known."""

    def __init__(self, message, tried=(), linked=None):
        super().__init__(message)
        self.tried, self.linked = list(tried), linked


@dataclass(frozen=True)
class Version:
    base: str            # OOOBaseVersion, e.g. "5.1"
    package: str         # OOOPackageVersion, e.g. "5.1.0" (may be "")
    major: int
    minor: int
    buildid: str
    revision: str

    @property
    def tuple(self):
        return (self.major, self.minor)


@dataclass(frozen=True)
class Layout:
    location: Path       # what the user gave, symlinks resolved
    program: Path
    soffice: Path        # the launcher to run
    unopkg: Path         # or None
    version_file: Path
    fundamental: Path
    version: Version
    platform: str        # "macos", "posix" or "windows", from the files found

    @property
    def pyuno_module(self):
        for n in ("pyuno.so", "pyuno.pyd"):
            if (self.program / n).is_file():
                return self.program / n
        return None


def parse_version(path):
    """Version of a versionrc or version.ini; raises LayoutError if it has none."""
    cp = configparser.RawConfigParser(interpolation=None)
    cp.optionxform = str
    try:
        cp.read(path, encoding="utf-8")
        sec = cp["Version"]
    except (configparser.Error, KeyError, OSError) as e:
        raise LayoutError("%s has no [Version] section: %s" % (path, e))
    base, major_minor = sec.get("OOOBaseVersion", ""), None
    m = re.match(r"(\d+)\.(\d+)", base)
    if m:
        major_minor = (int(m.group(1)), int(m.group(2)))
    elif sec.get("ProductMajor", "").isdigit():
        pm = int(sec["ProductMajor"])
        major_minor = (pm // 100, (pm // 10) % 10)
        base = "%d.%d" % major_minor
    if major_minor is None:
        raise LayoutError("%s: no OOOBaseVersion or ProductMajor" % path)
    return Version(base, sec.get("OOOPackageVersion", ""), major_minor[0], major_minor[1], sec.get("buildid", ""), sec.get("Revision", ""))


def _program_dirs(d):
    yield d
    yield d / "program"
    yield d / "Contents" / "program"
    if d.name == "MacOS" or d.name == "Contents":
        yield (d if d.name == "Contents" else d.parent) / "program"


def find_layout(location):
    """The Layout of the install at `location`; raises LayoutError, or UnsupportedVersion below 5.0."""
    p = Path(os.path.realpath(os.path.expanduser(str(location))))
    if not p.exists():
        raise LayoutError("no such file or directory: %s" % location)
    start = p.parent if p.is_file() else p
    program = vfile = None
    for d in _program_dirs(start):
        for n in ("versionrc", "version.ini"):
            if (d / n).is_file():
                program, vfile = d, d / n
                break
        if program:
            break
    if program is None:
        raise LayoutError("%s is not an OpenOffice install: no program/versionrc or version.ini found" % location)
    version = parse_version(vfile)
    if version.tuple < MIN_VERSION:
        raise UnsupportedVersion("AOO %s found at %s; this suite needs 5.0 or later" % (version.base, location))
    windows = vfile.name == "version.ini"
    macos = not windows and program.parent.name == "Contents"
    launcher = [program.parent / "MacOS" / "soffice"] if macos else []
    launcher += [program / n for n in (("soffice.exe", "soffice.bin") if windows else ("soffice", "soffice.bin"))]
    soffice = next((c for c in launcher if c.is_file()), None)
    if soffice is None:
        raise LayoutError("no soffice launcher in %s" % program)
    unopkg = next((program / n for n in (("unopkg.com", "unopkg.exe") if windows else ("unopkg", "unopkg.bin")) if (program / n).is_file()), None)
    fundamental = program / ("fundamental.ini" if windows else "fundamentalrc")
    return Layout(p, program, soffice, unopkg, vfile, fundamental, version, "windows" if windows else "macos" if macos else "posix")


def default_locations(environ=None):
    """Where installs usually are (a convenience; the location is normally given explicitly)."""
    env = os.environ if environ is None else environ
    pats = ["/Applications/OpenOffice*.app", "/opt/openoffice*", "/usr/local/openoffice*"]
    for var in ("ProgramFiles", "ProgramFiles(x86)"):
        if env.get(var):
            pats.append(os.path.join(env[var], "OpenOffice*"))
    return sorted(p for pat in pats for p in glob.glob(pat))


def discover(location=None, environ=None):
    """The Layout for `location`, else $AOO_OFFICE, else the newest 5.x install in the default places."""
    env = os.environ if environ is None else environ
    loc = location or env.get("AOO_OFFICE")
    if loc:
        return find_layout(loc)
    found = []
    for p in default_locations(env):
        try:
            found.append(find_layout(p))
        except LayoutError:
            continue
    if not found:
        raise LayoutError("no AOO 5 install found in the default locations; give the location of the app or binary")
    return max(found, key=lambda l: l.version.tuple)


# ---- a Python that can import uno -----------------------------------------------------------------------------

@dataclass(frozen=True)
class UnoPython:
    path: str
    version: str
    source: str


def parse_otool(text):
    """The Python framework path from `otool -L` output, or None."""
    for line in text.splitlines():
        m = re.match(r"\s*(\S*/Python\.framework/Versions/[\d.]+/Python|\S*/libpython[\d.]+\.dylib)\s", line)
        if m:
            return m.group(1)
    return None


def parse_ldd(text):
    """The libpython file name from `ldd` output, or None."""
    m = re.search(r"(libpython(\d+\.\d+)\w*\.so[\d.]*)", text)
    return m.group(1) if m else None


def linked_python(layout, runner=subprocess.run):
    """What libpyuno is linked against: a path (macOS), a library name (Linux, FreeBSD), or None."""
    lib = next((layout.program / n for n in ("libpyuno.dylib", "libpyuno.so") if (layout.program / n).is_file()), None)
    if lib is None or layout.platform == "windows":
        return None
    tool = ["otool", "-L"] if layout.platform == "macos" else ["ldd"]
    try:
        out = runner(tool + [str(lib)], capture_output=True, text=True, timeout=30).stdout
    except (OSError, subprocess.SubprocessError):
        return None
    return parse_otool(out) if layout.platform == "macos" else parse_ldd(out)


def interpreter_for(linked, which=shutil.which):
    """The interpreter that belongs to a linked framework path or libpython name, or None."""
    if not linked:
        return None
    m = re.search(r"(.*)/Python\.framework/Versions/([\d.]+)/Python$", linked)
    if m:
        d = Path(m.group(1)) / "Python.framework" / "Versions" / m.group(2) / "bin"
        for n in ("python" + m.group(2), "python3"):
            if (d / n).is_file():
                return str(d / n)
        return None
    m = re.search(r"libpython(\d+\.\d+)", linked)
    return which("python" + m.group(1)) if m else None


def candidates(layout, explicit=None, environ=None, which=shutil.which, linked=linked_python):
    """[(path, source)] in the order they are tried."""
    env = os.environ if environ is None else environ
    out = []
    if explicit or env.get("AOO_UNO_PYTHON"):
        return [(explicit or env["AOO_UNO_PYTHON"], "explicit")]
    for n in ("python", "python.exe", "python3", "python.bin"):
        if (layout.program / n).is_file():
            out.append((str(layout.program / n), "bundled program/" + n))
    for core in sorted(layout.program.glob("python-core-*")):
        for n in ("bin/python3", "bin/python", "python.exe"):
            if (core / n).is_file():
                out.append((str(core / n), "bundled " + core.name))
    link = linked(layout)
    mapped = interpreter_for(link, which)
    if mapped:
        out.append((mapped, "linked by libpyuno (%s)" % link))
    out.append((sys.executable, "the Python running pytest"))
    for minor in range(15, 10, -1):
        w = which("python3.%d" % minor)
        if w:
            out.append((w, "python3.%d on PATH" % minor))
    seen, uniq = set(), []
    for path, src in out:
        key = os.path.realpath(path) if os.path.exists(path) else path
        if key not in seen:
            seen.add(key)
            uniq.append((path, src))
    return uniq


def probe_env(layout, base=None):
    env = dict(os.environ if base is None else base)
    env["URE_BOOTSTRAP"] = "vnd.sun.star.pathname:" + str(layout.fundamental)
    env["PYTHONPATH"] = os.pathsep.join([str(layout.program)] + ([env["PYTHONPATH"]] if env.get("PYTHONPATH") else []))
    var = {"macos": "DYLD_FALLBACK_LIBRARY_PATH", "posix": "LD_LIBRARY_PATH", "windows": "PATH"}[layout.platform]
    env[var] = os.pathsep.join([str(layout.program)] + ([env[var]] if env.get(var) else []))
    return env


PROBE_CODE = "import sys, uno, pyuno; print('%d.%d.%d' % sys.version_info[:3])"


def probe(python, layout, runner=subprocess.run, timeout=60):
    """(True, version) if `python` imports uno and pyuno against this install, else (False, reason)."""
    try:
        r = runner([python, "-c", PROBE_CODE], env=probe_env(layout), capture_output=True, text=True, timeout=timeout)
    except (OSError, subprocess.SubprocessError) as e:
        return False, "could not run: %s" % e
    if r.returncode != 0:
        tail = (r.stderr or r.stdout).strip().splitlines()
        return False, tail[-1] if tail else "exit %d" % r.returncode
    ver = r.stdout.strip()
    parts = tuple(int(x) for x in ver.split(".")[:2]) if re.fullmatch(r"\d+\.\d+\.\d+", ver) else None
    if parts is None:
        return False, "unexpected output %r" % ver
    if parts < MIN_WORKER_PYTHON:
        return False, "Python %s is older than %d.%d" % (ver, *MIN_WORKER_PYTHON)
    return True, ver


def _cache_key(layout):
    mod = layout.pyuno_module
    return "%s|%s" % (layout.program, mod.stat().st_mtime_ns if mod else 0)


def find_uno_python(layout, explicit=None, cache_file=None, environ=None, which=shutil.which, linked=linked_python, prober=probe):
    """The first candidate that passes a real probe. An explicit choice (argument or AOO_UNO_PYTHON) is the only candidate: a
    wrong override is reported, not silently replaced. The result is cached per install and pyuno build."""
    cache = {}
    if cache_file and Path(cache_file).is_file():
        try:
            cache = json.loads(Path(cache_file).read_text())
        except (OSError, ValueError):
            cache = {}
    key = _cache_key(layout)
    hit = cache.get(key)
    if hit and not explicit and not (environ or os.environ).get("AOO_UNO_PYTHON") and os.path.exists(hit["path"]):
        return UnoPython(hit["path"], hit["version"], hit["source"] + " (cached)")
    cands = candidates(layout, explicit, environ, which, linked)
    tried = []
    for path, source in cands:
        ok, detail = prober(path, layout)
        if ok:
            res = UnoPython(path, detail, source)
            if cache_file:
                try:
                    cache[key] = {"path": path, "version": detail, "source": source}
                    Path(cache_file).parent.mkdir(parents=True, exist_ok=True)
                    Path(cache_file).write_text(json.dumps(cache))
                except OSError:
                    pass
            return res
        tried.append((path, source, detail))
    link = linked(layout)
    need = ("; libpyuno is linked against %s%s" % (link, "" if link and os.path.exists(link) else " (not found on this machine)")) if link else ""
    raise NoUnoPython("no Python can import uno from %s%s" % (layout.program, need), tried, link)
