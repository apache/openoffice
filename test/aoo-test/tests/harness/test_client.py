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

"""client.py against the fake worker: normal replies, error replies, and every way a worker can go wrong."""
import os
import time

import pytest

from aootest import client
from aootest.client import ProtocolError, WorkerCrash, WorkerError, WorkerHang

pytestmark = pytest.mark.selftest
POSIX = os.name == "posix"


def gone(pid, wait=5.0):
    end = time.time() + wait
    while time.time() < end:
        try:
            os.kill(pid, 0)
        except ProcessLookupError:
            return True
        time.sleep(0.05)
    return False


def test_round_trip(start_worker):
    w = start_worker()
    assert w.hello["protocol"] == 1 and w.hello["pid"] == w.pid
    assert w.call("echo", {"x": 1, "y": [1, 2]}) == {"x": 1, "y": [1, 2]}
    assert w.call("add", {"a": 2, "b": 3}) == {"sum": 5}
    assert w.call("ping") == {"pong": True}


def test_printing_in_the_worker_does_not_corrupt_the_protocol(start_worker):
    w = start_worker()
    assert w.call("chatter") == {"printed": True}
    assert w.call("echo", {"after": True}) == {"after": True}


def test_error_replies_keep_the_worker_usable(start_worker):
    w = start_worker()
    with pytest.raises(WorkerError) as e:
        w.call("fail", {"kind": "uno", "message": "the document would not load"})
    assert (e.value.kind, e.value.message) == ("uno", "the document would not load")
    with pytest.raises(WorkerError) as e:
        w.call("boom")
    assert e.value.kind == "python" and e.value.exc == "ZeroDivisionError" and "ZeroDivisionError" in e.value.traceback
    with pytest.raises(WorkerError) as e:
        w.call("no-such-op")
    assert e.value.kind == "bad_args"
    with pytest.raises(WorkerError) as e:
        w.call("add", {"a": 1})
    assert e.value.kind == "bad_args" and "missing" in e.value.message
    assert w.call("add", {"a": 1, "b": 1}) == {"sum": 2}


def test_close_ends_the_worker(start_worker):
    w = start_worker()
    p = w._proc
    w.close()
    assert p.poll() is not None


# ---- HANG ----------------------------------------------------------------------------------------------------------

def test_hang_is_reported_and_the_worker_killed(start_worker):
    w = start_worker("hang")
    t0 = time.time()
    with pytest.raises(WorkerHang, match="no answer within 1s"):
        w.call("echo", timeout=1)
    assert time.time() - t0 < 6
    assert w._proc.poll() is not None
    with pytest.raises(WorkerHang):                                   # a finished client stays finished
        w.call("echo")


@pytest.mark.skipif(not POSIX, reason="child process ids are only checked on POSIX")
def test_hang_kills_the_workers_children_too(start_worker):
    w = start_worker()
    child = w.call("spawn_child")["pid"]
    os.kill(child, 0)
    with pytest.raises(WorkerHang):
        w.call("sleep", {"seconds": 60}, timeout=1)
    assert gone(child)


def test_worker_that_never_says_hello(start_worker):
    with pytest.raises(WorkerHang, match="hello"):
        start_worker("silent", hello_timeout=1)


# ---- CRASH ---------------------------------------------------------------------------------------------------------

def test_exit_139(start_worker):
    w = start_worker("crash139")
    with pytest.raises(WorkerCrash) as e:
        w.call("echo")
    assert e.value.returncode == 139 and "128 + SIGSEGV" in str(e.value)


@pytest.mark.skipif(not POSIX, reason="signals")
def test_killed_by_a_signal(start_worker):
    w = start_worker("sigkill")
    with pytest.raises(WorkerCrash) as e:
        w.call("echo")
    assert e.value.returncode == -9 and "killed by SIGKILL" in str(e.value)


def test_stderr_tail_is_kept(start_worker):
    w = start_worker("stderr-then-crash")
    with pytest.raises(WorkerCrash) as e:
        w.call("echo")
    assert e.value.returncode == 7 and "the office bridge fell over" in str(e.value)


def test_office_dead_reply_is_a_crash_and_the_worker_is_finished(start_worker):
    w = start_worker()
    with pytest.raises(client.OfficeDead, match="ServiceManager") as e:
        w.call("office-gone")
    assert isinstance(e.value, WorkerCrash)
    with pytest.raises(client.OfficeDead):
        w.call("echo")
    assert w._proc.poll() is not None


def test_death_mid_call_in_the_good_worker(start_worker):
    w = start_worker()
    with pytest.raises(WorkerCrash) as e:
        w.call("die139")
    assert e.value.returncode == 139
    with pytest.raises(WorkerCrash):
        w.call("echo")


def test_exit_before_hello(start_worker):
    with pytest.raises(WorkerCrash) as e:
        start_worker("exit-before-hello")
    assert e.value.returncode == 3


# ---- garbage -------------------------------------------------------------------------------------------------------

@pytest.mark.parametrize("mode", ["garbage-reply", "not-an-object", "wrong-id", "binary-reply"])
def test_bad_replies_are_protocol_errors_and_the_worker_is_killed(start_worker, mode):
    w = start_worker(mode)
    with pytest.raises(ProtocolError):
        w.call("echo", timeout=5)
    assert w._proc.poll() is not None


@pytest.mark.parametrize("mode", ["garbage-hello", "wrong-protocol"])
def test_bad_hello(start_worker, mode):
    with pytest.raises(ProtocolError):
        start_worker(mode, hello_timeout=5)


@pytest.mark.parametrize("rc, text", [(0, "exit 0"), (1, "exit 1"), (7, "exit 7"), (139, "exit 139, 128 + SIGSEGV"), (-9, "killed by SIGKILL"),
                                      (-11, "killed by SIGSEGV"), (None, "no exit status")])
def test_describe_exit(rc, text):
    assert client.describe_exit(rc) == text
