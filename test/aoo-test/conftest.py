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

"""Shared pytest configuration for the AOO test suite."""

import os

import pytest

from aootest import documents, expect, install, pipeline, session, twins
from aootest.odfversion import SAVE_MODES, odf_versions


def pytest_addoption(parser):
    g = parser.getgroup("aoo", "AOO test suite")
    g.addoption("--office", default=None, help="an AOO 5+ install: .app, install root, program dir or soffice (default: $AOO_OFFICE)")
    g.addoption("--uno-python", default=None, help="Python that can import uno (default: $AOO_UNO_PYTHON, else probed)")
    g.addoption("--connect", choices=("socket", "pipe"), default="socket", help="how the worker reaches the office")
    g.addoption("--start-timeout", type=float, default=240.0, help="seconds an office may take to start and accept a connection (default %(default)s)")
    g.addoption("--max-restarts", type=int, default=5, help="office restarts allowed per mode before the session aborts")
    g.addoption("--require-office", action="store_true", help="fail instead of skipping when no usable office is found")
    g.addoption("--documents", default=None, help="a directory of ODF documents to run through every save mode, in addition to the ones the office ships")
    g.addoption("--shipped-limit", type=int, default=documents.DEFAULT_LIMIT, help="how many of the templates and documents shipped with the office to use (default %(default)s)")
    g.addoption("--twin-corpus", default=None, help="a directory with v12, v13, v12ext and v13ext subdirectories holding the same documents saved as ODF 1.2 and 1.3 by another producer (default: $AOO_TWIN_CORPUS)")
    g.addoption("--strict-expect", action="store_true", help="an item that a data/expect row expects to fail must fail (XPASS becomes a failure)")
    g.addoption("--require-odf13", action="store_true", help="fail instead of skipping the ODF 1.3 save modes when the office cannot write 1.3")


def _unavailable(config, message):
    if config.getoption("--require-office"):
        pytest.fail(message, pytrace=False)
    pytest.skip(message)


@pytest.fixture(scope="session")
def aoo_layout(pytestconfig):
    """The install named by --office or AOO_OFFICE. There is no search of default places: an office is only started on request."""
    location = office_location(pytestconfig)
    if not location:
        _unavailable(pytestconfig, "set AOO_OFFICE or --office to an installed AOO 5 to run office tests")
    try:
        return install.discover(location)
    except install.LayoutError as e:
        _unavailable(pytestconfig, "no usable office at %s: %s" % (location, e))


@pytest.fixture(scope="session")
def aoo_python(pytestconfig, aoo_layout, tmp_path_factory):
    try:
        return install.find_uno_python(aoo_layout, explicit=pytestconfig.getoption("--uno-python"),
                                       cache_file=tmp_path_factory.getbasetemp() / "uno-python.json")
    except install.NoUnoPython as e:
        _unavailable(pytestconfig, "no UNO Python for this install: %s; tried %s" % (e, e.tried))


class Offices:
    """One lazily started office and worker per ODF mode (None = product default, or a Save/ODF/DefaultVersion number)."""

    def __init__(self, layout, python, workdir, connect, max_restarts, start_timeout=240.0):
        self.layout, self.python, self.workdir, self.connect, self.max_restarts = layout, python, workdir, connect, max_restarts
        self.start_timeout = start_timeout
        self.sessions = {}

    def get(self, mode=None, extra=None, tag=None):
        """The session for `mode`; `extra` (other Save/ODF settings) needs a `tag` that names it."""
        key = (mode, tag)
        if key not in self.sessions:
            self.sessions[key] = session.ModeSession(self.layout, self.python, mode, self.workdir, self.connect, self.max_restarts,
                                                     connect_timeout=self.start_timeout, extra=extra, tag=tag)
        return self.sessions[key]

    def close(self):
        for s in self.sessions.values():
            s.close()
        left = [(s.name, o.pid) for s in self.sessions.values() for o in s.started if o.alive()]
        assert not left, "offices still running after the session: %s" % left


