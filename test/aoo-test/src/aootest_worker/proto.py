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

"""The worker's request loop. Anything a handler prints goes to stderr so it cannot corrupt the protocol on stdout."""
import inspect
import json
import os
import sys
import time
import traceback

PROTOCOL = 1


class Fail(Exception):
    """An error a handler wants reported with a specific kind: uno, office_dead, bad_args or python."""

    def __init__(self, kind, message=""):
        super().__init__(message)
        self.kind = kind


def serve(ops, stdin=None, stdout=None):
    """Sends the hello, then answers requests until stdin closes or an op named 'shutdown' arrives."""
    out = stdout or sys.stdout
    inp = stdin or sys.stdin
    sys.stdout = sys.stderr

    def send(obj):
        out.write(json.dumps(obj) + "\n")
        out.flush()

    send({"hello": {"protocol": PROTOCOL, "python": "%d.%d.%d" % sys.version_info[:3], "pid": os.getpid()}})
    for line in inp:
        if not line.strip():
            continue
        try:
            req = json.loads(line)
            rid, op, args = req["id"], req["op"], req.get("args") or {}
        except (ValueError, KeyError, TypeError) as e:
            send({"id": None, "ok": False, "error": {"kind": "bad_args", "exc": type(e).__name__, "message": "unreadable request: %s" % e, "traceback": ""}})
            continue
        if op == "shutdown":
            send({"id": rid, "ok": True, "result": {}, "elapsed": 0.0})
            return
        t0 = time.time()
        try:
            if op == "ping":
                result = {"pong": True}
            elif op in ops:
                try:
                    inspect.signature(ops[op]).bind(**args)
                except TypeError as e:
                    raise Fail("bad_args", "%s: %s" % (op, e))
                result = ops[op](**args)
            else:
                raise Fail("bad_args", "unknown op %r" % op)
            send({"id": rid, "ok": True, "result": result, "elapsed": round(time.time() - t0, 3)})
        except Fail as e:
            send({"id": rid, "ok": False, "error": {"kind": e.kind, "exc": "Fail", "message": str(e), "traceback": ""}})
        except Exception as e:
            send({"id": rid, "ok": False, "error": {"kind": "python", "exc": type(e).__name__, "message": str(e), "traceback": traceback.format_exc()}})
