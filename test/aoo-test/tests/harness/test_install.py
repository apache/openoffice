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

"""install.py against fake macOS, Linux and Windows trees, and the UNO Python search with a faked probe."""
import os
import subprocess
import sys

import pytest

from aootest import install

pytestmark = pytest.mark.selftest

FORMS = {
    "macos": [lambda a: a, lambda a: a / "Contents", lambda a: a / "Contents" / "MacOS" / "soffice", lambda a: a / "Contents" / "program",
              lambda a: a / "Contents" / "program" / "soffice"],
    "posix": [lambda r: r, lambda r: r / "program", lambda r: r / "program" / "soffice", lambda r: r / "program" / "soffice.bin"],
    "windows": [lambda r: r, lambda r: r / "program", lambda r: r / "program" / "soffice.exe"],
}
CASES = [pytest.param(kind, i, id="%s-form%d" % (kind, i)) for kind, forms in FORMS.items() for i in range(len(forms))]


@pytest.mark.parametrize("kind, form", CASES)
def test_every_way_of_pointing_at_an_install_finds_the_same_layout(fake, kind, form):
    top = fake(kind)
    lay = install.find_layout(FORMS[kind][form](top))
    assert lay.platform == kind
    assert lay.version.base == "5.1" and lay.version.package == "5.1.0" and lay.version.tuple == (5, 1)
    assert lay.program.name == "program" and lay.program.parent.name in ("Contents", top.name)
    assert lay.version_file.name == ("version.ini" if kind == "windows" else "versionrc")
    assert lay.fundamental.name == ("fundamental.ini" if kind == "windows" else "fundamentalrc")
    assert lay.soffice.name == ("soffice.exe" if kind == "windows" else "soffice")
    assert (lay.soffice.parent.name == "MacOS") == (kind == "macos")
    assert lay.unopkg is not None and lay.unopkg.name.startswith("unopkg")
    assert lay.pyuno_module.name == ("pyuno.pyd" if kind == "windows" else "pyuno.so")


def test_windows_prefers_the_console_unopkg(fake):
    assert install.find_layout(fake("windows")).unopkg.name == "unopkg.com"


def test_version_from_productmajor_alone(fake):
    lay = install.find_layout(fake("macos", with_base=False, major="510"))
    assert (lay.version.base, lay.version.tuple) == ("5.1", (5, 1))


def test_newer_major_is_accepted(fake):
    assert install.find_layout(fake("posix", base="6.0", pkg="6.0.0", major="600", minor="0")).version.tuple == (6, 0)


@pytest.mark.parametrize("kind", ["macos", "posix", "windows"])
def test_aoo_4_is_refused(fake, kind):
    with pytest.raises(install.UnsupportedVersion, match="5.0 or later"):
        install.find_layout(fake(kind, base="4.1", pkg="4.1.17", major="410", minor="1"))


def test_not_an_install(tmp_path):
    (tmp_path / "program").mkdir()
    with pytest.raises(install.LayoutError, match="not an OpenOffice install"):
        install.find_layout(tmp_path)
    with pytest.raises(install.LayoutError, match="no such file"):
        install.find_layout(tmp_path / "missing")


def test_version_file_without_a_version(fake):
    top = fake("posix")
    (top / "program" / "versionrc").write_text("[Version]\nProductSource=AOO510\n")
    with pytest.raises(install.LayoutError, match="no OOOBaseVersion"):
        install.find_layout(top)


def test_missing_launcher(fake):
    top = fake("posix")
    for n in ("soffice", "soffice.bin"):
        (top / "program" / n).unlink()
    with pytest.raises(install.LayoutError, match="no soffice launcher"):
        install.find_layout(top)


def test_discover_prefers_the_location_then_the_environment_then_the_default_places(fake, monkeypatch, tmp_path):
    five = fake("posix")
    assert install.discover(str(five)).version.tuple == (5, 1)
    assert install.discover(None, {"AOO_OFFICE": str(five)}).location == five.resolve()
    four = tmp_path / "openoffice4"
    (four / "program").mkdir(parents=True)
    (four / "program" / "soffice").write_text("x")
    (four / "program" / "versionrc").write_text("[Version]\nOOOBaseVersion=4.1\n")
    monkeypatch.setattr(install, "default_locations", lambda environ=None: [str(four), str(five), str(tmp_path / "nothing")])
    assert install.discover(None, {}).location == five.resolve()        # the 4.x one is skipped, the missing one ignored


