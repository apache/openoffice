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

"""A running office and its worker for one ODF save mode: wires install, office and client into an OfficeSession."""
import os
import time
from pathlib import Path

import aootest_worker
from aootest import install, office
from aootest.client import WorkerClient, WorkerCrash, WorkerHang

SRC_DIR = str(Path(aootest_worker.__file__).resolve().parent.parent)


def worker_env(layout, base=None):
    """Environment for the worker's Python: the office program dir, URE_BOOTSTRAP, and this package on the path."""
    env = install.probe_env(layout, base)
    env["PYTHONPATH"] = os.pathsep.join([SRC_DIR, env["PYTHONPATH"]])
    return env


def uno_url(accept):
    return "uno:" + accept + "StarOffice.ComponentContext"


class ModeSession(office.OfficeSession):
    """OfficeSession for one mode (None = product default, or the Save/ODF/DefaultVersion number). The profile lives in
    `workdir/profile_<mode>` and is kept across restarts."""

    def __init__(self, layout, python, mode, workdir, connect="socket", max_restarts=5, connect_timeout=90.0, hello_timeout=60.0,
                 extra=None, tag=None):
        self.layout, self.python, self.mode = layout, python, mode
        self.name = ("default" if mode is None else str(mode)) + ("_" + tag if tag else "")
        self.workdir = Path(workdir)
        self.connect, self.connect_timeout, self.hello_timeout = connect, connect_timeout, hello_timeout
        self.profile_url = office.make_profile(self.workdir / ("profile_" + self.name), mode, extra)
        self.starts = 0
        self.started = []
        super().__init__(self._start_office_proc, self._start_worker_proc, max_restarts)

    def _start_office_proc(self):
        accept = office.accept_string(self.connect)
        self.starts += 1
        log = self.workdir / ("office_%s_%d.log" % (self.name, self.starts))
        off = office.Office(office.command(self.layout, self.profile_url, accept), log=str(log)).start()
        off.url = uno_url(accept)
        self.started.append(off)
        return off

    def _start_worker_proc(self, off):
        client = WorkerClient([self.python.path, "-m", "aootest_worker"], env=worker_env(self.layout), hello_timeout=self.hello_timeout)
        try:
            client.start()
            deadline = time.time() + self.connect_timeout
            while True:
                if not off.alive():
                    raise WorkerCrash("the office exited before the worker could connect", getattr(off._proc, "returncode", None))
                if client.call("connect", {"url": off.url, "wait": 2.0}, timeout=30)["connected"]:
                    return client
                if time.time() > deadline:
                    raise WorkerHang("no connection to the office within %gs" % self.connect_timeout)
        except BaseException:
            client.close()
            raise
