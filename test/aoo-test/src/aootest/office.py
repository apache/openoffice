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

"""Starting the office, its profile, and the session that restarts it after a hang or crash."""
import socket
import uuid
from pathlib import Path

from aootest import proc
from aootest.client import ProtocolError, WorkerCrash, WorkerHang

XCU_HEAD = ('<?xml version="1.0" encoding="UTF-8"?>\n<oor:items xmlns:oor="http://openoffice.org/2001/registry" '
            'xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">\n')


def registry_xcu(default_version=None, extra=None):
    """registrymodifications.xcu content. `default_version` is Save/ODF/DefaultVersion (3 = ODF 1.2 extended, 4 = 1.2, 5 = 1.3,
    6 = 1.3 extended; 0 leaves the product default). `extra` maps other Save/ODF boolean properties (UseSHA1InODF12, ...) to values."""
    values = {}
    if default_version is not None:
        values["DefaultVersion"] = "%d" % default_version
    for name, v in (extra or {}).items():
        values[name] = ("true" if v else "false") if isinstance(v, bool) else str(v)
    items = "".join('<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="%s" oor:op="fuse">'
                    '<value>%s</value></prop></item>\n' % kv for kv in values.items())
    return XCU_HEAD + items + "</oor:items>\n"


def make_profile(directory, default_version=None, extra=None):
    """Creates a user profile in `directory` (empty or absent; nothing is deleted) and returns its file URL."""
    d = Path(directory).resolve()
    if d.is_dir() and any(d.iterdir()):
        raise ValueError("profile directory is not empty: %s" % d)
    (d / "user").mkdir(parents=True, exist_ok=True)
    (d / "user" / "registrymodifications.xcu").write_text(registry_xcu(default_version, extra), encoding="utf-8")
    return d.as_uri()


def free_port():
    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def accept_string(connect="pipe", name=None, port=None):
    """The -accept= value: a uniquely named pipe, or a socket on `port` (a free one by default)."""
    if connect == "pipe":
        return "pipe,name=%s;urp;" % (name or "aootest_" + uuid.uuid4().hex[:12])
    if connect == "socket":
        return "socket,host=localhost,port=%d;urp;" % (port or free_port())
    raise ValueError("connect must be 'pipe' or 'socket'")


def command(layout, profile_url, accept, headless=True):
    """soffice command line. AOO takes single-dash options only and ignores `--headless`."""
    return ([str(layout.soffice)] + (["-headless"] if headless else []) +
            ["-invisible", "-nofirststartwizard", "-norestore", "-nologo", "-nodefault", "-nolockcheck",
             "-env:UserInstallation=" + profile_url, "-accept=" + accept])


class Office:
    """A running soffice. Readiness is the worker connecting to it, so there is nothing to poll here."""

    def __init__(self, cmd, env=None, log=None):
        self.cmd, self.env, self.log, self._proc = list(cmd), env, log, None

    def start(self):
        out = open(self.log, "ab") if self.log else None
        try:
            self._proc = proc.spawn(self.cmd, env=self.env, stdin=None, stdout=out, stderr=out)
        finally:
            if out:
                out.close()
        return self

    @property
    def pid(self):
        return self._proc.pid if self._proc else None

    def alive(self):
        return self._proc is not None and self._proc.poll() is None

    def kill(self):
        if self._proc is not None:
            proc.kill_tree(self._proc)


class SessionAborted(Exception):
    pass


class OfficeSession:
    """One office and one worker, started lazily. A hang, crash or protocol error tears both down, is re-raised to the
    caller, and the next call starts fresh; after more than `max_restarts` such failures the session is aborted.

    start_office() returns an object with kill(); start_worker(office) returns a started client with call() and close()."""

    def __init__(self, start_office, start_worker, max_restarts=5):
        self._start_office, self._start_worker, self.max_restarts = start_office, start_worker, max_restarts
        self.office = self.worker = None
        self.failures = 0
        self.aborted = False

    def _ensure(self):
        if self.aborted:
            raise SessionAborted("aborted after %d failures (limit %d)" % (self.failures, self.max_restarts))
        if self.worker is None:
            self.office = self._start_office()
            try:
                self.worker = self._start_worker(self.office)
            except BaseException as e:
                self._teardown()
                if isinstance(e, (WorkerHang, WorkerCrash, ProtocolError)):
                    self._failed()
                raise

    def _failed(self):
        self.failures += 1
        if self.failures > self.max_restarts:
            self.aborted = True

    def _teardown(self):
        w, o, self.worker, self.office = self.worker, self.office, None, None
        if w is not None:
            w.close()
        if o is not None:
            o.kill()

    def call(self, op, args=None, timeout=60.0):
        self._ensure()
        try:
            return self.worker.call(op, args, timeout)
        except (WorkerHang, WorkerCrash, ProtocolError):
            self._teardown()
            self._failed()
            raise

    def close(self):
        self._teardown()
