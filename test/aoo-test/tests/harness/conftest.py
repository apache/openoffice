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

"""Fake install layouts and a helper to start the fake worker."""
import os
import sys
from pathlib import Path

import pytest

from aootest import client
from aootest.paths import ROOT

FAKE_WORKER = Path(__file__).with_name("fake_worker.py")

VERSIONRC = ("[Version]\nAllLanguages=en-US\nbuildid=510m1(Build:99000)\n%(base)sOOOPackageVersion=%(pkg)s\nProductBuildid=99000\n"
             "ProductMajor=%(major)s\nProductMinor=%(minor)s\nRevision=7d712b94e3\n"
             "UpdateUserAgent=<PRODUCT> (${buildid}; ${_OS}; ${_ARCH}; BundledLanguages=${AllLanguages})\n")


def touch(path, text="x"):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text)
    return path


def make_install(root, kind, base="5.1", pkg="5.1.0", major="510", minor="1", with_base=True):
    """A fake install of the given kind (macos, posix, windows) under `root`; returns what a user would point at."""
    rc = VERSIONRC % {"base": "OOOBaseVersion=%s\n" % base if with_base else "", "pkg": pkg, "major": major, "minor": minor}
    if kind == "macos":
        app = root / "OpenOffice5.app"
        prog = app / "Contents" / "program"
        touch(app / "Contents" / "MacOS" / "soffice")
        for n in ("soffice.bin", "unopkg.bin", "pyuno.so", "uno.py", "libpyuno.dylib"):
            touch(prog / n)
        os.symlink("../MacOS/soffice", prog / "soffice")
        os.symlink("unopkg.bin", prog / "unopkg")
        touch(prog / "versionrc", rc)
        touch(prog / "fundamentalrc", "[Bootstrap]\n")
        return app
    if kind == "posix":
        top = root / "openoffice5"
        prog = top / "program"
        for n in ("soffice", "soffice.bin", "unopkg", "unopkg.bin", "pyuno.so", "uno.py", "libpyuno.so"):
            touch(prog / n)
        touch(prog / "versionrc", rc)
        touch(prog / "fundamentalrc", "[Bootstrap]\n")
        return top
    top = root / "OpenOffice 5"
    prog = top / "program"
    for n in ("soffice.exe", "soffice.bin", "unopkg.com", "unopkg.exe", "pyuno.pyd", "uno.py"):
        touch(prog / n)
    touch(prog / "version.ini", rc)
    touch(prog / "fundamental.ini", "[Bootstrap]\n")
    return top


@pytest.fixture
def fake(tmp_path):
    """fake(kind, **version) -> the path a user would point at for a fake install of that kind."""
    return lambda kind, **kw: make_install(tmp_path, kind, **kw)


@pytest.fixture
def worker_env():
    return dict(os.environ, PYTHONPATH=str(ROOT / "src"))


@pytest.fixture
def start_worker(worker_env):
    """start_worker(mode, **client_kw) -> a started WorkerClient on the fake worker; all are closed at the end."""
    made = []

    def start(mode="good", **kw):
        c = client.WorkerClient([sys.executable, str(FAKE_WORKER), mode], env=worker_env, **kw)
        made.append(c)
        c.start()
        return c
    yield start
    for c in made:
        c.close()