def test_discover_with_nothing_found(monkeypatch):
    monkeypatch.setattr(install, "default_locations", lambda environ=None: [])
    with pytest.raises(install.LayoutError, match="give the location"):
        install.discover(None, {})


# ---- parsing what libpyuno links ---------------------------------------------------------------------------------

OTOOL = """/Applications/OpenOffice5.app/Contents/program/libpyuno.dylib:
\t@_______OOO/libpyuno.dylib (compatibility version 0.0.0, current version 0.0.0)
\t@loader_path/libuno_cppu.dylib (compatibility version 0.0.0, current version 0.0.0)
\t/opt/local/Library/Frameworks/Python.framework/Versions/3.14/Python (compatibility version 3.14.0, current version 3.14.0)
\t/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1351.0.0)
"""
LDD = "\tlinux-vdso.so.1 (0x0000)\n\tlibpython3.11.so.1.0 => /usr/lib/libpython3.11.so.1.0 (0x00007f)\n\tlibc.so.6 => /lib/libc.so.6 (0x0000)\n"


def test_parse_otool_and_ldd():
    assert install.parse_otool(OTOOL) == "/opt/local/Library/Frameworks/Python.framework/Versions/3.14/Python"
    assert install.parse_otool("\t/usr/lib/libSystem.B.dylib (x)\n") is None
    assert install.parse_ldd(LDD) == "libpython3.11.so.1.0"
    assert install.parse_ldd("libc.so.6 => /lib/libc.so.6\n") is None


def test_interpreter_for_a_framework_path(tmp_path):
    fw = tmp_path / "Library/Frameworks/Python.framework/Versions/3.14"
    (fw / "bin").mkdir(parents=True)
    linked = str(fw / "Python")
    assert install.interpreter_for(linked) is None                         # no interpreter there
    (fw / "bin" / "python3").write_text("x")
    assert install.interpreter_for(linked) == str(fw / "bin" / "python3")
    (fw / "bin" / "python3.14").write_text("x")
    assert install.interpreter_for(linked) == str(fw / "bin" / "python3.14")   # the exact version wins
    assert install.interpreter_for("libpython3.11.so.1.0", which=lambda n: "/usr/bin/" + n) == "/usr/bin/python3.11"
    assert install.interpreter_for(None) is None


# ---- the search ----------------------------------------------------------------------------------------------------

def searcher(top, passing, linked=None, environ=None):
    """(layout, asked, find): find() runs find_uno_python with a probe that passes only for `passing` and logs each question."""
    lay = install.find_layout(top)
    asked = []

    def prober(python, layout):
        asked.append(python)
        return (True, "3.14.0") if python in passing else (False, "No module named 'uno'")

    def find(**kw):
        return install.find_uno_python(lay, prober=prober, linked=lambda l: linked, which=lambda n: None, environ=environ or {}, **kw)
    return lay, asked, find


def test_bundled_python_is_tried_before_the_system_ones(fake):
    top = fake("posix")
    (top / "program" / "python").write_text("x")
    py = str(top / "program" / "python")
    lay, asked, find = searcher(top, {py, sys.executable})
    res = find()
    assert res.path == py and res.source == "bundled program/python" and asked == [py]


def test_falls_through_to_the_interpreter_libpyuno_links(fake, tmp_path):
    top = fake("macos")
    fw = tmp_path / "fw/Python.framework/Versions/3.14"
    (fw / "bin").mkdir(parents=True)
    (fw / "bin" / "python3.14").write_text("x")
    lay, asked, find = searcher(top, {str(fw / "bin" / "python3.14")}, linked=str(fw / "Python"))
    res = find()
    assert res.path == str(fw / "bin" / "python3.14") and res.source.startswith("linked by libpyuno")
    assert asked[0] == res.path                                              # nothing bundled, so the linked one is first


