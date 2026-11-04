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

"""office.py: profile, command line, process handling, and the session that restarts after failures."""
import os
import sys
import time
import xml.etree.ElementTree as ET

import pytest

from aootest import client, office
from aootest.client import WorkerCrash, WorkerHang
from aootest.install import find_layout

pytestmark = pytest.mark.selftest
POSIX = os.name == "posix"
NS = {"oor": "http://openoffice.org/2001/registry"}


@pytest.mark.parametrize("version", [0, 3, 4, 5, 6])
def test_registry_xcu_sets_the_odf_version(version):
    root = ET.fromstring(office.registry_xcu(version))
    item = root.find("item")
    assert item.get("{http://openoffice.org/2001/registry}path") == "/org.openoffice.Office.Common/Save/ODF"
    prop = item.find("prop")
    assert prop.get("{http://openoffice.org/2001/registry}name") == "DefaultVersion" and prop.find("value").text == str(version)


def test_registry_xcu_without_a_version_changes_nothing():
    assert ET.fromstring(office.registry_xcu()).findall("item") == []


def test_make_profile(tmp_path):
    url = office.make_profile(tmp_path / "p", 4)
    assert url.startswith("file:///") and url.endswith("/p")
    assert "<value>4</value>" in (tmp_path / "p" / "user" / "registrymodifications.xcu").read_text()
    with pytest.raises(ValueError, match="not empty"):
        office.make_profile(tmp_path / "p", 4)
    (tmp_path / "present").mkdir()
    assert office.make_profile(tmp_path / "present").startswith("file:///")           # an empty directory is fine


def test_accept_strings_are_unique_pipes_or_sockets():
    a, b = office.accept_string(), office.accept_string()
    assert a != b and a.startswith("pipe,name=aootest_") and a.endswith(";urp;")
    assert office.accept_string("socket", port=2002) == "socket,host=localhost,port=2002;urp;"
    assert 1024 < office.free_port() < 65536
    with pytest.raises(ValueError):
        office.accept_string("fax")


def test_command_line(fake):
    lay = find_layout(fake("macos"))
    cmd = office.command(lay, "file:///tmp/prof", "pipe,name=x;urp;")
    assert cmd[0] == str(lay.soffice)
    assert "-headless" in cmd and "-invisible" in cmd and "-env:UserInstallation=file:///tmp/prof" in cmd and cmd[-1] == "-accept=pipe,name=x;urp;"
    assert not [a for a in cmd[1:] if a.startswith("--")]                  # AOO ignores double-dash options
    assert "-headless" not in office.command(lay, "file:///tmp/prof", "pipe,name=x;urp;", headless=False)
    assert "-invisible" in office.command(lay, "file:///tmp/prof", "pipe,name=x;urp;", headless=False)


FAKE_OFFICE = "import subprocess, sys, time\nsubprocess.Popen([sys.executable, '-c', 'import time; time.sleep(120)'])\nprint('started', flush=True)\ntime.sleep(120)\n"


@pytest.mark.skipif(not POSIX, reason="process ids are only checked on POSIX")
def test_office_kill_takes_the_whole_tree(tmp_path):
    o = office.Office([sys.executable, "-c", FAKE_OFFICE], log=tmp_path / "office.log").start()
    deadline = time.time() + 5
    while time.time() < deadline and not (tmp_path / "office.log").read_text():
        time.sleep(0.05)
    assert o.alive() and "started" in (tmp_path / "office.log").read_text()
    o.kill()
    assert not o.alive()


class FakeOffice:
    def __init__(self):
        self.killed = False

    def kill(self):
        self.killed = True


def make_session(start_worker, modes, max_restarts=5, hello_timeout=5):
    """A session whose successive workers behave as listed in `modes`."""
    offices, workers, it = [], [], iter(modes)

    def start_office():
        offices.append(FakeOffice())
        return offices[-1]

    def start_w(off):
        w = start_worker(next(it), hello_timeout=hello_timeout)
        workers.append(w)
        return w
    return office.OfficeSession(start_office, start_w, max_restarts), offices, workers


def test_session_starts_lazily_and_reuses_the_worker(start_worker):
    s, offices, workers = make_session(start_worker, ["good"])
    assert offices == []
    assert s.call("add", {"a": 1, "b": 2}) == {"sum": 3} and s.call("add", {"a": 2, "b": 2}) == {"sum": 4}
    assert len(offices) == len(workers) == 1 and s.failures == 0
    s.close()
    assert offices[0].killed and workers[0]._proc.poll() is not None


def test_session_restarts_after_a_hang_and_the_next_test_passes(start_worker):
    s, offices, workers = make_session(start_worker, ["hang", "good"])
    with pytest.raises(WorkerHang):
        s.call("echo", timeout=1)
    assert offices[0].killed and s.failures == 1 and not s.aborted
    assert s.call("add", {"a": 1, "b": 1}) == {"sum": 2}               # a fresh office and worker
    assert len(offices) == 2 and not offices[1].killed
    s.close()


def test_session_restarts_after_a_crash(start_worker):
    s, offices, workers = make_session(start_worker, ["crash139", "good"])
    with pytest.raises(WorkerCrash):
        s.call("echo")
    assert s.call("echo", {"k": 1}) == {"k": 1} and s.failures == 1
    s.close()


def test_session_restarts_after_the_office_reports_itself_dead(start_worker):
    s, offices, workers = make_session(start_worker, ["good", "good"])
    with pytest.raises(client.OfficeDead):
        s.call("office-gone")
    assert offices[0].killed and s.failures == 1
    assert s.call("add", {"a": 1, "b": 2}) == {"sum": 3} and len(offices) == 2
    s.close()


def test_session_aborts_after_too_many_failures(start_worker):
    s, offices, workers = make_session(start_worker, ["crash139"] * 5, max_restarts=2)
    for _ in range(3):
        with pytest.raises(WorkerCrash):
            s.call("echo")
    assert s.aborted and s.failures == 3
    with pytest.raises(office.SessionAborted, match="limit 2"):
        s.call("echo")
    assert len(offices) == 3 and all(o.killed for o in offices) and all(w._proc.poll() is not None for w in workers)


def test_a_worker_that_cannot_start_counts_as_a_failure(start_worker):
    s, offices, workers = make_session(start_worker, ["exit-before-hello", "good"], hello_timeout=5)
    with pytest.raises(WorkerCrash):
        s.call("echo")
    assert s.failures == 1 and offices[0].killed
    assert s.call("add", {"a": 5, "b": 5}) == {"sum": 10}
    s.close()


def test_an_error_reply_is_not_a_failure(start_worker):
    s, offices, workers = make_session(start_worker, ["good"])
    with pytest.raises(client.WorkerError):
        s.call("boom")
    assert s.failures == 0 and not offices[0].killed
    s.close()