@pytest.fixture(scope="session")
def aoo_offices(pytestconfig, aoo_layout, aoo_python, tmp_path_factory):
    offices = Offices(aoo_layout, aoo_python, tmp_path_factory.mktemp("aoo"), pytestconfig.getoption("--connect"),
                      pytestconfig.getoption("--max-restarts"), pytestconfig.getoption("--start-timeout"))
    yield offices
    offices.close()


def office_location(config):
    return config.getoption("--office") or os.environ.get("AOO_OFFICE")


@pytest.fixture(scope="session")
def odf_runs(pytestconfig, aoo_offices, tmp_path_factory):
    """{mode name: ModeRun}, created on first use: `odf_runs("strict")`."""
    work = tmp_path_factory.mktemp("odf")
    runs = {}
    sources = documents.from_config(pytestconfig)
    # the sources are made in ODF 1.2 extended mode, which keeps every feature the reload checks look for
    prepared = documents.Prepared(aoo_offices.get(SAVE_MODES["ext"]), work / "src")

    def get(name):
        if name not in runs:
            runs[name] = pipeline.ModeRun(name, aoo_offices.get(SAVE_MODES[name]), work, sources, prepared)
        return runs[name]
    return get


@pytest.fixture(scope="session")
def twin_run(pytestconfig, aoo_offices, tmp_path_factory):
    """The TwinRun over the corpus named by --twin-corpus or AOO_TWIN_CORPUS; skips when there is none."""
    root = pytestconfig.getoption("--twin-corpus") or os.environ.get("AOO_TWIN_CORPUS")
    if not root:
        pytest.skip("set AOO_TWIN_CORPUS or --twin-corpus to a corpus of ODF 1.2 and 1.3 twins to run the import twin tests")
    problem = twins.corpus_problem(root)
    if problem:
        pytest.fail("twin corpus: " + problem, pytrace=False)
    return twins.TwinRun(aoo_offices.get(None), root, tmp_path_factory.mktemp("twins"))


_EXPECT_ROWS = []


def pytest_collection_modifyitems(config, items):
    rows = _EXPECT_ROWS[:] or expect.load()
    _EXPECT_ROWS[:] = rows
    strict = config.getoption("--strict-expect")
    for item in items:
        row = expect.match(rows, item.name)
        if row is None:
            continue
        row.used += 1
        if row.kind in ("xfail", "flaky"):
            item.add_marker(pytest.mark.xfail(reason=row.reason, strict=strict and row.kind == "xfail"))
        else:
            item.add_marker(pytest.mark.skip(reason=row.reason))


@pytest.fixture(scope="session")
def odf13(aoo_offices, tmp_path_factory):
    """Whether this build writes ODF 1.3 in save mode 5: builds from before the ODF 1.3 work ignore the setting."""
    dst = tmp_path_factory.mktemp("odf13") / "probe.odt"
    aoo_offices.get(5).call("new_and_store", {"app": "writer", "dst": str(dst), "filter": "writer8"}, timeout=120)
    return odf_versions(dst) == ("1.3", "1.3")


@pytest.fixture
def need_mode(odf13, pytestconfig):
    """need_mode(mode): skip (or fail with --require-odf13) when the build cannot write that save mode (names or numbers)."""
    def check(mode):
        number = SAVE_MODES.get(mode, mode)
        if number in (5, 6) and not odf13:
            message = "this build does not write ODF 1.3 (save mode %s)" % mode
            if pytestconfig.getoption("--require-odf13"):
                pytest.fail(message, pytrace=False)
            pytest.skip(message)
    return check


def pytest_terminal_summary(terminalreporter, config):
    unused = [r for r in _EXPECT_ROWS if not r.used]
    if unused and not config.option.keyword and not config.args[1:]:
        terminalreporter.write_line("expectation rows that matched no collected item: %d (first: %s at %s)" % (len(unused), unused[0].pattern, unused[0].origin))
