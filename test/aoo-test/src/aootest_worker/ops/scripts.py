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

"""Runs the large fixture scripts in aootest_worker/fixtures (copied from the ODF harness's fixtures/*/make.py) inside the worker.
They were written as stand-alone programs: `make.py <port> <outdir>`. Here they run under runpy with that argv, an `unohelp` that
hands them the worker's connection, and two guards for what cannot work from inside the worker: os._exit (the draw script ends
with it) and subprocess.run (the draw script's watchdog restarts the office with a script of the old harness). They do not run as
`__main__`: pyuno keeps its runtime in that module's dict, and runpy swaps it out while a script runs."""
import contextlib
import os
import runpy
import subprocess
import sys
import traceback
from pathlib import Path

from aootest_worker import unohelp

FIXTURES = Path(__file__).resolve().parent.parent / "fixtures"


def _no_exit(code=0):
    raise SystemExit(code)


def _no_run(*args, **kwargs):
    raise RuntimeError("a fixture script cannot restart the office from inside the worker")


def run_script(script, outdir, args=()):
    """Runs fixtures/<script> with `outdir` as its output directory and returns the names of the files it left there. The
    scripts put scratch files next to `outdir`, so `outdir` should be a directory of its own. What the script printed goes to
    `<outdir>/../<script>.log` as it runs, so a hung script leaves its log; a failing step is logged and does not stop the run, as in the old harness."""
    path = FIXTURES / script
    os.makedirs(outdir, exist_ok=True)
    logfile = Path(outdir).parent / (script + ".log")
    log = open(logfile, "w", buffering=1, encoding="utf-8")
    saved = (sys.argv, sys.modules.get("unohelp"), os._exit, subprocess.run)
    sys.argv = [str(path), "0", str(outdir)] + list(args)
    sys.modules["unohelp"] = unohelp
    os._exit, subprocess.run = _no_exit, _no_run
    code = 0
    try:
        with contextlib.redirect_stdout(log):
            runpy.run_path(str(path), run_name="fixture_script")
    except SystemExit as e:
        code = e.code if isinstance(e.code, int) else (0 if e.code is None else 1)
    except Exception:
        log.write(traceback.format_exc())
        code = "error"
        raise
    finally:
        sys.argv, mod, os._exit, subprocess.run = saved
        if mod is None:
            sys.modules.pop("unohelp", None)
        else:
            sys.modules["unohelp"] = mod
        log.write("exit %s\n" % code)
        log.close()
    return sorted(p.name for p in Path(outdir).iterdir() if p.is_file())
