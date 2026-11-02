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

"""Starting a process in its own session or process group, and killing it together with everything it started."""
import os
import signal
import subprocess

POSIX = os.name == "posix"


def spawn(cmd, **kw):
    """Popen in a new session (POSIX) or process group (Windows), so kill_tree() can reach the whole tree."""
    if POSIX:
        kw.setdefault("start_new_session", True)
    else:
        kw.setdefault("creationflags", subprocess.CREATE_NEW_PROCESS_GROUP)
    return subprocess.Popen(cmd, **kw)


def kill_tree(proc, wait=10):
    """Kills `proc` (which must come from spawn()) and its descendants, then reaps it.

    POSIX: SIGKILL to the process group, only while the leader is still alive (its pid cannot have been reused) and
    never to our own group. Windows: taskkill /T /F. A leader that has already exited leaves its children alone."""
    if proc.poll() is None:
        if POSIX:
            try:
                if os.getpgid(proc.pid) != os.getpgrp():
                    os.killpg(proc.pid, signal.SIGKILL)
                else:
                    proc.kill()
            except ProcessLookupError:
                pass
        else:
            subprocess.run(["taskkill", "/T", "/F", "/PID", str(proc.pid)], capture_output=True)
            if proc.poll() is None:
                proc.kill()
    try:
        proc.wait(timeout=wait)
    except subprocess.TimeoutExpired:
        pass
