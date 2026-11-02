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

"""proc.kill_tree(): the whole process tree dies, not just the leader."""
import os
import subprocess
import sys
import time

import pytest

from aootest import proc

pytestmark = [pytest.mark.selftest, pytest.mark.skipif(not proc.POSIX, reason="the tree kill is only exercised on POSIX here")]

PARENT = "import subprocess, sys, time\nc = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(120)'])\nprint(c.pid, flush=True)\ntime.sleep(120)\n"


def gone(pid, wait=5.0):
    end = time.time() + wait
    while time.time() < end:
        try:
            os.kill(pid, 0)
        except ProcessLookupError:
            return True
        time.sleep(0.05)
    return False


def test_kill_tree_kills_the_children_too():
    p = proc.spawn([sys.executable, "-c", PARENT], stdout=subprocess.PIPE)
    child = int(p.stdout.readline())
    os.kill(child, 0)                       # alive before
    proc.kill_tree(p)
    assert p.poll() is not None
    assert gone(child)


def test_plain_kill_would_leave_the_child(tmp_path):
    """Negative control: without the group kill the child survives, so the test above can fail."""
    p = proc.spawn([sys.executable, "-c", PARENT], stdout=subprocess.PIPE)
    child = int(p.stdout.readline())
    try:
        p.kill()
        p.wait(timeout=5)
        os.kill(child, 0)                   # still alive
    finally:
        os.kill(child, 9)
    assert gone(child)


def test_kill_tree_on_an_exited_process_is_harmless():
    p = proc.spawn([sys.executable, "-c", "pass"])
    p.wait(timeout=10)
    proc.kill_tree(p)
    assert p.returncode == 0


def test_the_session_is_separate_from_ours():
    p = proc.spawn([sys.executable, "-c", "import time; time.sleep(30)"])
    try:
        assert os.getpgid(p.pid) == p.pid != os.getpgrp()
    finally:
        proc.kill_tree(p)