def test_explicit_python_is_the_only_candidate(fake):
    top = fake("posix")
    (top / "program" / "python").write_text("x")
    lay, asked, find = searcher(top, {str(top / "program" / "python")})
    with pytest.raises(install.NoUnoPython) as e:
        find(explicit="/nonexistent/python")
    assert asked == ["/nonexistent/python"] and e.value.tried[0][1] == "explicit"


def test_environment_override_is_explicit_too(fake):
    lay, asked, find = searcher(fake("posix"), {"/my/python"}, environ={"AOO_UNO_PYTHON": "/my/python"})
    res = find()
    assert res.path == "/my/python" and res.source == "explicit" and asked == ["/my/python"]


def test_failure_names_the_python_pyuno_needs(fake):
    lay, asked, find = searcher(fake("macos"), set(), linked="/nonexistent/Library/Frameworks/Python.framework/Versions/3.14/Python")
    with pytest.raises(install.NoUnoPython) as e:
        find()
    assert "/nonexistent/Library/Frameworks/Python.framework/Versions/3.14/Python (not found on this machine)" in str(e.value)
    assert e.value.tried and all(r == "No module named 'uno'" for _, _, r in e.value.tried)


def test_result_is_cached_per_install_and_pyuno_build(fake, tmp_path):
    cache = tmp_path / "uno-python.json"
    lay, asked, find = searcher(fake("posix"), {sys.executable})
    assert find(cache_file=cache).path == sys.executable and len(asked) == 1
    again = find(cache_file=cache)
    assert again.source.endswith("(cached)") and len(asked) == 1               # no second probe
    os.utime(lay.pyuno_module, (1, 1))                                         # a different pyuno build
    find(cache_file=cache)
    assert len(asked) == 2
    cache.write_text("not json")
    assert find(cache_file=cache).path == sys.executable                       # a damaged cache is ignored


def test_cache_is_not_used_for_an_explicit_choice(fake, tmp_path):
    cache = tmp_path / "c.json"
    lay, asked, find = searcher(fake("posix"), {sys.executable, "/other/python"})
    find(cache_file=cache)
    n = len(asked)
    assert find(cache_file=cache, explicit="/other/python").path == "/other/python" and len(asked) == n + 1


# ---- the probe itself ----------------------------------------------------------------------------------------------

def runner(returncode=0, stdout="3.14.0\n", stderr="", exc=None):
    seen = {}

    def run(cmd, **kw):
        seen.update(cmd=cmd, env=kw["env"])
        if exc:
            raise exc
        return subprocess.CompletedProcess(cmd, returncode, stdout, stderr)
    run.seen = seen
    return run


@pytest.mark.parametrize("kind, var", [("macos", "DYLD_FALLBACK_LIBRARY_PATH"), ("posix", "LD_LIBRARY_PATH"), ("windows", "PATH")])
def test_probe_environment(fake, kind, var):
    lay = install.find_layout(fake(kind))
    run = runner()
    assert install.probe("/p/python", lay, runner=run) == (True, "3.14.0")
    env = run.seen["env"]
    assert env["URE_BOOTSTRAP"] == "vnd.sun.star.pathname:" + str(lay.fundamental)
    assert env["PYTHONPATH"].split(os.pathsep)[0] == str(lay.program)
    assert env[var].split(os.pathsep)[0] == str(lay.program)
    assert run.seen["cmd"][:2] == ["/p/python", "-c"] and "import sys, uno, pyuno" in run.seen["cmd"][2]


def test_probe_failures(fake):
    lay = install.find_layout(fake("posix"))
    assert install.probe("p", lay, runner=runner(1, "", "Traceback\nImportError: no uno\n")) == (False, "ImportError: no uno")
    assert install.probe("p", lay, runner=runner(1, "", ""))[1] == "exit 1"
    assert "older than 3.11" in install.probe("p", lay, runner=runner(stdout="3.9.2\n"))[1]
    assert "unexpected output" in install.probe("p", lay, runner=runner(stdout="hello\n"))[1]
    assert "could not run" in install.probe("p", lay, runner=runner(exc=FileNotFoundError("p")))[1]
    assert "could not run" in install.probe("p", lay, runner=runner(exc=subprocess.TimeoutExpired("p", 1)))[1]
