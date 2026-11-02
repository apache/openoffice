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

"""Host side of the worker protocol: JSON Lines over the worker's stdin and stdout, with a host-enforced timeout per call.

The worker runs under whichever Python can import uno (aootest_worker). It sends {"hello": {...}} first; each request
{"id", "op", "args", "timeout"} gets one reply {"id", "ok": true, "result"} or {"id", "ok": false, "error": {...}}.
Failures are distinguished: WorkerError (the worker answered with an error and is still usable), WorkerHang (no answer
in time; the process tree was killed), WorkerCrash (the process ended; OfficeDead when it reports its office gone) and ProtocolError (it said something that is not
the protocol; killed). After the last three the client is finished and a new one must be started.
"""
import collections
import itertools
import json
import queue
import signal
import subprocess
import threading

from aootest import proc

PROTOCOL = 1
_EOF = object()


class WorkerFailure(Exception):
    pass


class WorkerError(WorkerFailure):
    def __init__(self, kind, message="", exc="", traceback=""):
        super().__init__("%s: %s" % (kind, message))
        self.kind, self.message, self.exc, self.traceback = kind, message, exc, traceback


class WorkerHang(WorkerFailure):
    pass


class WorkerCrash(WorkerFailure):
    def __init__(self, message, returncode=None, stderr_tail=""):
        super().__init__("%s (%s)%s" % (message, describe_exit(returncode), "\n" + stderr_tail if stderr_tail else ""))
        self.returncode, self.stderr_tail = returncode, stderr_tail


class OfficeDead(WorkerCrash):
    """The worker is fine but its office is gone (the bridge was disposed). Handled as a crash: both are torn down."""

    def __init__(self, message):
        super().__init__("office died: " + message)


class ProtocolError(WorkerFailure):
    pass


def describe_exit(rc):
    if rc is None:
        return "no exit status"
    if rc == 0 or 0 < rc <= 128:
        return "exit %d" % rc
    n = -rc if rc < 0 else rc - 128
    try:
        name = signal.Signals(n).name
    except ValueError:
        name = "signal %d" % n
    return "killed by %s" % name if rc < 0 else "exit %d, 128 + %s" % (rc, name)


class WorkerClient:
    def __init__(self, cmd, env=None, cwd=None, hello_timeout=30.0):
        self.cmd, self.env, self.cwd, self.hello_timeout = list(cmd), env, cwd, hello_timeout
        self.hello = None
        self._proc = None
        self._lines = queue.Queue()
        self._stderr = collections.deque(maxlen=50)
        self._ids = itertools.count(1)
        self._dead = None

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, *exc):
        self.close()

    @property
    def pid(self):
        return self._proc.pid if self._proc else None

    def start(self):
        self._proc = proc.spawn(self.cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=self.env, cwd=self.cwd)
        threading.Thread(target=self._read_stdout, daemon=True).start()
        threading.Thread(target=self._read_stderr, daemon=True).start()
        line = self._next("hello", self.hello_timeout)
        msg = self._parse(line, "hello")
        h = msg.get("hello")
        if not isinstance(h, dict) or h.get("protocol") != PROTOCOL:
            self._fail_protocol("expected a hello for protocol %d, got %r" % (PROTOCOL, msg))
        self.hello = h
        return self

    def _read_stdout(self):
        for raw in self._proc.stdout:
            self._lines.put(raw)
        self._lines.put(_EOF)

    def _read_stderr(self):
        for raw in self._proc.stderr:
            self._stderr.append(raw.decode("utf-8", "replace").rstrip())

    def _tail(self):
        return "\n".join(list(self._stderr)[-10:])

    def _next(self, what, timeout):
        try:
            line = self._lines.get(timeout=timeout)
        except queue.Empty:
            self._kill()
            self._dead = WorkerHang("%s: no answer within %gs; worker killed" % (what, timeout))
            raise self._dead
        if line is _EOF:
            self._lines.put(_EOF)
            self._crash("worker ended while waiting for %s" % what)
        return line

    def _parse(self, line, what):
        try:
            msg = json.loads(line.decode("utf-8"))
        except (UnicodeDecodeError, ValueError):
            self._fail_protocol("%s: not a JSON line: %r" % (what, line[:80]))
        if not isinstance(msg, dict):
            self._fail_protocol("%s: not a JSON object: %r" % (what, line[:80]))
        return msg

    def _kill(self):
        proc.kill_tree(self._proc)

    def _fail_protocol(self, message):
        self._kill()
        self._dead = ProtocolError(message)
        raise self._dead

    def _crash(self, message):
        try:
            self._proc.wait(timeout=5)
        except Exception:
            self._kill()
        self._dead = WorkerCrash(message, self._proc.returncode, self._tail())
        raise self._dead

    def call(self, op, args=None, timeout=60.0):
        """Runs `op` in the worker and returns its result; see the module docstring for what can be raised."""
        if self._dead:
            raise self._dead
        if self._proc is None:
            raise RuntimeError("client not started")
        rid = next(self._ids)
        data = json.dumps({"id": rid, "op": op, "args": args or {}, "timeout": timeout}).encode("utf-8") + b"\n"
        try:
            self._proc.stdin.write(data)
            self._proc.stdin.flush()
        except (BrokenPipeError, OSError):
            self._crash("worker is not reading requests")
        msg = self._parse(self._next(op, timeout), op)
        if msg.get("id") != rid or not isinstance(msg.get("ok"), bool):
            self._fail_protocol("%s: reply does not match request %d: %r" % (op, rid, str(msg)[:120]))
        if msg["ok"]:
            return msg.get("result")
        err = msg.get("error") if isinstance(msg.get("error"), dict) else {}
        if err.get("kind") == "office_dead":
            self._kill()
            self._dead = OfficeDead(err.get("message", ""))
            raise self._dead
        raise WorkerError(err.get("kind", "python"), err.get("message", ""), err.get("exc", ""), err.get("traceback", ""))

    def close(self):
        """Asks the worker to shut down, then kills whatever is left."""
        if self._proc is None:
            return
        if self._dead is None and self._proc.poll() is None:
            try:
                self.call("shutdown", timeout=5)
            except WorkerFailure:
                pass
        self._kill()
        for s in (self._proc.stdin, self._proc.stdout, self._proc.stderr):
            try:
                s.close()
            except OSError:
                pass
