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

"""A stand-in for the real worker. usage: fake_worker.py <mode>. Modes other than 'good' misbehave in one specific way."""
import json
import os
import signal
import subprocess
import sys
import time

from aootest_worker.proto import Fail, serve

mode = sys.argv[1]


def hello(protocol=1):
    print(json.dumps({"hello": {"protocol": protocol, "python": "3.14.0", "pid": os.getpid()}}), flush=True)


def request():
    return json.loads(sys.stdin.readline())


def spawn_child():
    c = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(120)"])
    return {"pid": c.pid}


def chatter():
    print("this goes to stderr, not into the protocol")
    return {"printed": True}


OPS = {
    "echo": lambda **kw: kw,
    "add": lambda a, b: {"sum": a + b},
    "sleep": lambda seconds: time.sleep(seconds) or {},
    "fail": lambda kind, message="": (_ for _ in ()).throw(Fail(kind, message)),
    "boom": lambda: 1 / 0,
    "chatter": chatter,
    "spawn_child": spawn_child,
    "die139": lambda: os._exit(139),
    "office-gone": lambda: (_ for _ in ()).throw(Fail("office_dead", "DisposedException: cannot get value ServiceManager")),
}

if mode == "good":
    serve(OPS)
elif mode == "silent":
    time.sleep(120)
elif mode == "garbage-hello":
    print("this is not json", flush=True)
    time.sleep(120)
elif mode == "wrong-protocol":
    hello(99)
    time.sleep(120)
elif mode == "exit-before-hello":
    os._exit(3)
else:
    hello()
    req = request()
    if mode == "crash139":
        os._exit(139)
    elif mode == "sigkill":
        os.kill(os.getpid(), signal.SIGKILL)
    elif mode == "stderr-then-crash":
        print("fatal: the office bridge fell over", file=sys.stderr, flush=True)
        os._exit(7)
    elif mode == "garbage-reply":
        print("garbage", flush=True)
    elif mode == "not-an-object":
        print("[1, 2, 3]", flush=True)
    elif mode == "wrong-id":
        print(json.dumps({"id": req["id"] + 100, "ok": True, "result": {}}), flush=True)
    elif mode == "binary-reply":
        sys.stdout.buffer.write(b"\xff\xfe\x00\n")
        sys.stdout.buffer.flush()
    elif mode == "hang":
        pass
    else:
        sys.exit("unknown mode " + mode)
    time.sleep(120)
